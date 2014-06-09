#pragma once

#include "ofxAudioUnitBase.h"

// ofxAudioUnitFilePlayer wraps the AUAudioFilePlayer unit.
// This audio unit allows you to play any file that
// Core Audio supports (mp3, aac, caf, aiff, etc)

class ofxAudioUnitFilePlayer : public ofxAudioUnit
{
public:
	ofxAudioUnitFilePlayer();
	~ofxAudioUnitFilePlayer();
	
	bool   setFile(const std::string &filePath);
	
	// you can use this pair of functions to make sure two
	// file players stay in sync while looping. Call setLength()
	// on one using the return value of getLength() from another.
	UInt32 getLength() const;
	void   setLength(UInt32 length);
	
	enum {
		OFX_AU_LOOP_FOREVER = -1
	};
	
	// the startTime arg for play() and loop() allows you to specify
	// a time in the future for playback to start. The intention for
	// this is to make it easier to get a handful of ofxAudioUnitFilePlayers
	// to start in sync with each other. It is not necessary to specify
	// a start time.
	
	// Ex.
	// uint64_t now = mach_absolute_time();
	// player1.play(now);
	// player2.play(now);
	// .. etc ..
	
	void play(uint64_t startTime = 0);
	void loop(unsigned int timesToLoop = OFX_AU_LOOP_FOREVER, uint64_t startTime = 0);
	void stop();
	AudioTimeStamp pause(); // returns the timestamp the file player is paused at
	
	// this will start playback at a specified sample time (e.g. if your
	// file is 44100hz (likely) then starting at 44100 * 3 will start
	// playback 3 seconds into the file)
	void playAtSampleTime(SInt64 sampleTime);
	
	// this call will block until the file player is ready to play.
	// play() will call this on its own, so you only need to call this
	// ahead of time if you want the minimum amount of latency after
	// calling play()
	void prime();
	
	AudioTimeStamp getCurrentTimestamp() const;
	
private:
	AudioFileID _fileID[1];
	ScheduledAudioFileRegion _region;
	AudioTimeStamp _pauseTimeStamp;
	Float64 _pauseTimeAccumulator;
	SInt64 _seekSampleTime;
	unsigned int _loopCount;
	bool _primed;
};
