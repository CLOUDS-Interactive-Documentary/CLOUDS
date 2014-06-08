#include "ofxAudioUnit.h"
#include "ofxAudioUnitUtils.h"

#include <sstream>

AudioComponentDescription netReceiveDesc = {
	kAudioUnitType_Generator,
	kAudioUnitSubType_NetReceive,
	kAudioUnitManufacturer_Apple
};

// ----------------------------------------------------------
ofxAudioUnitNetReceive::ofxAudioUnitNetReceive()
// ----------------------------------------------------------
{
	_desc = netReceiveDesc;
	initUnit();
}

// ----------------------------------------------------------
void ofxAudioUnitNetReceive::connectToHost(const std::string &address, unsigned long port)
// ----------------------------------------------------------
{
	stringstream ss;
	ss << address << ":" << port;
	CFStringRef hostName = CFStringCreateWithCString(kCFAllocatorDefault,
													 ss.str().c_str(),
													 kCFStringEncodingUTF8);
	
	OFXAU_PRINT(AudioUnitSetProperty(*_unit,
									 kAUNetReceiveProperty_Hostname,
									 kAudioUnitScope_Global,
									 0,
									 &hostName,
									 sizeof(hostName)),
				"setting net receive host name");
	
	// setting net send disconnect to 0 to connect net receive because that makes sense
	UInt32 connect = 0;
	OFXAU_PRINT(AudioUnitSetProperty(*_unit,
									 kAUNetSendProperty_Disconnect,
									 kAudioUnitScope_Global,
									 0,
									 &connect,
									 sizeof(connect)),
				"connecting net receive");
	
	CFRelease(hostName);
}

// ----------------------------------------------------------
void ofxAudioUnitNetReceive::disconnect()
// ----------------------------------------------------------
{
	UInt32 disconnect = 1;
	OFXAU_PRINT(AudioUnitSetProperty(*_unit,
									 kAUNetSendProperty_Disconnect,
									 kAudioUnitScope_Global,
									 0,
									 &disconnect,
									 sizeof(disconnect)),
				"disconnecting net receive");
}
