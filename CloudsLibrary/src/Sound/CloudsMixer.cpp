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
//    events = new CloudsMixerEvents();
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
    ofSoundStreamSetup(nChannels, 0, sampleRate, bufferSize, nBuffers);
    ofSoundStreamStart();
}


void CloudsMixer::setMusicVolume(float vol)
{
    musicVol = vol;
}

void CloudsMixer::setDiageticVolume(float vol)
{
    diageticVol = vol;
}

void CloudsMixer::fillBuffer(float *output, int bufferSize, int nChannels)
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

        // clip
        if (output[i] > 1) {
            output[i] = 1;
        }
        else if (output[i] < -1) {
            output[i] = -1;
        }
    }
    
    
}