// AudioDeviceImpl.cpp

#include "AudioDeviceImpl.h"
#include "audiostream.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define DEBUG 0

#if DEBUG > 1
#define PRINT0 if (1) printf
#define PRINT1 if (1) printf
#elif DEBUG > 0
#define PRINT0 if (1) printf
#define PRINT1 if (0) printf
#else
#define PRINT0 if (0) printf
#define PRINT1 if (0) printf
#endif

inline int max(int x, int y) { return (x > y) ? x : y; }

// AudioDeviceImpl is the "workhorse" intermediate base class for most
// AudioDevice leaf classes.  It holds the common state, handles all logic
// that can be considered common to all AudioDevices (including state checking
// and format conversion), and handles the callbacks associated with playback
// and stop.  All primary AudioDevice methods are implemented in this class,
// where they in turn call the protected virtual "hook" functions, which
// perform the HW-specific operations.

AudioDeviceImpl::AudioDeviceImpl() 
	: _mode(Unset), _state(Closed),
	  _frameFormat(MUS_UNSUPPORTED), _deviceFormat(MUS_UNSUPPORTED),
	  _frameChannels(0), _deviceChannels(0), _samplingRate(0.0), _maxFrames(0),
	  _runCallback(NULL), _stopCallback(NULL),
	  _convertBuffer(NULL), 
	  _recConvertFunction(NULL), _playConvertFunction(NULL)
{
	for (int n = 0; n < 32; ++n) {
		_peaks[n] = 0.0;
		_peakLocs[n] = 0;
	}
}

AudioDeviceImpl::~AudioDeviceImpl()
{
	PRINT0("AudioDeviceImpl::~AudioDeviceImpl()\n");
	/* if this asserts, close() is not being called before destructor */
	assert(_convertBuffer == NULL);
	delete _runCallback;
	delete _stopCallback;
}

int	AudioDeviceImpl::setFrameFormat(int frameSampFmt, int frameChans)
{
	_frameFormat = frameSampFmt;
	_frameChannels = frameChans;
	return 0;
}

int AudioDeviceImpl::open(int mode, int sampfmt, int chans, double srate)
{
	PRINT0("AudioDeviceImpl::open: opening device %p for %s in %s mode\n",
		   this,
		   (mode & DirectionMask) == RecordPlayback ? "Record/Playback"
		   : (mode & DirectionMask) == Playback ? "Playback" : "Record",
		   (mode & Passive) ? "Passive" : "Active");
	_lastErr[0] = 0;
	setMode(mode);
	int status = 0;
	close();
	if ((status = doOpen(mode)) == 0) {
		setState(Open);
		if ((status = setFormat(sampfmt, chans, srate)) != 0)
			close();
	}
	return status;
}

int AudioDeviceImpl::close()
{
	PRINT0("AudioDeviceImpl::close -- begin\n");
	int status = 0;
	if (isOpen()) {
		PRINT0("AudioDeviceImpl::close: was open, calling stop()\n");
		stop();
		PRINT0("AudioDeviceImpl::close: now calling doClose()\n");
		if ((status = doClose()) == 0) {
			PRINT0("AudioDeviceImpl::close: now calling destroyConvertBuffer()\n");
			destroyConvertBuffer();
			setState(Closed);
			_frameFormat = MUS_UNSUPPORTED;
			_frameChannels = 0;
			_samplingRate = 0;
			PRINT0("AudioDeviceImpl::close: state now set to Closed\n");
		}
	}
	PRINT0("AudioDeviceImpl::close -- finish\n");
	return status;
}

int AudioDeviceImpl::start(AudioDevice::Callback *callback)
{
	PRINT1("AudioDeviceImpl::start -- begin\n");
	int status = 0;
	if (!isRunning()) {
		PRINT1("AudioDeviceImpl::start: not running, calling doStart()\n");
		_runCallback = callback;
		if ((status = doStart()) == 0) {
			setState(Running);
		}
	}
	PRINT1("AudioDeviceImpl::start -- finish\n");
	return status;
}

int AudioDeviceImpl::pause(bool willPause)
{
	int status = 0;
	if (isRunning() && isPaused() != willPause) {
		if ((status = doPause(willPause)) == 0) {
			setState(willPause ? Paused : Running);
		}
	}
	return status;
}

