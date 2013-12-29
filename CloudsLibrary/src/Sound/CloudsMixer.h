//
//  CloudsMixer.h
//  CLOUDS
//
//  Created by Gal Sasson on 12/18/13.
//
//

#ifndef __CloudsMixer__
#define __CloudsMixer__

#include "ofMain.h"
#include "ofEvents.h"
#include "CloudsAudioEvents.h"

class CloudsMixer
{
public:
    CloudsMixer();
    ~CloudsMixer();
    void setup(int nChannels = 2, int sampleRate = 44100, int bufferSize = 512, int nBuffers = 2);
    
    void setMusicVolume(float vol);
    void setDiageticVolume(float vol);
    void fillBuffer(float *output, int bufferSize, int nChannels);
    
    CloudsAudioEvents& getEvents() { return events; };
    
    bool showCompressor;
    
protected:
    ofAudioEventArgs musicArgs;
    ofAudioEventArgs diageticArgs;
    
    float musicVol;
    float diageticVol;
    float masterGain;
    
    // compressor data
    float attack;
    float decay;
    float thresh;
    float ratio;
    float followgain;
    
    CloudsAudioEvents events;
};


#endif /* defined(__CloudsScoreDesigner__CloudsMixer__) */
