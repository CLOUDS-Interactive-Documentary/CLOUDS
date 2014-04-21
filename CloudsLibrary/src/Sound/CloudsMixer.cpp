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
	musicVol = 1.0;  // RTcmix main volume
	diageticVol = 1.0; // Tonic main volume
    
    // envelope follower
    attack = 200.0;
    decay = 17640.0;
    followgain = 0.;
    // compressor
    thresh = 0.2; // set lower for a quieter squish point
    ratio = 3.; // set higher for more squish

    showCompressor = false;
    
    fsig = 0; // no fade
    GetCloudsAudioEvents()->fadeValue = 1.0; // normal gain
    dval = 0.; // delay gain
    famt = 0.025; // fade amount
	GetCloudsAudioEvents()->gain = 0;
}

CloudsMixer::~CloudsMixer()
{
    if (musicArgs.buffer) {
        free(musicArgs.buffer);
    }
    if (diageticArgs.buffer) {
        free(diageticArgs.buffer);
    }
    if (delayLine.buffer) {
        free(delayLine.buffer);
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
    
    size = nChannels*44100*sizeof(float); // 1 second delay line
    delayLine.buffer = (float*)malloc(size);
    delayLine.bufferSize = 44100;
    delayLine.nChannels = nChannels;
    delptr = 0;

    // initialize OF audio streaming
    ofSoundStreamSetup(nChannels, 0, this, sampleRate, bufferSize, nBuffers);
    ofSoundStreamStart();
    
    ofAddListener(GetCloudsAudioEvents()->fadeAudioDown, this, &CloudsMixer::fadeDown);
    ofAddListener(GetCloudsAudioEvents()->fadeAudioUp, this, &CloudsMixer::fadeUp);
}

void CloudsMixer::setMusicVolume(float vol)
{
	musicVol = vol;
}

void CloudsMixer::setDiageticVolume(float vol)
{
    diageticVol = vol;
}

///LUKE STUBBS
void CloudsMixer::fadeDown(float& time){
    cout << "SOUND: fading down" << endl;
    fsig = -1;
    famt = 1.0/((44100./512.)*time);
    
}

void CloudsMixer::fadeUp(float& time){
    cout << "SOUND: fading up" << endl;
    fsig = 1;
    famt = 1.0/((44100./512.)*time);
}
//LUKE STUBBS

//void CloudsMixer::fillBuffer(float *output, int bufferSize, int nChannels)
void CloudsMixer::audioOut(float * output, int bufferSize, int nChannels )
{
    GetCloudsAudioEvents()->dopull = GetCloudsAudioEvents()->fadeValue > 0;
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
        output[i] = (musicArgs.buffer[i]*musicVol*GetCloudsAudioEvents()->fadeValue) + diageticArgs.buffer[i]*diageticVol;
        
		 // read from delay
        if(GetCloudsAudioEvents()->dodelay) {
            output[i]+=delayLine.buffer[(delptr-44099+44100)%44100]*dval;
            delayLine.buffer[delptr%44100] = delayLine.buffer[delptr%44100]*0.8;
        }
		// write into delay
        else {
            delayLine.buffer[delptr%44100] = musicArgs.buffer[i]*musicVol + diageticArgs.buffer[i]*diageticVol;
        }
        delptr++;
        
        
        // Luke's Compressor
        float current = abs(output[i]);
        if(current>followgain) {
            followgain = followgain + ((current-followgain)/attack);
        }
        else{
            followgain = followgain + ((current-followgain)/decay);
        }
        if(followgain>thresh) GetCloudsAudioEvents()->gain = 1.0-((followgain-thresh)*ratio); else GetCloudsAudioEvents()->gain = 1.0;
        
        output[i] = output[i]*GetCloudsAudioEvents()->gain*MASTER_GAIN;
        
        // clip
        if (output[i] > 1) {
            output[i] = 1;
        }
        else if (output[i] < -1) {
            output[i] = -1;
        }
    }
    
    // figure out when delay turns off
    if(GetCloudsAudioEvents()->dodelay)
    {
        float delsum = 0.;
        for(int i = 0;i<44100;i++)
        {
            delsum += fabs(delayLine.buffer[i]);
        }
        if(delsum<20.) {
            GetCloudsAudioEvents()->dodelay = false;
            dval = 0.;
        }
    }
    
    // adjust fade
    if(fsig==1) // fading up
    {
        GetCloudsAudioEvents()->fadeValue+=famt;
        if(GetCloudsAudioEvents()->fadeValue>0.999)
        {
            GetCloudsAudioEvents()->fadeValue = 1.0;
            fsig = 0;
            delptr = 0; // reset delay line
        }
    }
    else if(fsig==-1) // fading down
    {
        GetCloudsAudioEvents()->fadeValue-=famt;
        dval+=famt; // fade in delay while fading out audio
        if(dval>1.0) dval = 1.0;
        if(GetCloudsAudioEvents()->fadeValue<0.001)
        {
            GetCloudsAudioEvents()->fadeValue = 0.;
            fsig = 0;
            if(GetCloudsAudioEvents()->setupflush) {
                GetCloudsAudioEvents()->doflush = true;   
            }
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