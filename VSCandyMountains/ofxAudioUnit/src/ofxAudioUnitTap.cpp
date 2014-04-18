#include "ofxAudioUnit.h"
#include <Accelerate/Accelerate.h>

// ----------------------------------------------------------
ofxAudioUnitTap::ofxAudioUnitTap(unsigned int samplesToTrack)
// ----------------------------------------------------------
{
	setBufferSize(samplesToTrack);
}

// ----------------------------------------------------------
ofxAudioUnitTap::~ofxAudioUnitTap()
// ----------------------------------------------------------
{
	
}

// ----------------------------------------------------------
ofxAudioUnitTap::ofxAudioUnitTap(const ofxAudioUnitTap& orig)
// ----------------------------------------------------------
{
	setBufferSize(orig.getBufferSize());
}

// ----------------------------------------------------------
ofxAudioUnitTap& ofxAudioUnitTap::operator=(const ofxAudioUnitTap &orig)
// ----------------------------------------------------------
{
	setBufferSize(orig.getBufferSize());
	return *this;
}

// ----------------------------------------------------------
void ofxAudioUnitTap::setBufferLength(unsigned int samplesToBuffer)
// ----------------------------------------------------------
{
	setBufferSize(samplesToBuffer);
}

#pragma mark - Samples

// ----------------------------------------------------------
void ofxAudioUnitTap::getSamples(MonoSamples &outData) const
// ----------------------------------------------------------
{
	getSamplesFromChannel(outData, 0);
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getSamples(MonoSamples &outData, unsigned int channel) const
// ----------------------------------------------------------
{
	getSamplesFromChannel(outData, channel);
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getSamples(StereoSamples &outData) const
// ----------------------------------------------------------
{
	getSamplesFromChannel(outData.left, 0);
	getSamplesFromChannel(outData.right, 1);
}

#pragma mark - RMS

// ----------------------------------------------------------
float ofxAudioUnitTap::getRMS(unsigned int channel)
// ----------------------------------------------------------
{
	getSamplesFromChannel(_tempBuffer, channel);
	float rms;
	vDSP_rmsqv(&_tempBuffer[0], 1, &rms, _tempBuffer.size());
	return rms;
}

#pragma mark - Waveforms

// ----------------------------------------------------------
void WaveformForBuffer(const ofxAudioUnitTap::MonoSamples &buffer, float width, float height, ofPolyline &outLine, unsigned sampleRate)
// ----------------------------------------------------------
{	
	outLine.clear();
	
	const float xStep = width / (buffer.size() / sampleRate);
	float x = 0;
	
	for (int i = 0; i < buffer.size(); i += sampleRate, x += xStep)
	{
#if TARGET_OS_IPHONE
		SInt16 s = SInt16(buffer[i] >> 9);
		float y = ofMap(s, -32768, 32767, height, 0, true);
#else
		float y = ofMap(buffer[i], -1, 1, height, 0, true);
#endif
		outLine.addVertex(ofPoint(x, y));
	}
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getLeftWaveform(ofPolyline &outLine, float w, float h, unsigned sampleRate)
// ----------------------------------------------------------
{
	getSamples(_tempBuffer, 0);
	WaveformForBuffer(_tempBuffer, w, h, outLine, sampleRate);
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getRightWaveform(ofPolyline &outLine, float w, float h, unsigned sampleRate)
// ----------------------------------------------------------
{
	getSamples(_tempBuffer, 1);
	WaveformForBuffer(_tempBuffer, w, h, outLine, sampleRate);
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getStereoWaveform(ofPolyline &l, ofPolyline &r, float w, float h, unsigned rate)
// ----------------------------------------------------------
{
	getLeftWaveform(l, w, h, rate);
	getRightWaveform(r, w, h, rate);
}