int AudioDeviceImpl::setStopCallback(Callback *stopCallback)
{
	if (stopCallback != _stopCallback) {
		delete _stopCallback;
		_stopCallback = stopCallback;
	}
	return 0;
}

bool AudioDeviceImpl::runCallback()
{ 
	assert(_runCallback);
	return _runCallback->call();
}

// Call the callback if present; delete and null to avoid multiple calls

bool AudioDeviceImpl::stopCallback() {
	Callback *stop = _stopCallback;
	_stopCallback = NULL;
	bool ret = (stop) ? stop->call() : false;
	delete stop;
	return ret;
}

int AudioDeviceImpl::stop()
{
	PRINT0("AudioDeviceImpl::stop -- begin\n");
	int status = 0;
	if (isRunning()) {
		PRINT0("AudioDeviceImpl::stop: was running, calling doStop()\n");
		if ((status = doStop()) == 0) {
			setState(Configured);
		}
	}
	Callback *tempCallback = _runCallback;
	_runCallback = NULL;
	delete tempCallback;
	PRINT0("AudioDeviceImpl::stop -- finish\n");
	return status;
}

int AudioDeviceImpl::getFrames(void *frameBuffer, int frameCount)
{
	int status = 0;
	if (isRecording()) {
		// XXX this is a hack to avoid an extra buffer copy.
		if (_frameFormat == _deviceFormat) {
			PRINT1("getFrames: skipping conversion after doGetFrames (formats identical)\n");
			doGetFrames(frameBuffer, frameCount);
		}
		else {
			PRINT1("getFrames: running conversion after doGetFrames\n");
			doGetFrames(_convertBuffer, frameCount);
			convertFrame(_convertBuffer, frameBuffer, frameCount, true);
		}
	}
	else
		status = error("Not in record mode");
	return status;
}

int AudioDeviceImpl::sendFrames(void *frameBuffer, int frameCount)
{
	int status = 0;
	if (isPlaying()) {
		// Clip if converting from non-clipped to clipped.
		bool doClipping = !isFrameFmtClipped() && isPlaybackDeviceFmtClipped();
		limitFrame(frameBuffer, frameCount,
				   doClipping, checkPeaks(), reportClipping());
		void *sendBuffer = convertFrame(frameBuffer,
										_convertBuffer, 
										frameCount, 
										false);
		status = doSendFrames(sendBuffer, frameCount);
	}
	else
		status = error("Not in playback mode");
	return status;
}

const char *AudioDeviceImpl::getLastError() const {
	return _lastErr;
}

double AudioDeviceImpl::getPeak(int chan, long *pLocation) const
{
	*pLocation = _peakLocs[chan];
	return _peaks[chan];
}

int AudioDeviceImpl::error(const char *msg, const char *msg2)
{
	sprintf(_lastErr, "AudioDevice: %s%s", msg, msg2 ? msg2 : "");
	return -1;
}

int AudioDeviceImpl::setFormat(int sampfmt, int chans, double srate)
{
	if (isOpen()) {
		if (doSetFormat(sampfmt, chans, srate) == 0) {
			assert(_deviceFormat != 0);
			assert(_deviceChannels != 0);
			assert(_samplingRate != 0.0);
			setState(Configured);
			return 0;
		}
		else return -1;
	}
	return error("Audio device not open");
}

// Code for creating and destroying interleaved conversion buffer

void *
AudioDeviceImpl::createInterleavedBuffer(int fmt, int chans, int len)
{
	void *buffer = NULL;
	if (IS_FLOAT_FORMAT(fmt)) {
		buffer = (void *) new float[chans * len];
	}
	else if (IS_32BIT_FORMAT(fmt)) {
		buffer = (void *) new int32_t[chans * len];
	}
	else if (IS_SHORT_FORMAT(fmt)) {
		buffer = (void *) new short[chans * len];
	}
	else if (IS_24BIT_FORMAT(fmt)) {
		buffer = (void *) new char[chans * len * 3];
	}
	else {
		error("createInterleavedBuffer: unknown sample format!");
		return NULL;
	}
	if (!buffer)
		error("createInterleavedBuffer: memory allocation failure");
	return buffer;
}

