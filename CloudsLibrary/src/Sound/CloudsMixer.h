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

class CloudsMixer : public ofBaseApp //extends base app to become the sound stream
{
public:
    CloudsMixer();
    ~CloudsMixer();
    void setup(int nChannels = 2, int sampleRate = 44100, int bufferSize = 512, int nBuffers = 2);
    
    void setMusicVolume(float vol);
    void setDiageticVolume(float vol);
    void fadeMusicDown();
    void fadeMusicUp();

    //legacy
    void fillBuffer(float *output, int bufferSize, int nChannels){
        audioOut(output,bufferSize,nChannels);
    }
    //*****
    
    void audioOut(float * output, int bufferSize, int nChannels );

    CloudsAudioEvents& getEvents() { return events; };
    
    //receive fade events from the act timelines
    void fadeDown(float& time);
    void fadeUp(float& time);
    
    bool showCompressor;
    float followgain;
    float gain;
    
protected:
    ofAudioEventArgs musicArgs;
    ofAudioEventArgs diageticArgs;
    
    float musicVol;
    float diageticVol;

    // compressor data
    float attack;
    float decay;
    float thresh;
    float ratio;
    int fsig;
    float fval;
    
    CloudsAudioEvents events;
};


#endif /* defined(__CloudsScoreDesigner__CloudsMixer__) */
