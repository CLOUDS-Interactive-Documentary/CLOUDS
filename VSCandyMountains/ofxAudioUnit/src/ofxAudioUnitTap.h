#pragma once

#include "ofxAudioUnitDSPNode.h"
#include "ofPolyline.h"

// ofxAudioUnitTap acts like an Audio Unit (as in, you
// can connect it to other Audio Units). In reality, it
// hooks up two Audio Units to each other, but also copies
// the samples that are passing through the chain. This
// allows you to extract the samples to do audio visualization,
// FFT analysis and so on.

// Samples retreived from the ofxAudioUnitTap will be floating
// point numbers between -1 and 1. It is possible for them to exceed
// this range, but this will typically be due to an Audio Unit
// overloading its output.

// Note that if you just want to know how loud the audio is,
// the ofxAudioUnitMixer will allow you to access that
// value with less CPU overhead.

class ofxAudioUnitTap : public ofxAudioUnitDSPNode
{
public:
	explicit ofxAudioUnitTap(unsigned int samplesToBuffer = 2048);
	ofxAudioUnitTap(const ofxAudioUnitTap &orig);
	ofxAudioUnitTap& operator=(const ofxAudioUnitTap &orig);
	virtual ~ofxAudioUnitTap();
	
	// Container for samples returned from an ofxAudioUnitTap
	typedef std::vector<AudioUnitSampleType> MonoSamples;
	
	struct StereoSamples
	{
		ofxAudioUnitTap::MonoSamples left;
		ofxAudioUnitTap::MonoSamples right;
		size_t size(){return min(left.size(), right.size());}
		bool empty(){return left.empty() || right.empty();}
	};
	
	void setBufferLength(unsigned int samplesToBuffer);
	
	void getSamples(StereoSamples &outData) const;
	void getSamples(MonoSamples &outData) const;
	void getSamples(MonoSamples &outData, unsigned int channel) const;
	
	// These output an ofPolyline representing the waveform of the most recent samples in the buffer.
	// You can use the "sampleRate" param to skip samples for the sake of speed (i.e. a sampleRate
	// of 3 = every 3rd sample will be represented in the resulting ofPolyline)
	void getStereoWaveform(ofPolyline &outLeft, ofPolyline &outRight, float width, float height, unsigned sampleRate = 1);
	void getLeftWaveform(ofPolyline &outLine, float width, float height, unsigned sampleRate = 1);
	void getRightWaveform(ofPolyline &outLine, float width, float height, unsigned sampleRate = 1);
	
	// These output the RMS (i.e. "loudness") of the most recent buffer
	float getRMS(unsigned int channel);
	float getLeftChannelRMS()  {return getRMS(0);}
	float getRightChannelRMS() {return getRMS(1);}
	
private:
	MonoSamples _tempBuffer;
};
