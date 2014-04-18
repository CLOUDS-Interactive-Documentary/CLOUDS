#pragma once

#include "ofxAudioUnitBase.h"

// ofxAudioUnitNetReceive wraps the AUNetReceive unit.
// This audio unit receives audio from an AUNetSend.
// Call connectToHost() with the IP Address of a
// host running an instance of AUNetSend in order to
// connect.

// For example, myNetReceive.connectToHost("127.0.0.1")
// will start streaming audio from an AUNetSend on the
// same machine.

class ofxAudioUnitNetReceive : public ofxAudioUnit
{
public:
	ofxAudioUnitNetReceive();
	void connectToHost(const std::string &ipAddress, unsigned long port = 52800);
	void disconnect();
};
