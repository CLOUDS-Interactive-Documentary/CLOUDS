#include "ofxAudioUnit.h"
#include "ofxAudioUnitUtils.h"
#import <mach/mach_time.h>

static ScheduledAudioFileRegion RegionForEntireFile(AudioFileID fileID);

static AudioComponentDescription filePlayerDesc = {
	kAudioUnitType_Generator,
	kAudioUnitSubType_AudioFilePlayer,
	kAudioUnitManufacturer_Apple
};

ofxAudioUnitFilePlayer::ofxAudioUnitFilePlayer()
: _region((ScheduledAudioFileRegion){0})
, _pauseTimeStamp((AudioTimeStamp){0})
, _loopCount(0)
, _pauseTimeAccumulator(0)
, _primed(false) {
	_fileID[0] = NULL;
	_desc = filePlayerDesc;
	initUnit();
}

ofxAudioUnitFilePlayer::~ofxAudioUnitFilePlayer() {
	stop();
	AudioFileClose(_fileID[0]);
}

#pragma mark - Properties

bool ofxAudioUnitFilePlayer::setFile(const std::string &filePath) {
	CFURLRef fileURL;
	fileURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
	                                                  (const UInt8 *)filePath.c_str(),
	                                                  filePath.length(),
	                                                  NULL);
	
	if(_fileID[0]) {
		AudioFileClose(_fileID[0]);
		_fileID[0] = NULL;
	}
	
	OSStatus s = AudioFileOpenURL(fileURL, kAudioFileReadPermission, 0, _fileID);
	CFRelease(fileURL);
	
	_primed = false;
	
	if(s != noErr) {
		cout << "Error " << s << " while opening file at " << filePath << endl;
		return false;
	} else {
		// setting the file ID now since it seems to have some overhead.
		// Doing it now ensures you'll get sound pretty much instantly after
		// calling play() (subsequent calls don't have the overhead)
		OFXAU_RET_BOOL(AudioUnitSetProperty(*_unit,
											kAudioUnitProperty_ScheduledFileIDs,
											kAudioUnitScope_Global,
											0,
											_fileID,
											sizeof(_fileID)),
					   "setting file player's file ID");
	}
}

void ofxAudioUnitFilePlayer::setLength(UInt32 length) {
	_region.mFramesToPlay = length;
}

UInt32 ofxAudioUnitFilePlayer::getLength() const {
	return RegionForEntireFile(_fileID[0]).mFramesToPlay;
}

AudioTimeStamp ofxAudioUnitFilePlayer::getCurrentTimestamp() const {
	AudioTimeStamp timeStamp = {0};
	UInt32 timeStampSize = sizeof(AudioTimeStamp);
	
	OFXAU_PRINT(AudioUnitGetProperty(*_unit,
									 kAudioUnitProperty_CurrentPlayTime,
									 kAudioUnitScope_Global,
									 0,
									 &timeStamp,
									 &timeStampSize),
				"getting time stamp from file player");
	
	return timeStamp;
}

#pragma mark - Playback

