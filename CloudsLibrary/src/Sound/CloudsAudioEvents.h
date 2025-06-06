//
//  CloudsAudioEvents.h
//  CLOUDS
//
//  Created by Gal Sasson on 12/19/13.
//
//


#pragma once

#include "ofMain.h"
#include "ofEvents.h"

//#define MASTER_GAIN 0.45
#define MASTER_GAIN 1.

class CloudsAudioEvents {
public:
    CloudsAudioEvents();
    ofEvent<ofAudioEventArgs>   musicAudioRequested;
    ofEvent<ofAudioEventArgs>   diageticAudioRequested;
    ofEvent<float>   fadeAudioDown;
    ofEvent<float>   fadeAudioUp;
    bool dopull;
    bool setupflush;
    bool doflush;
    bool respawn;
    bool dodelay;
	float gain;
	float fadeValue;
};

ofPtr<CloudsAudioEvents> GetCloudsAudioEvents();

