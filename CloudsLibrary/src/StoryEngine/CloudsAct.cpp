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
    startTime = 0;
    actPlaying =false;
    waitingForNextClip = false;
    //TODO: Make non-arbritrary
    visualSystemDuration = 60;

}

void CloudsAct::update(){
    
    if(actPlaying){

        if(waitingForNextClip && nextClipTime<timer.getAppTimeSeconds()){
            loadNextClip();
        }
        
        if(timeToPlayVisualSystem()){
//            ofNotifyEvent(events.visualSystemBegan, args, this)
        }
        else if(visualSystemEndTime>timer.getAppTimeSeconds()){
//            ofNotifyEvent(events.visualSystemEnded, args, this);
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
    startTime = timer.getAppTimeSeconds();
    actPlaying = true;
    clipEndTime = startTime +currentClip.getDuration();

    //secsSinceLastVisualSystemPlayed =0;
    CloudsStoryEventArgs argsA(currentClip,clips,currentTopic);
    ofNotifyEvent(events.actBegan,argsA ,this);
    
    CloudsStoryEventArgs argsB(currentClip,clips,currentTopic);
    ofNotifyEvent(events.clipBegan, argsB,this);
    
}


void CloudsAct::populateTime(){
    for(int i=0; i < actItems.size(); i++){
        if(actItems[i].type ==Clip){
            
        }
        else if(actItems[i].type == VS){
            
        }
        else if (actItems[i].type == Gap){
            
        }
    }
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
		ofDrawBitmapString(clips[i].getLinkName() , screenX+10, 100 + 30*(i+.75));
	}
    
}

CloudsClip& CloudsAct::getClipInAct(int index){
    return clips[index];
}

ActTimeItem& CloudsAct::getItemForClip(CloudsClip& clip){
    if(clipMap.find(clip.getLinkName()) == clipMap.end()){
        ofLogError() << "Couldn't find Act Item for cilp " << clip.getLinkName();
        return dummy;
    }
    return clipItems[clip.getLinkName()];
}

void CloudsAct::addClipToAct(CloudsClip clip, float startTime){
    clips.push_back(clip);
    clipMap[clip.getLinkName()] = clip;
   
    ActTimeItem item;
    
    item.type = Clip; 
    item.key =clip.getLinkName();
    item.startTime = startTime;
    item.endTime = startTime+clip.getDuration();
    
    actItems.push_back(item);
    clipItems[clip.getLinkName()] = item;
}

void CloudsAct::addVisualSystem(CloudsVisualSystemPreset preset, float startTime, float duration){
    visualSystems.push_back(preset);
    visualSystemsMap[preset.getID()] = preset;
    
    ActTimeItem item;
    // start the visual system halfway through the clip
    float vsStartTime  = startTime;
    item.type = VS;
    item.key =preset.getID();
    item.startTime = vsStartTime;

    item.endTime = vsStartTime + duration;
    
    actItems.push_back(item);
    visualSystemItems[preset.getID()] = item;
    
}

void CloudsAct::addGapForVisualSystem(float startTime){
    ActTimeItem item;
    item.type = Gap;
    item.startTime = startTime;
    item.endTime = startTime + visualSystemDuration;
    
    actItems.push_back(item);
    
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