void ofxAudioUnitFilePlayer::prime() {
	
	_region = RegionForEntireFile(_fileID[0]);
	
	if(_seekSampleTime) {
		_region.mStartFrame = _seekSampleTime;
		_pauseTimeAccumulator += _seekSampleTime;
	} else if(_pauseTimeStamp.mSampleTime > 0) {
		_region.mStartFrame = _pauseTimeStamp.mSampleTime + _pauseTimeAccumulator;
		_pauseTimeAccumulator += _pauseTimeStamp.mSampleTime;
	} else {
		_pauseTimeAccumulator = 0;
	}
	
	if(_loopCount > 0) {
		_region.mLoopCount = _loopCount;
	}
	
	// resetting time-tracking members
	memset(&_pauseTimeStamp, 0, sizeof(_pauseTimeStamp));
	_loopCount = 0;
	_seekSampleTime = 0;
	
	if(!(_region.mTimeStamp.mFlags & kAudioTimeStampHostTimeValid)) {
		cout << "ofxAudioUnitFilePlayer has no file to play" << endl;
		return;
	}
	
	OFXAU_RETURN(AudioUnitSetProperty(*_unit,
	                                  kAudioUnitProperty_ScheduledFileIDs,
	                                  kAudioUnitScope_Global,
	                                  0,
	                                  _fileID,
	                                  sizeof(_fileID)),
	             "setting file player's file ID");
	
	OFXAU_RETURN(AudioUnitSetProperty(*_unit,
	                                  kAudioUnitProperty_ScheduledFileRegion,
	                                  kAudioUnitScope_Global,
	                                  0,
	                                  &_region,
	                                  sizeof(_region)),
	             "setting file player region");
	
	UInt32 framesToPrime = 0; // 0 = "use the default"
	OFXAU_RETURN(AudioUnitSetProperty(*_unit,
									  kAudioUnitProperty_ScheduledFilePrime,
									  kAudioUnitScope_Global,
									  0,
									  &framesToPrime,
									  sizeof(framesToPrime)),
				 "priming file player");
	
	_primed = true;
}

void ofxAudioUnitFilePlayer::play(uint64_t startTime) {
	
	if(!_primed) {
		prime();
	}
	
	_primed = false;
	
	if(startTime == 0) {
		startTime = mach_absolute_time();
	}
	
	AudioTimeStamp startTimeStamp = {0};
	FillOutAudioTimeStampWithHostTime(startTimeStamp, startTime);
	
	OFXAU_RETURN(AudioUnitSetProperty(*_unit,
	                                  kAudioUnitProperty_ScheduleStartTimeStamp,
	                                  kAudioUnitScope_Global,
	                                  0,
	                                  &startTimeStamp,
	                                  sizeof(startTimeStamp)),
	             "setting file player start time");
}

void ofxAudioUnitFilePlayer::playAtSampleTime(SInt64 sampleTime) {
	_seekSampleTime = sampleTime;
	_pauseTimeAccumulator = 0;
	play();
}

void ofxAudioUnitFilePlayer::loop(unsigned int timesToLoop, uint64_t startTime) {
	_loopCount = timesToLoop;
	play(startTime);
}

AudioTimeStamp ofxAudioUnitFilePlayer::pause() {
	UInt32 size = sizeof(_pauseTimeStamp);
	AudioUnitGetProperty(*_unit,
						 kAudioUnitProperty_CurrentPlayTime,
						 kAudioUnitScope_Global,
						 0,
						 &_pauseTimeStamp,
						 &size);
	
	reset();
	return _pauseTimeStamp;
}

void ofxAudioUnitFilePlayer::stop() {
	_primed = false;
	_pauseTimeAccumulator = 0;
	_seekSampleTime = 0;
	_pauseTimeStamp = (AudioTimeStamp){0};
	reset();
}

#pragma mark - Util

ScheduledAudioFileRegion RegionForEntireFile(AudioFileID fileID) {
	ScheduledAudioFileRegion region = {0};
	UInt64 numPackets = 0;
	UInt32 dataSize = sizeof(numPackets);
	
	AudioFileGetProperty(fileID,
	                     kAudioFilePropertyAudioDataPacketCount,
	                     &dataSize,
	                     &numPackets);
	
	AudioStreamBasicDescription asbd = {0};
	dataSize = sizeof(asbd);
	
	AudioFileGetProperty(fileID, kAudioFilePropertyDataFormat, &dataSize, &asbd);
	
	// defining a region which basically says "play the whole file"
	region.mTimeStamp.mFlags       = kAudioTimeStampHostTimeValid;
	region.mTimeStamp.mSampleTime  = 0;
	region.mCompletionProc         = NULL;
	region.mCompletionProcUserData = NULL;
	region.mAudioFile              = fileID;
	region.mLoopCount              = 0;
	region.mStartFrame             = 0;
	region.mFramesToPlay           = numPackets * asbd.mFramesPerPacket;
	
	return region;
}
