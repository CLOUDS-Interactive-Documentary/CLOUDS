#include "ofxAudioUnitRecorder.h"
#include "ofxAudioUnitUtils.h"

// a render callback records audio passing through it
static OSStatus Record(void * inRefCon,
					   AudioUnitRenderActionFlags *	ioActionFlags,
					   const AudioTimeStamp *	inTimeStamp,
					   UInt32 inBusNumber,
					   UInt32	inNumberFrames,
					   AudioBufferList * ioData);


ofxAudioUnitRecorder::ofxAudioUnitRecorder()
: _recordFile(NULL) {
	
}

ofxAudioUnitRecorder::~ofxAudioUnitRecorder() {
	if(_recordFile) {
		ExtAudioFileDispose(_recordFile);
		_recordFile = NULL;
	}
}

bool ofxAudioUnitRecorder::startRecording(const std::string &filePath) {
	
	if(_recordFile) {
		stopRecording();
	}
	
	AudioStreamBasicDescription inASBD = getSourceASBD();
	
	if(inASBD.mFormatID == 0) {
		std::cout << "Recorder couldn't determine proper stream format. ";
		std::cout << "Is the recorder directly after an Audio Unit?" << std::endl;
		return false;
	}
	
	CFURLRef fileURL = CFURLCreateFromFileSystemRepresentation(NULL,
															   (const UInt8*)filePath.c_str(),
															   filePath.length(),
															   false);
	
	AudioStreamBasicDescription outASBD = {
		.mChannelsPerFrame = inASBD.mChannelsPerFrame,
		.mSampleRate = inASBD.mSampleRate,
		.mFormatID = kAudioFormatMPEG4AAC
	};
	
	OSStatus s = ExtAudioFileCreateWithURL(fileURL,
										   kAudioFileM4AType,
										   &outASBD,
										   NULL,
										   kAudioFileFlags_EraseFile,
										   &_recordFile);
	CFRelease(fileURL);
	
	if(s != noErr) {
		cout << "Couldn't create audio file: " << (OSStatus)s << endl;
		return false;
	}
	
	s = ExtAudioFileSetProperty(_recordFile,
								kExtAudioFileProperty_ClientDataFormat,
								sizeof(inASBD),
								&inASBD);
	
	
	if(s != noErr) {
		cout << "Couldn't set client format: " << (OSStatus)s << endl;
		return false;
	}
	
	setProcessCallback((AURenderCallbackStruct){Record, _recordFile});
	
	return true;
}

void ofxAudioUnitRecorder::stopRecording() {
	if(_recordFile) {
		setProcessCallback((AURenderCallbackStruct){0}); // stop calling Record callback
		ExtAudioFileDispose(_recordFile);
		_recordFile = NULL;
	}
}

OSStatus Record(void * inRefCon,
				AudioUnitRenderActionFlags * ioActionFlags,
				const AudioTimeStamp *	inTimeStamp,
				UInt32 inBusNumber,
				UInt32	inNumberFrames,
				AudioBufferList * ioData)
{
	ExtAudioFileRef file = (ExtAudioFileRef)inRefCon;
	
	OSStatus s = ExtAudioFileWriteAsync(file, inNumberFrames, ioData);
	
	if(s != noErr) {
		cout << "error while recording audio: " << (OSStatus)s << endl;
	}
	
	return noErr;
}
