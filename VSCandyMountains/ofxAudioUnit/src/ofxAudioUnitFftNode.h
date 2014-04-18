#pragma once

#include "ofxAudioUnitDSPNode.h"
#include <Accelerate/Accelerate.h>

typedef enum {
	OFXAU_WINDOW_HAMMING,
	OFXAU_WINDOW_HANNING,
	OFXAU_WINDOW_BLACKMAN
}
ofxAudioUnitWindowType;

typedef enum {
	OFXAU_SCALE_LINEAR,
	OFXAU_SCALE_DECIBEL,
	OFXAU_SCALE_LOG10
}
ofxAudioUnitScaleType;

class ofxAudioUnitFftNode : public ofxAudioUnitDSPNode {
public:
	struct Settings {
		ofxAudioUnitWindowType window;
		ofxAudioUnitScaleType scale;
		bool normalizeInput;
		bool normalizeOutput;
		bool clampMinToZero;
		
		Settings()
		: window(OFXAU_WINDOW_HAMMING)
		, scale(OFXAU_SCALE_LOG10)
		, normalizeInput(true)
		, normalizeOutput(true)
		, clampMinToZero(true)
		{ }
	};
	
	ofxAudioUnitFftNode(unsigned int fftBufferSize = 1024, Settings settings = Settings());
	ofxAudioUnitFftNode(const ofxAudioUnitFftNode &orig);
	ofxAudioUnitFftNode& operator=(const ofxAudioUnitFftNode &orig);
	virtual ~ofxAudioUnitFftNode();
	
	bool getAmplitude(std::vector<float> &outAmplitude);
	bool getPhase(std::vector<float> &outPhase);
	
	// this should be set to a power of 2 (512, 1024, 2048, etc), and will be rounded up otherwise
	void setFftBufferSize(unsigned int bufferSize);
	
	void setSettings(const Settings &settings);
	void setWindowType(ofxAudioUnitWindowType windowType);
	void setScale(ofxAudioUnitScaleType scaleType);
	void setNormalizeInput(bool normalizeInput);
	void setNormalizeOutput(bool normalizeOutput);
	void setClampMinToZero(bool clampToZero);
	
private:
	Settings _outputSettings;
	unsigned int _N;
	unsigned int _log2N;
	unsigned int _currentMaxLog2N;
	FFTSetup _fftSetup;
	COMPLEX_SPLIT _fftData;
	float * _window;
	std::vector<AudioUnitSampleType> _sampleBuffer;
	void freeBuffers();
};
