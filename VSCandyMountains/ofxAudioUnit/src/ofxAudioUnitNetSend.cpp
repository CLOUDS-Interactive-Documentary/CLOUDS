#include "ofxAudioUnit.h"
#include "ofxAudioUnitUtils.h"

AudioComponentDescription netSendDesc = {
	kAudioUnitType_Effect,
	kAudioUnitSubType_NetSend,
	kAudioUnitManufacturer_Apple
};

// ----------------------------------------------------------
ofxAudioUnitNetSend::ofxAudioUnitNetSend()
// ----------------------------------------------------------
{
	_desc = netSendDesc;
	initUnit();
	setName("openFrameworks");
}

// ----------------------------------------------------------
void ofxAudioUnitNetSend::setPort(unsigned int portNumber)
// ----------------------------------------------------------
{
	UInt32 pNum = portNumber;
	OFXAU_PRINT(AudioUnitSetProperty(*_unit,
									 kAUNetSendProperty_PortNum, 
									 kAudioUnitScope_Global,
									 0,
									 &pNum,
									 sizeof(pNum)),
				"setting net send port number");
}

// ----------------------------------------------------------
void ofxAudioUnitNetSend::setName(const std::string &name)
// ----------------------------------------------------------
{
	CFStringRef serviceName = CFStringCreateWithCString(kCFAllocatorDefault,
														name.c_str(),
														kCFStringEncodingUTF8);
	
	OFXAU_PRINT(AudioUnitSetProperty(*_unit,
									 kAUNetSendProperty_ServiceName,
									 kAudioUnitScope_Global,
									 0,
									 &serviceName,
									 sizeof(serviceName)),
				"setting net send service name");
	
	CFRelease(serviceName);
}

// ----------------------------------------------------------
void ofxAudioUnitNetSend::setFormat(unsigned int formatIndex)
// ----------------------------------------------------------
{
	UInt32 format = formatIndex;
	OFXAU_PRINT(AudioUnitSetProperty(*_unit,
									 kAUNetSendProperty_TransmissionFormatIndex,
									 kAudioUnitScope_Global,
									 0,
									 &format,
									 sizeof(format)),
				"setting net send format");
}
