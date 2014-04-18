#include "ofxAudioUnitDSPNode.h"
#include "ofxAudioUnitBase.h"
#include "ofxAudioUnitUtils.h"
#include "TPCircularBuffer.h"

// a passthru render callback which copies the rendered samples in the process
static OSStatus RenderAndCopy(void * inRefCon,
							  AudioUnitRenderActionFlags *	ioActionFlags,
							  const AudioTimeStamp *	inTimeStamp,
							  UInt32 inBusNumber,
							  UInt32	inNumberFrames,
							  AudioBufferList * ioData);

// a render callback which just outputs silence and doesn't require a context
static OSStatus SilentRenderCallback(void * inRefCon,
									 AudioUnitRenderActionFlags *	ioActionFlags,
									 const AudioTimeStamp *	inTimeStamp,
									 UInt32 inBusNumber,
									 UInt32	inNumberFrames,
									 AudioBufferList * ioData);

typedef enum
{
	NodeSourceNone,
	NodeSourceUnit,
	NodeSourceCallback
}
ofxAudioUnitDSPNodeSourceType;

struct DSPNodeContext
{
	ofxAudioUnitDSPNodeSourceType sourceType;
	ofxAudioUnit * sourceUnit;
	UInt32 sourceBus;
	AURenderCallbackStruct sourceCallback;
	AURenderCallbackStruct processCallback;
	std::vector<TPCircularBuffer> circularBuffers;
	ofMutex bufferMutex;
	
	DSPNodeContext()
	: sourceBus(0)
	, sourceType(NodeSourceNone)
	, sourceCallback((AURenderCallbackStruct){0})
	, processCallback((AURenderCallbackStruct){0})
	, sourceUnit(NULL)
	, _bufferSize(0)
	{ }
	
	void setCircularBufferSize(UInt32 bufferCount, unsigned int samplesToBuffer) {
		if(bufferCount != circularBuffers.size() || samplesToBuffer != _bufferSize) {
			bufferMutex.lock();
			{
				for(int i = 0; i < circularBuffers.size(); i++) {
					TPCircularBufferCleanup(&circularBuffers[i]);
				}
				
				circularBuffers.resize(bufferCount);
				
				for(int i = 0; i < circularBuffers.size(); i++) {
					TPCircularBufferInit(&circularBuffers[i], samplesToBuffer * sizeof(AudioUnitSampleType));
				}
				_bufferSize = samplesToBuffer;
			}
			bufferMutex.unlock();
		}
	}
	
private:
	unsigned int _bufferSize;
};

struct ofxAudioUnitDSPNode::NodeImpl
{
	NodeImpl(unsigned int samplesToBuffer, unsigned int channelsToBuffer)
	: samplesToBuffer(samplesToBuffer)
	, channelsToBuffer(channelsToBuffer)
	{
		
	}
	
	DSPNodeContext ctx;
	unsigned int samplesToBuffer;
	unsigned int channelsToBuffer;
};

// ----------------------------------------------------------
ofxAudioUnitDSPNode::ofxAudioUnitDSPNode(unsigned int samplesToBuffer)
: _impl(new NodeImpl(samplesToBuffer, 2))
// ----------------------------------------------------------
{
	
}

// ----------------------------------------------------------
ofxAudioUnitDSPNode::~ofxAudioUnitDSPNode()
// ----------------------------------------------------------
{
	for(int i = 0; i < _impl->ctx.circularBuffers.size(); i++) {
		TPCircularBufferCleanup(&_impl->ctx.circularBuffers[i]);
	}
}

#pragma mark - Connections

// ----------------------------------------------------------
ofxAudioUnit& ofxAudioUnitDSPNode::connectTo(ofxAudioUnit &destination, int destinationBus, int sourceBus)
// ----------------------------------------------------------
{
	// checking to see if we have a source, and if that source is also valid
	if((_impl->ctx.sourceType == NodeSourceNone) ||
	   (_impl->ctx.sourceType == NodeSourceUnit     && !_impl->ctx.sourceUnit) ||
	   (_impl->ctx.sourceType == NodeSourceCallback && !_impl->ctx.sourceCallback.inputProc))
	{
		std::cout << "Tap can't be connected without a source" << std::endl;
		AURenderCallbackStruct silentCallback = {SilentRenderCallback};
		destination.setRenderCallback(silentCallback);
		return destination;
	}
	
	_impl->ctx.sourceBus = sourceBus;
	AURenderCallbackStruct callback = {RenderAndCopy, &_impl->ctx};
	destination.setRenderCallback(callback, destinationBus);
	return destination;
}

