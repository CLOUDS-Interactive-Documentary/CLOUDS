#pragma once

#include "ofxAudioUnitBase.h"

// ofxAudioUnitMatrixMixer wraps the AUMatrixMixer
// this unit allows you to have an arbitrary number
// of input / output busses, with an arbitrary mix
// between inputs and outputs (e.g. an input on bus
// 2 could have a level of 0.2 in output bus 7, and
// an level of 0.6 in output bus 4).

// This is unfinished, and should be considered very
// alpha. Feel free to get in contact with me if you
// need a hand figuring out how to use it, since it
// is not intuitive in the slightest.

class ofxAudioUnitMatrixMixer : public ofxAudioUnit {

public:
	ofxAudioUnitMatrixMixer(UInt32 inputBusCount = 8, UInt32 outputBusCount = 8, AudioStreamBasicDescription * inASBD = NULL, AudioStreamBasicDescription * outASBD = NULL);
	~ofxAudioUnitMatrixMixer();
	ofxAudioUnitMatrixMixer(const ofxAudioUnitMatrixMixer &orig);
	ofxAudioUnitMatrixMixer& operator=(const ofxAudioUnitMatrixMixer &orig);
	
	// this will remove any active connections, effectively resetting the unit
	void setChannelLayout(UInt32 inputBusCount, UInt32 outputBusCount, AudioStreamBasicDescription * inASBD = NULL, AudioStreamBasicDescription * outASBD = NULL);
	
private:
	void initWithLayout(UInt32 inputBusCount, UInt32 outputBusCount, AudioStreamBasicDescription * inASBD = NULL, AudioStreamBasicDescription * outASBD = NULL);
	std::vector<UInt32> getBusCount() const;
	std::vector<AudioStreamBasicDescription> getASBDs() const;
};
