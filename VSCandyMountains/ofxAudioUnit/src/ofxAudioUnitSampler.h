#pragma once

#include "ofxAudioUnitBase.h"

class ofxAudioUnitSampler : public ofxAudioUnit
{
public:
	ofxAudioUnitSampler();
    ofxAudioUnitSampler(OSType type,
                        OSType subType,
                        OSType manufacturer);
    
    ofxAudioUnitSampler(const ofxAudioUnitSampler &orig);
    ofxAudioUnitSampler& operator=(const ofxAudioUnitSampler &orig);
    
    
	
	bool setSample(const std::string &samplePath);
	bool setSamples(const std::vector<std::string> &samplePaths);
    void setVolume(float volume);
    
    void midiEvent(const UInt32 status, const UInt32 data1, const UInt32 data2);
    void midiCC(const UInt32 controller, const UInt32 value);
    
    void setChannel(const UInt32 channel) { midiChannelInUse = channel; };
    void setBank(const UInt32 MSB, const UInt32 LSB);
    void setProgram(const UInt32 prog); 
    void midiNoteOn(const UInt32 note, const UInt32 vel);
    void midiNoteOff(const UInt32 note, const UInt32 vel);
    
    
    UInt32 midiChannelInUse;
    
    enum {
        kMidiMessage_ControlChange      = 0xB,
        kMidiMessage_ProgramChange      = 0xC,
        kMidiMessage_BankMSBControl     = 0,
        kMidiMessage_BankLSBControl     = 32,
        kMidiMessage_NoteOn             = 0x9,
        kMidiMessage_NoteOff            = 0x8
    };



};
