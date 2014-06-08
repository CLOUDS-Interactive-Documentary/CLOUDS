#pragma once

#include <AudioToolbox/AudioToolbox.h>
#include <string>
#include <vector>
#include "ofTypes.h"
#include "AUParamInfo.h"

typedef ofPtr<AudioUnit> AudioUnitRef;
class ofxAudioUnitDSPNode;

// ofxAudioUnit is a general-purpose class to simplify using Audio Units in
// openFrameworks apps. There are also several subclasses of ofxAudioUnit
// which provide convenience functions for specific Audio Units (eg. file player,
// mixer, output, speech synth...)

// There is also a class named ofxAudioUnitTap which acts like an Audio Unit,
// but allows you to read samples that are passing through your Audio Unit
// chains. For example, putting an ofxAudioUnitTap between a synth and an
// output unit will allow you to read the samples being sent from the synth
// to the output.

class ofxAudioUnit
{
public:
	ofxAudioUnit(){};
	explicit ofxAudioUnit(AudioComponentDescription description);
	explicit ofxAudioUnit(OSType type,
						  OSType subType,
						  OSType manufacturer = kAudioUnitManufacturer_Apple);
	ofxAudioUnit(const ofxAudioUnit &orig);
	ofxAudioUnit& operator=(const ofxAudioUnit &orig);
	
	virtual ~ofxAudioUnit();
	
	virtual ofxAudioUnit& connectTo(ofxAudioUnit &otherUnit, int destinationBus = 0, int sourceBus = 0);
	virtual ofxAudioUnitDSPNode& connectTo(ofxAudioUnitDSPNode &node);
	OF_DEPRECATED_MSG("Use connectTo() instead.", virtual ofxAudioUnit& operator>>(ofxAudioUnit& otherUnit));
	OF_DEPRECATED_MSG("Use connectTo() instead.", virtual ofxAudioUnitDSPNode& operator>>(ofxAudioUnitDSPNode& node));
	
	// By default, this just calls AudioUnitRender() on the underlying AudioUnit. However, some
	// subclasses require more complex rendering behaviour
	virtual OSStatus render(AudioUnitRenderActionFlags *ioActionFlags,
							const AudioTimeStamp *inTimeStamp,
							UInt32 inOutputBusNumber,
							UInt32 inNumberFrames,
							AudioBufferList *ioData);
	
	// explicit and implicit conversions to the underlying AudioUnit pointer
	AudioUnit getUnit()       {return *_unit;}
	operator AudioUnit()      {return *_unit;}
	AudioUnitRef getUnitRef() {return _unit;}
	
	// This pair of functions will look for the preset in the apps's data folder and
	// append ".aupreset" to the name parameter
	bool loadCustomPreset(const std::string &presetName);
	bool saveCustomPreset(const std::string &presetName);
	
	// This pair of functions expect an absolute path (including the file extension)
	bool saveCustomPresetAtPath(const std::string &presetPath);
	bool loadCustomPresetAtPath(const std::string &presetPath);
	
	// Retrieves a list of all parameters the unit is advertising (helpful for 3rd party units)
	std::vector<AudioUnitParameterInfo> getParameterList(bool includeExpert = true, bool includeReadOnly = false);
	void printParameterList(bool includeExpert = true, bool includeReadOnly = false);
	
	// some convenience functions for common properties
	void setRenderCallback(AURenderCallbackStruct callback, int destinationBus = 0);
	void setParameter(AudioUnitParameterID property, AudioUnitScope scope, AudioUnitParameterValue value, int bus = 0);
	void reset(){AudioUnitReset(*_unit, kAudioUnitScope_Global, 0);}
	
#if !(TARGET_OS_IPHONE)
	void showUI(const std::string &title = "Audio Unit UI",
				int x = 100,
				int y = 100,
				bool forceGeneric = false);
#endif
	
protected:
	AudioUnitRef _unit;
	
	AudioComponentDescription _desc;
	AudioUnitRef allocUnit(AudioComponentDescription desc);
	void initUnit();
	bool loadPreset(const CFURLRef &presetURL);
	bool savePreset(const CFURLRef &presetURL);
	
	static void AudioUnitDeleter(AudioUnit * unit);
};
