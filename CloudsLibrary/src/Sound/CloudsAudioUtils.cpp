//
//  CloudsAudioUtils.cpp
//  Colony
//
//  Created by Elias Zananiri on 2015-04-22.
//
//

#include "CloudsAudioUtils.h"

#include "ofxAudioDecoder.h"

Tonic::SampleTable CloudsAudioLoadMp3File(string path)
{    
    ofxAudioDecoder audio;
    audio.load(path);
    
    ofLogNotice("CloudsAudioLoadMp3File") << "Opened file " << path << endl
        << "\tSample rate: " << audio.getSampleRate() << endl
        << "\tChannels: " << audio.getChannels() << endl
        << "\tFrames: " << audio.getNumFrames();
    
    Tonic::SampleTable sampleTable = Tonic::SampleTable(audio.getNumFrames(), audio.getChannels());
    memcpy(sampleTable.dataPointer(), audio.getRawSamples().data(), audio.getNumSamples() * sizeof(float));
    
    return sampleTable;
}
