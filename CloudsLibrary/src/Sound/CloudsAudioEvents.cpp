//
//  CloudsAudioEvents.cpp
//  CLOUDS
//
//  Created by Gal Sasson on 12/19/13.
//
//

#include "CloudsAudioEvents.h"

///////////// //STATICS
static ofPtr<CloudsAudioEvents> cloudsAudioEvents;

CloudsAudioEvents::CloudsAudioEvents(){
    dopull = false;
    setupflush = false;
    doflush = false;
    respawn = false;
    dodelay = false;
    gain = 0.0;
    fadeValue = 0.0;
}

ofPtr<CloudsAudioEvents> GetCloudsAudioEvents(){
	if(cloudsAudioEvents == NULL){
        cloudsAudioEvents = ofPtr<CloudsAudioEvents>(new CloudsAudioEvents());
	}
	return cloudsAudioEvents;
}

