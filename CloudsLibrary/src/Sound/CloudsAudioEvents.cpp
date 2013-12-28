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

ofPtr<CloudsAudioEvents> GetCloudsAudioEvents(){
	if(cloudsAudioEvents == NULL){
        cloudsAudioEvents = ofPtr<CloudsAudioEvents>(new CloudsAudioEvents());
	}
	return cloudsAudioEvents;
}