void 
AudioDeviceImpl::destroyInterleavedBuffer(int fmt)
{
	if (IS_FLOAT_FORMAT(fmt)) {
		delete [] (float *) _convertBuffer;
	}
	else if (IS_32BIT_FORMAT(fmt)) {
		delete [] (int32_t *) _convertBuffer;
	}
	else if (IS_SHORT_FORMAT(fmt)) {
		delete [] (short *) _convertBuffer;
	}
	else if (IS_24BIT_FORMAT(fmt)) {
		delete [] (char *) _convertBuffer;
	}
}

// Code for creating and destroying non-interleaved conversion buffer

template <class Type> 
static Type **newNoninterleavedBuffer(int chans, int len) {
	Type **tbuf = new Type *[chans];
	if (tbuf) {
		for (int c = 0; c < chans; ++c) {
			tbuf[c] = new Type[len];
			if (tbuf[c] == NULL) {		// Unwind in event of memory failure
				for (int n = c - 1; n > 0; --n)
					delete [] tbuf[n];
				delete [] tbuf;
				return NULL;
			}
		}
	}
	return tbuf;
}

template <class Type> 
static void deleteNoninterleavedBuffer(void *buf, int chans) {
	Type **tbuf = (Type **) buf;
	if (tbuf) {
		for (int c = 0; c < chans; ++c)
			delete [] tbuf[c];
		delete [] tbuf;
	}
}

void *
AudioDeviceImpl::createNoninterleavedBuffer(int fmt, int chans, int len)
{
	void *buffer = NULL;
	if (IS_FLOAT_FORMAT(fmt)) {
		buffer = (void *) newNoninterleavedBuffer<float>(chans, len);
	}
	else if (IS_32BIT_FORMAT(fmt)) {
		buffer = (void *) newNoninterleavedBuffer<int32_t>(chans, len);
	}
	else if (IS_24BIT_FORMAT(fmt)) {
		buffer = (void *) newNoninterleavedBuffer<char>(chans, len * 3);
	}
	else if (IS_SHORT_FORMAT(fmt)) {
		buffer = (void *) newNoninterleavedBuffer<short>(chans, len);
	}
	else {
		error("createNoninterleavedBuffer: unsupported sample format!");
		return NULL;
	}
	if (!buffer)
		error("createNoninterleavedBuffer: memory allocation failure");
	
	return buffer;
}

void
AudioDeviceImpl::destroyNoninterleavedBuffer(int fmt, int chans)
{
	if (IS_FLOAT_FORMAT(fmt)) {
		deleteNoninterleavedBuffer<float>(_convertBuffer, chans);
	}
	else if (IS_32BIT_FORMAT(fmt)) {
		deleteNoninterleavedBuffer<int32_t>(_convertBuffer, chans);
	}
	else if (IS_24BIT_FORMAT(fmt)) {
		deleteNoninterleavedBuffer<char>(_convertBuffer, chans);
	}
	else if (IS_SHORT_FORMAT(fmt)) {
		deleteNoninterleavedBuffer<short>(_convertBuffer, chans);
	}
}

// createConvertBuffer(): Creates an interleaved or non-interleaved intermediate
// buffer if the input and device formats/interleaves do not match.
// Note that for both record and playback, the conversion buffer is the same
// format -- that of the HW.

int AudioDeviceImpl::createConvertBuffer(int frames, int chans)
{
	if (isDeviceInterleaved())
		_convertBuffer = createInterleavedBuffer(getDeviceFormat(),
												 chans,
												 frames);
	else
		_convertBuffer = createNoninterleavedBuffer(getDeviceFormat(),
													chans,
													frames);
	if (!_convertBuffer)
		return -1;
	return 0;
}

void AudioDeviceImpl::destroyConvertBuffer()
{
	if (isDeviceInterleaved())
		destroyInterleavedBuffer(getDeviceFormat());
	else
		destroyNoninterleavedBuffer(getDeviceFormat(), max(getRecordDeviceChannels(), getPlaybackDeviceChannels()));
	_convertBuffer = NULL;
}

