//
//  CloudsAct.cpp
//  CloudsStoryEngine
//
//  Created by Surya Mattu on 04/07/13.
//
//

#include "CloudsAct.h"

CloudsAct::CloudsAct(){
    currentPlayIndex = 0;
    currentTime = 0;
    actPlaying =false;
}

void CloudsAct::update(){
    
    if(actPlaying){
        currentTime += ofGetElapsedTimef() -currentTime;
        if(currentTime>clipEndTime){
            playNextClip();
        }
    }


}

void CloudsAct::playNextClip(){
    
    currentClip = clips[currentPlayIndex++];
    clipEndTime = currentTime + currentClip.getDuration();
}


void CloudsAct::playAct(){
    currentPlayIndex = 0;
    currentClip = clips[currentPlayIndex];
    currentTime = ofGetElapsedTimef();
    actPlaying = true;
    clipEndTime = currentTime +currentClip.getDuration();
    

}


float CloudsAct::getActDuration(){
    float totalDuration;
    for(int i=0; i < clips.size(); i++){
        totalDuration += clips[i].getDuration();
    }
}

void CloudsAct::drawActDebug(){
    float totalTime = 0;
	for(int i = 0; i < clips.size(); i++){
		totalTime += clips[i].getDuration();
	}
	
	int currentTime = 0;
	for(int i = 0; i < clips.size(); i++){
		float screenX = ofMap(currentTime, 0, totalTime,  0, ofGetWidth());
		float width = ofMap(clips[i].getDuration(), 0, totalTime,  0, ofGetWidth());
		currentTime += clips[i].getDuration();
		ofNoFill();
		ofRect(screenX, 100 + 30*i, width, 30);
		ofDrawBitmapString(clips[i].getLinkName(), screenX+10, 100 + 30*(i+.75));
	}
    
}


CloudsEvents& CloudsAct::getEvents(){
	return events;
}