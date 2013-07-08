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
    waitingForNextClip = false;
}

void CloudsAct::update(){
    
    if(actPlaying){
       
        if(waitingForNextClip&&nextClipTime<timer.getAppTimeSeconds()){
            loadNextClip();
        }
    }
}

void CloudsAct::loadNextClip(){
    currentClip = clips[currentPlayIndex];
    currentTopic = topicHistory[currentPlayIndex];
    currentPlayIndex++;
    
    CloudsStoryEventArgs args(currentClip,clips,currentTopic);
    ofNotifyEvent(events.clipBegan, args, this); 
}


void CloudsAct::playAct(){
    currentPlayIndex = 0;
    currentClip = clips[currentPlayIndex];
    currentTopic = topicHistory[currentPlayIndex];
    currentTime = timer.getAppTimeSeconds();
    actPlaying = true;
    clipEndTime = currentTime +currentClip.getDuration();

    CloudsStoryEventArgs argsA(currentClip,clips,currentTopic);
    ofNotifyEvent(events.actBegan,argsA ,this);
    
    CloudsStoryEventArgs argsB(currentClip,clips,currentTopic);
    ofNotifyEvent(events.clipBegan, argsB,this);
    
}



bool CloudsAct::clipEnded(){
   
    CloudsStoryEventArgs args(currentClip,clips,currentTopic);
    ofNotifyEvent(events.clipEnded, args, this);
    
    if(currentPlayIndex<clips.size()){
        waitingForNextClip = true;
        nextClipTime = timer.getAppTimeSeconds()+args.timeUntilNextClip;
    }
    else{
        ofNotifyEvent(events.actEnded, args, this);
    }
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

CloudsClip& CloudsAct::getClipInAct(int index){
    return clips[index];
}

void CloudsAct::addClipToAct(CloudsClip clip){
    clips.push_back(clip);
}

vector<CloudsClip>& CloudsAct::getAllClips(){
    return clips;
}


string CloudsAct::getTopicInHistory(int index){
    return topicHistory[index];
}

vector<string>& CloudsAct::getAllTopics(){
    return topicHistory;
}

void CloudsAct::setTopicInHistory(string topic){
    topicHistory.push_back(topic);
}

void CloudsAct::clearAct(){
    clips.clear();
    topicHistory.clear();
}


CloudsEvents& CloudsAct::getEvents(){
	return events;
}