int AudioDeviceImpl::setQueueSize(int *pWriteSize, int *pCount)
{
	int reqWriteSize = *pWriteSize;
	int reqCount = *pCount;
	switch (getState()) {
	case Configured:
		break;
	case Running:
	case Paused:
		if (isPassive())
			break;	// In passive mode we can do this anytime.
		// else
		*pWriteSize = -1;
		return error("Cannot set queue size while running");
	case Open:
		*pWriteSize = -1;
		return error("Cannot set queue size before setting format");
	case Closed:
		*pWriteSize = -1;
		return error("Audio device not open");
	default:
		*pWriteSize = -1;
		return error("Unknown state!");
	}
	
	if (doSetQueueSize(&reqWriteSize, &reqCount) == 0) {
		*pWriteSize = reqWriteSize;
		*pCount = reqCount;
		_maxFrames = reqWriteSize * reqCount;
		return setupConversion(isRecording(), isPlaying());
	}
	*pWriteSize = -1;	// error condition
	return -1;
}

int AudioDeviceImpl::setupConversion(bool recording, bool playing)
{
	int status = 0;
	// Use the raw format because the accessor functions
	// filter out the interleave and normalize bits.
	PRINT0("AudioDeviceImpl::setupConversion(): user fmt: 0x%x, " "device fmt: 0x%x, user chans: %d\n",
			_frameFormat, _deviceFormat, getFrameChannels());
	int deviceChannels, maxChannels;
	if (recording && playing) {
		if (getRecordDeviceFormat() != getPlaybackDeviceFormat()) {
			return error("Full duplex only supported when rec and playback devices have same sample format");
		}
		PRINT0("\trecord device chans: %d, playback device chans: %d\n", getRecordDeviceChannels(), getPlaybackDeviceChannels());
		if (getRecordDeviceChannels() != getPlaybackDeviceChannels())
			deviceChannels = 0;		// This will force conversion to be set up, below
		maxChannels = max(getRecordDeviceChannels(), getPlaybackDeviceChannels());
	}
	else if (recording) {
		maxChannels = deviceChannels = getRecordDeviceChannels();
		PRINT0("\trecord device chans: %d\n", deviceChannels);
	}
	else /* playing */ {
		maxChannels = deviceChannels = getPlaybackDeviceChannels();
		PRINT0("\tplayback device chans: %d\n", deviceChannels);
	}
			
	// Create conversion routines if 1) frame format differs from device or 2) channel counts differ
	if (_frameFormat != _deviceFormat || getFrameChannels() != deviceChannels)
	{
		status = createConvertBuffer(_maxFrames, maxChannels);
		if (status == 0) {
			status = setConvertFunctions(_frameFormat, _deviceFormat);
		}
	}
	else {
		PRINT0("\tdevice and frame formats match exactly -- no conversion necessary\n");
	}
	return status;
}

// This is used by specialized derived classes that need to change formats
// in mid-stream.

int AudioDeviceImpl::resetFormatConversion()
{
	destroyConvertBuffer();
	return setupConversion(isRecording(), isPlaying());
}

int AudioDeviceImpl::getDeviceBytesPerFrame() const
{
	return mus_data_format_to_bytes_per_sample(getDeviceFormat()) 
				* getDeviceChannels();
}


void *
AudioDeviceImpl::convertFrame(void *inbuffer, void *outbuffer,
							  int frames, bool recording)
{
	const int frmChans = getFrameChannels();
	assert(inbuffer != NULL);
	if (recording) {
		if(_recConvertFunction == NULL) {
			PRINT1("convertFrame: skipping conversion during record\n");
			return inbuffer;
		}
		assert(outbuffer != NULL);
		const int devChans = getRecordDeviceChannels();
		(*_recConvertFunction)(inbuffer, outbuffer,
							   devChans, frmChans, frames);
	}
	else {
		if(_playConvertFunction == NULL) {
			PRINT1("convertFrame: skipping conversion during playback\n");
			return inbuffer;
		}
		assert(outbuffer != NULL);
		const int devChans = getPlaybackDeviceChannels();
		(*_playConvertFunction)(inbuffer, outbuffer, 
								frmChans, devChans, frames);
	}
	return outbuffer;
}

