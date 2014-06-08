#include "ofxAudioUnitSampler.h"
#include "ofxAudioUnitUtils.h"

#if (MAC_OS_X_VERSION_10_7 || __IPHONE_5_0)

AudioComponentDescription samplerDesc = {
	kAudioUnitType_MusicDevice,
	kAudioUnitSubType_Sampler,
	kAudioUnitManufacturer_Apple
};

// ----------------------------------------------------------
ofxAudioUnitSampler::ofxAudioUnitSampler()
// ----------------------------------------------------------
{
	_desc = samplerDesc;
	initUnit();
    midiChannelInUse = 0;
}

// ----------------------------------------------------------
ofxAudioUnitSampler::ofxAudioUnitSampler(OSType type,
						   OSType subType,
						   OSType manufacturer)
// ----------------------------------------------------------
{
	_desc.componentType         = type;
	_desc.componentSubType      = subType;
	_desc.componentManufacturer = manufacturer;
	_desc.componentFlags        = 0;
	_desc.componentFlagsMask    = 0;
	initUnit();
    midiChannelInUse = 0;
};


// ----------------------------------------------------------
ofxAudioUnitSampler::ofxAudioUnitSampler(const ofxAudioUnitSampler &orig)
// ----------------------------------------------------------
{
	_desc = orig._desc;
    initUnit();
    midiChannelInUse = orig.midiChannelInUse;
}



// ----------------------------------------------------------
ofxAudioUnitSampler& ofxAudioUnitSampler::operator=(const ofxAudioUnitSampler &orig)
// ----------------------------------------------------------
{
	if(this == &orig) return *this;
	
	_desc = orig._desc;
	initUnit();
    midiChannelInUse = orig.midiChannelInUse;
	
	return *this;
}


// ----------------------------------------------------------
bool ofxAudioUnitSampler::setSample(const std::string &samplePath)
// ----------------------------------------------------------
{
	CFURLRef sampleURL[1] = {CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
																	 (const UInt8 *)samplePath.c_str(),
																	 samplePath.length(),
																	 NULL)};
	
	CFArrayRef sample = CFArrayCreate(NULL, (const void **)&sampleURL, 1, &kCFTypeArrayCallBacks);

	OFXAU_PRINT(AudioUnitSetProperty(*_unit,
									 kAUSamplerProperty_LoadAudioFiles,
									 kAudioUnitScope_Global,
									 0,
									 &sample,
									 sizeof(sample)),
				"setting ofxAudioUnitSampler's source sample");
	
	CFRelease(sample);
	CFRelease(sampleURL[0]);
	
	return true;
}

// ----------------------------------------------------------
bool ofxAudioUnitSampler::setSamples(const std::vector<std::string> &samplePaths)
// ----------------------------------------------------------
{
	CFURLRef sampleURLs[samplePaths.size()];
	
	for(int i = 0; i < samplePaths.size(); i++)
	{
		sampleURLs[i] = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
																(const UInt8 *)samplePaths[i].c_str(),
																samplePaths[i].length(),
																NULL);
	}
	
	CFArrayRef samples = CFArrayCreate(NULL, (const void **)&sampleURLs, samplePaths.size(), &kCFTypeArrayCallBacks);
	
	OSStatus s = AudioUnitSetProperty(*_unit,
									 kAUSamplerProperty_LoadAudioFiles,
									 kAudioUnitScope_Global,
									 0,
									 &samples,
									 sizeof(samples));
	
	OFXAU_PRINT(s, "setting ofxAudioUnitSampler's source samples");
	
	for(int i = 0; i < samplePaths.size(); i++) CFRelease(sampleURLs[i]);
	
	CFRelease(samples);
	
	return s == noErr;
}

// ----------------------------------------------------------
void ofxAudioUnitSampler::midiEvent(const UInt32 status, const UInt32 data1, const UInt32 data2)
// ----------------------------------------------------------
{
    MusicDeviceMIDIEvent(*_unit, status, data1, data2, 0);
}

// ----------------------------------------------------------
void ofxAudioUnitSampler::midiCC(const UInt32 controller, const UInt32 value)
// ----------------------------------------------------------
{
    MusicDeviceMIDIEvent(*_unit,
                         kMidiMessage_ControlChange << 4 | midiChannelInUse,
                         controller, value,
                         0/*sample offset*/);
    
}

// ----------------------------------------------------------
void ofxAudioUnitSampler::setBank(const UInt32 MSB, const UInt32 LSB)
// ----------------------------------------------------------
{
    MusicDeviceMIDIEvent(*_unit,
                         kMidiMessage_ControlChange << 4 | midiChannelInUse,
                         kMidiMessage_BankMSBControl, MSB,
                         0/*sample offset*/);
    
    MusicDeviceMIDIEvent(*_unit,
                         kMidiMessage_ControlChange << 4 | midiChannelInUse,
                         kMidiMessage_BankLSBControl, LSB,
                         0/*sample offset*/);
}

// ----------------------------------------------------------
void ofxAudioUnitSampler::setProgram(const UInt32 prog)
// ----------------------------------------------------------
{
    MusicDeviceMIDIEvent(*_unit,
                         kMidiMessage_ProgramChange << 4 | midiChannelInUse,
                         prog, 0,
                         0/*sample offset*/);
}


// ----------------------------------------------------------
void ofxAudioUnitSampler::midiNoteOn(const UInt32 note, const UInt32 vel)
// ----------------------------------------------------------
{
    UInt32 noteOnCommand = 	kMidiMessage_NoteOn << 4 | midiChannelInUse;
    
    MusicDeviceMIDIEvent(	*_unit,
                         noteOnCommand,
                         note,
                         vel,
                         0);
}

// ----------------------------------------------------------
void ofxAudioUnitSampler::midiNoteOff(const UInt32 note, const UInt32 vel)
// ----------------------------------------------------------
{
    UInt32 noteOffCommand = kMidiMessage_NoteOff << 4 | midiChannelInUse;
    
    MusicDeviceMIDIEvent(	*_unit,
                         noteOffCommand,
                         note,
                         vel,
                         0);
}

// ----------------------------------------------------------
void ofxAudioUnitSampler::setVolume(float volume)
// ----------------------------------------------------------
{
    AudioUnitSetParameter(*_unit,
                          kMusicDeviceParam_Volume,
                          kAudioUnitScope_Global,
                          0,
                          volume,
                          0);
}




#else

#if MAC_OS_X_VERSION_10_6

#warning AUSampler doesn't exist on 10.6. ofxAudioUnitSampler is wrapping the DLSSynth instead

AudioComponentDescription samplerDesc = {
	kAudioUnitType_MusicDevice,
	kAudioUnitSubType_DLSSynth,
	kAudioUnitManufacturer_Apple
};

ofxAudioUnitSampler::ofxAudioUnitSampler()
{
	_desc = samplerDesc;
	initUnit();
}

bool ofxAudioUnitSampler::setSample(const std::string &samplePath){return false;}
bool ofxAudioUnitSampler::setSamples(const std::vector<std::string> &samplePaths){return false;}

#endif // MAC_OS_X_VERSION_10_6

#endif // (MAC_OS_X_VERSION_10_7 || __IPHONE_5_0)
