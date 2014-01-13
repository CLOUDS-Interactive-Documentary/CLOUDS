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

#define MASTER_GAIN 0.45

class CloudsAudioEvents {
public:
    ofEvent<ofAudioEventArgs>   musicAudioRequested;
    ofEvent<ofAudioEventArgs>   diageticAudioRequested;
    ofEvent<float>   fadeAudioDown;
    ofEvent<float>   fadeAudioUp;
};

ofPtr<CloudsAudioEvents> GetCloudsAudioEvents();