void
AudioDeviceImpl::limitFrame(void *frameBuffer, int frames, bool doClip, bool checkPeaks, bool reportClipping)
{
	const int chans = getFrameChannels();	// since frameBuffer is from user
	const long bufStartSamp = getFrameCount();
	int numclipped = 0;
	float clipmax = 0.0f;
	PRINT1("AudioDeviceImpl::limitFrame: clip = %d check = %d\n", doClip, checkPeaks);
	for (int c = 0; c < chans; ++c) {
		float *fp;
		int incr;
		if (isFrameInterleaved()) {
			fp = &((float *) frameBuffer)[c];
			incr = chans;
		}
		else {
			fp = ((float **) frameBuffer)[c];
			incr = 1;
		}
		for (int n = 0; n < frames; ++n, fp += incr) {
			if (doClip) {
				const float samp = *fp;
				if (samp < -32768.0f) {
					if (samp < -clipmax)
						clipmax = -samp;
					*fp = -32768.0f;
					++numclipped;
				}
				else if (samp > 32767.0f) {
					if (samp > clipmax)
						clipmax = samp;
					*fp = 32767.0f;
					++numclipped;
				}
			}
			if (checkPeaks) {
				const double fabsamp = fabs((double) *fp);
				if (fabsamp > (double) _peaks[c]) {
					_peaks[c] = (float) fabsamp;
					_peakLocs[c] = bufStartSamp + n;	// frame count
				}
			}
		}
	}
	if (numclipped && reportClipping) {
		float loc1 = bufStartSamp / getSamplingRate();
		float loc2 = loc1 + (frames / getSamplingRate());
		bool printing_dots = true;	// HACK FOR NOW
		/* We start with a newline if we're also printing the buffer dots. */
		fprintf(stderr,
    		  "%s  CLIPPING: %4d samps, max: %g, time range: %f - %f\n",
    		  (printing_dots? "\n" : ""), numclipped, clipmax, loc1, loc2);
	}
}

static const float kFloatNormalizer = (1.0 / 32768.0);
typedef unsigned char *UCharP;


// Note:  The 24bit converters are only used for writing soundfiles to disk.

template <class Type>
static void _convertIToIB24Bit(void *in, void *out,
							   int inchans, int outchans, int frames)
{
	Type *tin = (Type *) in;
	unsigned char *cout = (unsigned char *) out;
	assert(inchans == outchans);
	int samps = inchans * frames;
	for (int s = 0; s < samps; ++s, cout += 3) {
		const int samp = (int) (tin[s] * (1 << 8));
		cout[0] = (samp >> 16);
		cout[1] = (samp >> 8);
		cout[2] = (samp & 0xFF);
	}
}

template <class Type>
static void _convertIToIL24Bit(void *in, void *out,
							   int inchans, int outchans, int frames)
{
	Type *tin = (Type *) in;
	unsigned char *cout = (unsigned char *) out;
	assert(inchans == outchans);
	int samps = inchans * frames;
	for (int s = 0; s < samps; ++s, cout += 3) {
		const int samp = (int) (tin[s] * (1 << 8));
		cout[2] = (samp >> 16);
		cout[1] = (samp >> 8);
		cout[0] = (samp & 0xFF);
	}
}

static void _convertNFloatToIB24Bit(void *in, void *out,
									int inchans, int outchans, int frames)
{
	float **fin = (float **) in;
	assert(inchans == outchans);
	for (int ch = 0; ch < inchans; ++ch) {
		float *fbuffer = fin[ch];
		unsigned char *cout = ((unsigned char *) out) + (ch * 3);
		int incr = inchans * 3;
		for (int fr = 0; fr < frames; ++fr, cout += incr) {
			const int samp = (int) (fbuffer[fr] * (1 << 8));
			cout[0] = (samp >> 16);
			cout[1] = (samp >> 8);
			cout[2] = (samp & 0xFF);
		}
	}
}

static void _convertNFloatToIL24Bit(void *in, void *out,
									int inchans, int outchans, int frames)
{
	float **fin = (float **) in;
	assert(inchans == outchans);
	for (int ch = 0; ch < inchans; ++ch) {
		float *fbuffer = fin[ch];
		unsigned char *cout = ((unsigned char *) out) + (ch * 3);
		int incr = outchans * 3;
		for (int fr = 0; fr < frames; ++fr, cout += incr) {
			int samp = (int) (fbuffer[fr] * (1 << 8));
			cout[2] = (samp >> 16);
			cout[1] = (samp >> 8);
			cout[0] = (samp & 0xFF);
		}
	}
}

// This is the master template function which can handle conversion from one
// sample format, channel count, interleave, normalization, and endian-ness 
// to another.
// The definitions of the Instream and Outstream classes are in audiostream.h.
// ABANDON HOPE, ALL YE WHO ENTER HERE...

