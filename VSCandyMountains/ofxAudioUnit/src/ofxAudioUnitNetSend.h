#pragma once

#include "ofxAudioUnitBase.h"

// ofxAudioUnitNetSend wraps the AUNetSend unit.
// This audio unit allows you to send audio to
// an AUNetReceive via bonjour. It supports a handful
// of different stream formats as well.

// You can change the stream format by calling
// setFormat() with one of "kAUNetSendPresetFormat_"
// constants

// Multiple AUNetSends are differentiated by port
// number, NOT by bonjour name

class ofxAudioUnitNetSend : public ofxAudioUnit
{
public:
	ofxAudioUnitNetSend();
	void setName(const std::string &name);
	void setPort(unsigned int portNumber);
	void setFormat(unsigned int formatIndex);
};
