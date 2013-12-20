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
    void setup(int nChannels, int sampleRate, int bufferSize, int nBuffers);
    
    void setMusicVolume(float vol);
    void setDiageticVolume(float vol);
    void fillBuffer(float *output, int bufferSize, int nChannels);
    
    CloudsAudioEvents& getEvents() { return events; };

protected:
    ofAudioEventArgs musicArgs;
    ofAudioEventArgs diageticArgs;
    
    float musicVol;
    float diageticVol;
    
    CloudsAudioEvents events;
};


#endif /* defined(__CloudsScoreDesigner__CloudsMixer__) */