template< class InStream, class OutStream >
static void convert(void *_in, void *_out, int inchans, int outchans, int frames)
{
	typedef typename InStream::StreamType InType;
    typedef typename OutStream::StreamType OutType;    
	InType *in = (InType *)_in;
    OutType *out = (OutType *)_out;
	typedef typename InStream::ChannelType InChanType;
	typedef typename OutStream::ChannelType OutChanType;    
	const int chans = (outchans < inchans) ? outchans : inchans;
	const int inIncr = InStream::channelIncrement(inchans);
	const int outIncr = OutStream::channelIncrement(outchans);
	PRINT1("%s: inchans %d outchans %d - running conversion for %d channels\n",
		   __FUNCTION__, inchans, outchans, chans);
	PRINT1("%sinterleaved %s in ==> %sinterleaved %s out\n",
			InStream::channelIncrement(4) != 1 ? "" : "non-", InStream::normalized ? "normalized" : "",
			OutStream::channelIncrement(4) != 1 ? "" : "non-", OutStream::normalized ? "normalized" : "");
	int ch;
    for (ch = 0; ch < chans; ++ch) {
        InChanType *inbuffer = InStream::innerFromOuter(in, ch);
        OutChanType *outbuffer = OutStream::innerFromOuter(out, ch);
		PRINT1("\tchannel %d -- converting %d frames.  Outbuffer starts at %p\n", ch, frames, outbuffer);
        for (int fr = 0; fr < frames;
			 ++fr, inbuffer += inIncr, outbuffer += outIncr)
		{
			const OutChanType intermediate = 
				::deNormalize<InChanType, OutChanType>(
						InStream::normalized,
						::swap(InStream::endian != kMachineEndian,
							   *inbuffer));
            *outbuffer = ::swap(OutStream::endian != kMachineEndian,
								::normalize(OutStream::normalized,
											intermediate));
        }
    }
	// Zero out any output channels beyond input channel count
	for (; ch < outchans; ++ch) {
        OutChanType *outbuffer = OutStream::innerFromOuter(out, ch);
		const int outIncr = OutStream::channelIncrement(outchans);
		PRINT1("\tchannel %d -- zeroing %d frames\n", ch, frames);
        for (int fr = 0; fr < frames; ++fr, outbuffer += outIncr)
		{
            *outbuffer = (OutChanType) 0;
		}
	}
}

// Conversion functions are assigned in pairs, regardless of rec/pb state.

