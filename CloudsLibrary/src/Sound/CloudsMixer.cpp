//
//  CloudsMixer.cpp
//  CLOUDS
//
//  Created by Gal Sasson on 12/18/13.
//
//

#include "CloudsMixer.h"

CloudsMixer::CloudsMixer()
{
	musicVol = 1.0; // RTcmix main volume
	diageticVol = 1.0; // Tonic main volume
    
    // envelope follower
    attack = 200.0;
    decay = 17640.0;
    followgain = 0.;
    // compressor
    thresh = 0.2; // set lower for a quieter squish point
    ratio = 3.; // set higher for more squish

    showCompressor = false;
}

CloudsMixer::~CloudsMixer()
{
    if (musicArgs.buffer) {
        free(musicArgs.buffer);
    }
    if (diageticArgs.buffer) {
        free(diageticArgs.buffer);
    }
}

void CloudsMixer::setup(int nChannels, int sampleRate, int bufferSize, int nBuffers)
{
    size_t size = nChannels*bufferSize*sizeof(float);
    
    musicArgs.buffer = (float*)malloc(size);
    musicArgs.bufferSize = bufferSize;
    musicArgs.nChannels = nChannels;
    diageticArgs.buffer = (float*)malloc(size);
    diageticArgs.bufferSize = bufferSize;
    diageticArgs.nChannels = nChannels;
    
    // initialize OF audio streaming
    ofSoundStreamSetup(nChannels, 0, this, sampleRate, bufferSize, nBuffers);
    ofSoundStreamStart();
}

void CloudsMixer::fadeMusicDown()
{
    
}

void CloudsMixer::fadeMusicUp()
{
    
}

void CloudsMixer::setMusicVolume(float vol)
{
    musicVol = vol;
}

void CloudsMixer::setDiageticVolume(float vol)
{
    diageticVol = vol;
}

//void CloudsMixer::fillBuffer(float *output, int bufferSize, int nChannels)
void CloudsMixer::audioOut(float * output, int bufferSize, int nChannels )
{
    // check for buffer size mismatch
    if (bufferSize != musicArgs.bufferSize ||
        bufferSize != diageticArgs.bufferSize) {
        return;
    }
    
    // check for channels mismatch
    if (nChannels != musicArgs.nChannels ||
        nChannels != diageticArgs.nChannels)
    {
        return;
    }
    
    size_t size = nChannels*bufferSize*sizeof(float);
    // fill music buffer
    memset(musicArgs.buffer, 0, size);
	ofNotifyEvent(GetCloudsAudioEvents()->musicAudioRequested, musicArgs, this);
    
    // fill diagetic buffer
    memset(diageticArgs.buffer, 0, size);
	ofNotifyEvent(GetCloudsAudioEvents()->diageticAudioRequested, diageticArgs, this);
    
    // mix
    for (int i=0; i<bufferSize*nChannels; i++)
    {
        output[i] = musicArgs.buffer[i]*musicVol + diageticArgs.buffer[i]*diageticVol;
        
        // Luke's Compressor
        float current = abs(output[i]);
        if(current>followgain) {
            followgain = followgain + ((current-followgain)/attack);
        }
        else
        {
            followgain = followgain + ((current-followgain)/decay);
        }
        if(followgain>thresh) gain = 1.0-((followgain-thresh)*ratio); else gain = 1.0;
        
        output[i]=output[i]*gain*MASTER_GAIN;
        
        // clip
        if (output[i] > 1) {
            output[i] = 1;
        }
        else if (output[i] < -1) {
            output[i] = -1;
        }
    }
    
    /*
    if(showCompressor) {
        for(float i = 0;i<0.5;i=i+0.01)
        {
            if (followgain>i) cout << "•"; else cout << " ";
        }
        cout << " : ";
        for(float i = 0.5;i<1.;i=i+0.01)
        {
            if (gain>i) cout << "•"; else cout << " ";
        }
        cout << endl;
    }
     */
    
    //cout << followgain << " : " << gain << endl;
    
    
    
}