// ----------------------------------------------------------
ofxAudioUnitDSPNode& ofxAudioUnitDSPNode::connectTo(ofxAudioUnitDSPNode &destination, int destinationBus, int sourceBus)
// ----------------------------------------------------------
{
	_impl->ctx.sourceBus = sourceBus;
	AURenderCallbackStruct callback = {RenderAndCopy, &_impl->ctx};
	destination.setSource(callback);
	return destination;
}

// ----------------------------------------------------------
ofxAudioUnit& ofxAudioUnitDSPNode::operator>>(ofxAudioUnit &destination)
// ----------------------------------------------------------
{
	return connectTo(destination);
}

// ----------------------------------------------------------
void ofxAudioUnitDSPNode::setSource(ofxAudioUnit * source)
// ----------------------------------------------------------
{
	_impl->ctx.sourceUnit = source;
	_impl->ctx.sourceType = NodeSourceUnit;
	
	AudioStreamBasicDescription ASBD = {0};
	UInt32 ASBD_size = sizeof(ASBD);
	
	OFXAU_PRINT(AudioUnitGetProperty(source->getUnit(),
									 kAudioUnitProperty_StreamFormat,
									 kAudioUnitScope_Output,
									 0,
									 &ASBD,
									 &ASBD_size),
				"getting tap source's ASBD");
	
	_impl->channelsToBuffer = ASBD.mChannelsPerFrame;
	setBufferSize(_impl->samplesToBuffer);
}

// ----------------------------------------------------------
void ofxAudioUnitDSPNode::setSource(AURenderCallbackStruct callback, UInt32 channels)
// ----------------------------------------------------------
{
	_impl->ctx.sourceCallback = callback;
	_impl->ctx.sourceType = NodeSourceCallback;
	_impl->channelsToBuffer = channels;
	setBufferSize(_impl->samplesToBuffer);
}

// ----------------------------------------------------------
AudioStreamBasicDescription ofxAudioUnitDSPNode::getSourceASBD(int sourceBus) const
// ----------------------------------------------------------
{
	AudioStreamBasicDescription ASBD = {0};
	
	if(_impl->ctx.sourceType == NodeSourceUnit && _impl->ctx.sourceUnit != NULL) {
		UInt32 dataSize = sizeof(ASBD);
		OSStatus s = AudioUnitGetProperty(*(_impl->ctx.sourceUnit),
										  kAudioUnitProperty_StreamFormat,
										  kAudioUnitScope_Output,
										  sourceBus,
										  &ASBD,
										  &dataSize);
		if(s != noErr) {
			ASBD = (AudioStreamBasicDescription){0};
		}
	}
	
	return ASBD;
}

#pragma mark - Buffer Size

// ----------------------------------------------------------
void ofxAudioUnitDSPNode::setBufferSize(unsigned int samplesToBuffer)
// ----------------------------------------------------------
{
	_impl->samplesToBuffer = samplesToBuffer;
	_impl->ctx.setCircularBufferSize(_impl->channelsToBuffer, _impl->samplesToBuffer);
}

// ----------------------------------------------------------
unsigned int ofxAudioUnitDSPNode::getBufferSize() const
// ----------------------------------------------------------
{
	return _impl->samplesToBuffer;
}

#pragma mark - Getting Samples

// ----------------------------------------------------------
void ExtractSamplesFromCircularBuffer(std::vector<AudioUnitSampleType> &outBuffer, TPCircularBuffer * circularBuffer)
// ----------------------------------------------------------
{
	if(!circularBuffer) {
		outBuffer.clear();
	} else {
		int32_t circBufferSize;
		AudioUnitSampleType * circBufferTail = (AudioUnitSampleType *)TPCircularBufferTail(circularBuffer, &circBufferSize);
		AudioUnitSampleType * circBufferHead = circBufferTail + (circBufferSize / sizeof(AudioUnitSampleType));
		outBuffer.assign(circBufferTail, circBufferHead);
	}
}