int AudioDeviceImpl::setConvertFunctions(int rawFrameFormat,
										 int rawDeviceFormat)
{
	const bool frameNormalized = IS_NORMALIZED_FORMAT(rawFrameFormat);
	const bool frameInterleaved = IS_INTERLEAVED_FORMAT(rawFrameFormat);
	const bool deviceNormalized = IS_NORMALIZED_FORMAT(rawDeviceFormat);
	const bool deviceInterleaved = IS_INTERLEAVED_FORMAT(rawDeviceFormat);
	
	_recConvertFunction = NULL;
	_playConvertFunction = NULL;
	
	if (!IS_FLOAT_FORMAT(rawFrameFormat)) {
		return error("Only floating point frame data supported");
	}

	// The device may be a file or HW, so take endian-ness into account for
	// playback and some record options.
	if (frameInterleaved) {
		if (deviceInterleaved) {
			switch (MUS_GET_FORMAT(rawDeviceFormat)) {
			case MUS_LFLOAT:	// float frame, LE float device
				if (deviceNormalized == frameNormalized) {
					_recConvertFunction = 
						::convert< InterleavedStream<float, Little>, InterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< InterleavedStream<float, kMachineEndian>, InterleavedStream<float, Little> >;
				}
				else if (deviceNormalized && !frameNormalized) {
					_recConvertFunction = 
						::convert< InterleavedStream<float, Little, true>, InterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< InterleavedStream<float, kMachineEndian>, InterleavedStream<float, Little, true> >;
				}
				break;

			case MUS_BFLOAT:	// float frame, BE float device
				if (deviceNormalized == frameNormalized) {
					_recConvertFunction = 
						::convert< InterleavedStream<float, Big>, InterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< InterleavedStream<float, kMachineEndian>, InterleavedStream<float, Big> >;
				}
				else if (deviceNormalized && !frameNormalized) {
					_recConvertFunction = 
						::convert< InterleavedStream<float, Big, true>, InterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< InterleavedStream<float, kMachineEndian>, InterleavedStream<float, Big, true> >;
				}
				break;

			case MUS_B24INT:	// float frame, BE 24bit file.  HW record not supported
				_playConvertFunction = _convertIToIL24Bit<float>;
				break;

			case MUS_L24INT:	// float frame, BE 24bit file.  HW record not supported
				_playConvertFunction = _convertIToIB24Bit<float>;
				break;

			case MUS_LSHORT:	// float frame, LE short device
				_recConvertFunction = 
					::convert< InterleavedStream<short, Little>, InterleavedStream<float, kMachineEndian> >;
				_playConvertFunction =
					::convert< InterleavedStream<float, kMachineEndian>, InterleavedStream<short, Little> >;
				break;

			case MUS_BSHORT:	// float frame, BE short device
				_recConvertFunction = 
					::convert< InterleavedStream<short, Big>, InterleavedStream<float, kMachineEndian> >;
				_playConvertFunction =
					::convert< InterleavedStream<float, kMachineEndian>, InterleavedStream<short, Big> >;
				break;

			default:
				break;
			}
		}
		else /* !deviceInterleaved */ {
			switch (MUS_GET_FORMAT(rawDeviceFormat)) {
			case MUS_LFLOAT:	// float frame, LE float device
				if (deviceNormalized == frameNormalized) {
					_recConvertFunction = 
						::convert< NonInterleavedStream<float, Little>, InterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< InterleavedStream<float, kMachineEndian>, NonInterleavedStream<float, Little> >;
				}
				else if (deviceNormalized && !frameNormalized) {
					_recConvertFunction = 
						::convert< NonInterleavedStream<float, Little, true>, InterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< InterleavedStream<float, kMachineEndian>, NonInterleavedStream<float, Little, true> >;
				}
				break;
			case MUS_BFLOAT:	// float frame, BE float device
				if (deviceNormalized == frameNormalized) {
					_recConvertFunction = 
						::convert< NonInterleavedStream<float, Big>, InterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< InterleavedStream<float, kMachineEndian>, NonInterleavedStream<float, Big> >;
				}
				else if (deviceNormalized && !frameNormalized) {
					_recConvertFunction = 
						::convert< NonInterleavedStream<float, Big, true>, InterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< InterleavedStream<float, kMachineEndian>, NonInterleavedStream<float, Big, true> >;
				}
				break;

			case MUS_LSHORT:
				_recConvertFunction = 
					::convert< NonInterleavedStream<short, Little>, InterleavedStream<float, kMachineEndian> >;
				_playConvertFunction =
					::convert< InterleavedStream<float, kMachineEndian>, NonInterleavedStream<short, Little> >;
				break;
			case MUS_BSHORT:
				_recConvertFunction = 
					::convert< NonInterleavedStream<short, Big>, InterleavedStream<float, kMachineEndian> >;
				_playConvertFunction =
					::convert< InterleavedStream<float, kMachineEndian>, NonInterleavedStream<short, Big> >;
				break;

			default:
				break;
			}
		}	// !deviceInterleaved
	}
	else /* !frameInterleaved */ {
		if (deviceInterleaved) {
			switch (MUS_GET_FORMAT(rawDeviceFormat)) {
			case MUS_LFLOAT:	// float frame, LE float device
				if (deviceNormalized == frameNormalized) {
					_recConvertFunction = 
						::convert< InterleavedStream<float, Little>, NonInterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< NonInterleavedStream<float, kMachineEndian>, InterleavedStream<float, Little> >;
				}
				else if (deviceNormalized && !frameNormalized) {
					_recConvertFunction = 
						::convert< InterleavedStream<float, Little, true>, NonInterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< NonInterleavedStream<float, kMachineEndian>, InterleavedStream<float, Little, true> >;
				}
				break;

			case MUS_BFLOAT:	// float frame, BE float device
				if (deviceNormalized == frameNormalized) {
					_recConvertFunction = 
						::convert< InterleavedStream<float, Big>, NonInterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< NonInterleavedStream<float, kMachineEndian>, InterleavedStream<float, Big> >;
				}
				else if (deviceNormalized && !frameNormalized) {
					_recConvertFunction = 
						::convert< InterleavedStream<float, Big, true>, NonInterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< NonInterleavedStream<float, kMachineEndian>, InterleavedStream<float, Big, true> >;
				}
				break;

			case MUS_B24INT:	// float frame, BE 24bit file.  HW record not supported
				_playConvertFunction = _convertNFloatToIB24Bit;
				break;

			case MUS_L24INT:	// float frame, BE 24bit file.  HW record not supported
				_playConvertFunction = _convertNFloatToIL24Bit;
				break;

			case MUS_LSHORT:	// float frame, LE short device
				_recConvertFunction = 
					::convert< InterleavedStream<short, Little>, NonInterleavedStream<float, kMachineEndian> >;
				_playConvertFunction =
					::convert< NonInterleavedStream<float, kMachineEndian>, InterleavedStream<short, Little> >;
				break;

			case MUS_BSHORT:	// float frame, BE short device
				_recConvertFunction = 
					::convert< InterleavedStream<short, Big>, NonInterleavedStream<float, kMachineEndian> >;
				_playConvertFunction =
					::convert< NonInterleavedStream<float, kMachineEndian>, InterleavedStream<short, Big> >;
				break;

			default:
				break;
			}
		}
		else /* !deviceInterleaved */ {
			switch (MUS_GET_FORMAT(rawDeviceFormat)) {
			case MUS_LFLOAT:	// float frame, LE float device
				if (deviceNormalized == frameNormalized) {
					_recConvertFunction = 
						::convert< NonInterleavedStream<float, Little>, NonInterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< NonInterleavedStream<float, kMachineEndian>, NonInterleavedStream<float, Little> >;
				}
				else if (deviceNormalized && !frameNormalized) {
					_recConvertFunction = 
						::convert< NonInterleavedStream<float, Little, true>, NonInterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< NonInterleavedStream<float, kMachineEndian>, NonInterleavedStream<float, Little, true> >;
				}
				break;
			case MUS_BFLOAT:	// float frame, BE float device
				if (deviceNormalized == frameNormalized) {
					_recConvertFunction = 
						::convert< NonInterleavedStream<float, Big>, NonInterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< NonInterleavedStream<float, kMachineEndian>, NonInterleavedStream<float, Big> >;
				}
				else if (deviceNormalized && !frameNormalized) {
					_recConvertFunction = 
						::convert< NonInterleavedStream<float, Big, true>, NonInterleavedStream<float, kMachineEndian> >;
					_playConvertFunction =
						::convert< NonInterleavedStream<float, kMachineEndian>, NonInterleavedStream<float, Big, true> >;
				}
				break;

			case MUS_LINT:
				_recConvertFunction = 
					::convert< NonInterleavedStream<int32_t, Little, true>, NonInterleavedStream<float, kMachineEndian> >;
				_playConvertFunction =
					::convert< NonInterleavedStream<float, kMachineEndian>, NonInterleavedStream<int32_t, Little, true> >;
				break;
			case MUS_BINT:
				_recConvertFunction = 
					::convert< NonInterleavedStream<int32_t, Big, true>, NonInterleavedStream<float, kMachineEndian> >;
				_playConvertFunction =
					::convert< NonInterleavedStream<float, kMachineEndian>, NonInterleavedStream<int32_t, Big, true> >;
				break;

			case MUS_LSHORT:
				_recConvertFunction = 
					::convert< NonInterleavedStream<short, Little>, NonInterleavedStream<float, kMachineEndian> >;
				_playConvertFunction =
					::convert< NonInterleavedStream<float, kMachineEndian>, NonInterleavedStream<short, Little> >;
				break;
			case MUS_BSHORT:
				_recConvertFunction = 
					::convert< NonInterleavedStream<short, Big>, NonInterleavedStream<float, kMachineEndian> >;
				_playConvertFunction =
					::convert< NonInterleavedStream<float, kMachineEndian>, NonInterleavedStream<short, Big> >;
				break;

			default:
				break;
			}
		}	// !deviceInterleaved
	}	// !frameInterleaved
	
	if (isPlaying() && _playConvertFunction == NULL)
		return error("This format conversion is currently not supported!");
	if (isRecording() && _recConvertFunction == NULL)
		return error("This format conversion is currently not supported!");
	return 0;
}