void ofxAudioUnitDSPNode::getSamplesFromChannel(std::vector<AudioUnitSampleType> &samples, unsigned int channel) const
{
	if(_impl->ctx.circularBuffers.size() > channel) {
		ExtractSamplesFromCircularBuffer(samples, &_impl->ctx.circularBuffers[channel]);
	} else {
		samples.clear();
	}
}

void ofxAudioUnitDSPNode::setProcessCallback(AURenderCallbackStruct processCallback)
{
	_impl->ctx.bufferMutex.lock();
	_impl->ctx.processCallback = processCallback;
	_impl->ctx.bufferMutex.unlock();
}

#pragma mark - Render callbacks

inline void CopyAudioBufferIntoCircularBuffer(TPCircularBuffer * circBuffer, const AudioBuffer &audioBuffer)
{
	int32_t availableBytesInCircBuffer;
	TPCircularBufferHead(circBuffer, &availableBytesInCircBuffer);
	
	if(availableBytesInCircBuffer < audioBuffer.mDataByteSize) {
		TPCircularBufferConsume(circBuffer, audioBuffer.mDataByteSize - availableBytesInCircBuffer);
	}
	
	TPCircularBufferProduceBytes(circBuffer, audioBuffer.mData, audioBuffer.mDataByteSize);
}

// ----------------------------------------------------------
OSStatus RenderAndCopy(void * inRefCon,
					   AudioUnitRenderActionFlags * ioActionFlags,
					   const AudioTimeStamp * inTimeStamp,
					   UInt32 inBusNumber,
					   UInt32 inNumberFrames,
					   AudioBufferList * ioData)
{
	DSPNodeContext * ctx = static_cast<DSPNodeContext *>(inRefCon);
	
	OSStatus status;
	
	if(ctx->sourceType == NodeSourceUnit && ctx->sourceUnit->getUnit()) {
		status = ctx->sourceUnit->render(ioActionFlags, inTimeStamp, ctx->sourceBus, inNumberFrames, ioData);
	} else if(ctx->sourceType == NodeSourceCallback) {
		status = (ctx->sourceCallback.inputProc)(ctx->sourceCallback.inputProcRefCon,
												 ioActionFlags,
												 inTimeStamp,
												 ctx->sourceBus,
												 inNumberFrames,
												 ioData);
	} else {
		// if we don't have a source, render silence (or else you'll get an extremely loud
		// buzzing noise when we attempt to render a NULL unit. Ow.)
		status = SilentRenderCallback(inRefCon, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData);
	}
	
	if(ctx->processCallback.inputProc) {
		(ctx->processCallback.inputProc)(ctx->processCallback.inputProcRefCon,
										 ioActionFlags,
										 inTimeStamp,
										 ctx->sourceBus,
										 inNumberFrames,
										 ioData);
	}
	
	if(ctx->bufferMutex.tryLock()) {
		if(status == noErr) {
			const size_t buffersToCopy = min(ctx->circularBuffers.size(), ioData->mNumberBuffers);
			
			for(int i = 0; i < buffersToCopy; i++) {
				CopyAudioBufferIntoCircularBuffer(&ctx->circularBuffers[i], ioData->mBuffers[i]);
			}
		}
		ctx->bufferMutex.unlock();
	}
	
	return status;
}

OSStatus SilentRenderCallback(void * inRefCon,
							  AudioUnitRenderActionFlags * ioActionFlags,
							  const AudioTimeStamp * inTimeStamp,
							  UInt32 inBusNumber,
							  UInt32 inNumberFrames,
							  AudioBufferList * ioData)
{
	for(int i = 0; i < ioData->mNumberBuffers; i++) {
		memset(ioData->mBuffers[i].mData, 0, ioData->mBuffers[0].mDataByteSize);
	}
	
	*ioActionFlags |= kAudioUnitRenderAction_OutputIsSilence;
	
	return noErr;
}
