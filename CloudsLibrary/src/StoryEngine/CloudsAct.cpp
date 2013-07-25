//
//  CloudsAct.cpp
//  CloudsStoryEngine
//
//  Created by Surya Mattu on 04/07/13.
//
//

#include "CloudsAct.h"

CloudsAct::CloudsAct(){
    
	timelinePopulated = false;
    duration = 0;
}

CloudsAct::~CloudsAct(){
	if(timelinePopulated){
		ofRemoveListener(timeline.events().bangFired, this, &CloudsAct::timelineEventFired);
	}
}


void CloudsAct::play(){
    
    CloudsActEventArgs args(this);
    ofNotifyEvent(events.actBegan, args);
	
	timeline.play();
}

void CloudsAct::populateTime(){
	
	timelinePopulated = true;
	
	timeline.setSpacebarTogglePlay(false);
    timeline.setAutosave(false);
    timeline.setup();
	timeline.disableEvents();
	
    timeline.clear();
    
    timeline.setDurationInSeconds(duration);
    topicsTrack = timeline.addFlags("Topics");
    visualSystemsTrack = timeline.addFlags("Visual Systems");
    clipsTrack = timeline.addFlags("Clips");
    clipPreRollTrack = timeline.addFlags("Clip PreRoll Flags");
    questionsTrack = timeline.addFlags("Questions");
    
    timeline.setInPointAtSeconds(0);
    string previousTopic = "";
    string currentTopic = "";
    
    for(int i=0; i < actItems.size(); i++){
        ActTimeItem& item = actItems[i];
        
        if(item.type == Clip){
            currentTopic = topicMap[item.key];
            
            clipsTrack->addFlagAtTime(item.key, item.startTime * 1000);
            clipsTrack->addFlagAtTime(" ", item.endTime * 1000);
            
            
            if(currentTopic != previousTopic){
                topicsTrack->addFlagAtTime(currentTopic, item.startTime * 1000);
            }
            
            previousTopic = currentTopic;
        }
        else if(item.type == VS){
            if(item.startTime != item.introEndTime){
                visualSystemsTrack->addFlagAtTime("start :" + item.key, item.startTime * 1000);
                visualSystemsTrack->addFlagAtTime("intro :" + item.key, item.introEndTime * 1000);
            }
            else{
                visualSystemsTrack->addFlagAtTime("start :" + item.key, item.startTime * 1000);
            }
            
            if(item.endTime != item.outroStartTime){
                visualSystemsTrack->addFlagAtTime("outro :" + item.key, item.outroStartTime * 1000);
                visualSystemsTrack->addFlagAtTime("end :" + item.key, item.endTime * 1000);
            }
            else{
                visualSystemsTrack->addFlagAtTime("outro :" + item.key, item.outroStartTime * 1000);
            }
            
            
        }
        else if(item.type == PreRoll){
            clipPreRollTrack->addFlagAtTime(item.key, item.startTime * 1000);
        }
        else if (item.type == Gap){
            //nothing for now
        }
        else if(item.type == Question){
            questionsTrack->addFlagAtTime(item.key, item.startTime*1000);
        }
    }
    
	//TODO remove listener
    ofAddListener(timeline.events().bangFired, this, &CloudsAct::timelineEventFired);
}

void CloudsAct::timelineEventFired(ofxTLBangEventArgs& bang){
    if(bang.track == clipsTrack){
        if(bang.flag ==" "){
            
        }
        else{
            CloudsClipEventArgs args(clipMap[bang.flag], topicMap[bang.flag],getDichotomiesForClip(bang.flag));
            ofNotifyEvent(events.clipBegan, args);
        }
        
    }
    else if(bang.track == visualSystemsTrack){
        //split string on %, send VS either began or ended
        vector <string> presetId;
        presetId = ofSplitString(bang.flag, ":");
        
        CloudsVisualSystemEventArgs args(visualSystemsMap[presetId[1]]);
		if(presetId[0] == "start " ){
            cout<<"Starting Visual System " << visualSystemsMap[presetId[1]].getID()<<endl;
//			ofNotifyEvent(events.visualSystemBegan, args);
		}
		else if(presetId[0] == "outro "){
            cout<<"Ending Visual System " << visualSystemsMap[presetId[1]].getID()<<endl;
//			ofNotifyEvent(events.visualSystemEnded, args);
		}
        
    }
    else if(bang.track == questionsTrack){
        CloudsQuestionEventArgs args(questionsMap[bang.flag]);
        ofNotifyEvent(events.questionAsked, args);
    }
    else if(bang.track == clipPreRollTrack){
        vector<string> clipName = ofSplitString(bang.flag, "%");
        cout<<"sending clip: "<<clipName[1] <<" with Offset: "<< actItemsMap[bang.flag].handleLength << endl;
        CloudsPreRollEventArgs args(clipMap[clipName[1]],actItemsMap[bang.flag].handleLength);
        ofNotifyEvent(events.preRollRequested, args);
        
    }
}

float CloudsAct::getActDuration(){
    return duration;
}


vector<keywordDichotomy>& CloudsAct:: getDichotomiesForClip(string clipName){
    
    if(dichotomiesMap.find(clipName) != dichotomiesMap.end()){
        return dichotomiesMap[clipName];
    }
    cout<<"dichotomoies not found for clip: "<< clipName<<endl;
    return dummyDichotomies;
}
vector<CloudsVisualSystemPreset>& CloudsAct::getAllVisualSystems(){
    return visualSystems;
}

void CloudsAct::drawDebug(){
	
    timeline.setOffset(ofVec2f(0,ofGetHeight()/3));
    timeline.draw();
}

CloudsClip& CloudsAct::getClip(int index){
    return clips[index];
}

CloudsClip& CloudsAct:: getClipAtTime(float time){
    for(int i=0; i< clips.size(); i++){
        ActTimeItem item = getItemForClip(clips[i]);
        if(time >= item.startTime && time <= item.endTime){
            return clips[i];
        }
    }
    cout<<"No clip found at that time!"<<endl;
    return dummyClip;
}

CloudsVisualSystemPreset& CloudsAct::getVisualSystemInAct(int index){
    return visualSystems[index];
}

ActTimeItem& CloudsAct::getItemForClip(CloudsClip& clip){
    if(clipMap.find(clip.getLinkName()) == clipMap.end()){
        ofLogError() << "Couldn't find Act Item for cilp " << clip.getLinkName();
        return dummy;
    }
    return clipItems[clip.getLinkName()];
}

ActTimeItem& CloudsAct::getItemForVisualSystem(CloudsVisualSystemPreset& preset){
    if(visualSystemItems.find(preset.getID()) == visualSystemItems.end()){
        ofLogError()<<"Can't find Act item for Visual System"<<endl;
    }
    return visualSystemItems[preset.getID()];
}

void CloudsAct::addClip(CloudsClip clip, string topic, float startTime, float handleLength,vector<keywordDichotomy> currentDichotomiesBalance){
    clips.push_back(clip);
    clipMap[clip.getLinkName()] = clip;
    topicMap[clip.getLinkName()] = topic;
    
    cout<<"added " <<clip.getLinkName()<< " to clip map "<<endl;
    ActTimeItem item;
    
    item.type = Clip;
    item.key = clip.getLinkName();
    item.startTime = startTime;
    item.endTime = startTime+clip.getDuration() + handleLength;
    duration = MAX(item.endTime, duration);
    
    actItems.push_back(item);
    actItemsMap[item.key] = item;
    dichotomiesMap[item.key] = currentDichotomiesBalance;
    clipItems[clip.getLinkName()] = item;
    
}

void CloudsAct::addClip(CloudsClip clip, string topic, float startTime){
    clips.push_back(clip);
    clipMap[clip.getLinkName()] = clip;
    topicMap[clip.getLinkName()] = topic;
    
    cout<<"added " <<clip.getLinkName()<< " to clip map "<<endl;
    ActTimeItem item;
    
    item.type = Clip;
    item.key = clip.getLinkName();
    item.startTime = startTime;
    //defaulting handle length to 1
    item.endTime = startTime+clip.getDuration() + 1;
    duration = MAX(item.endTime, duration);
    
    actItems.push_back(item);
    actItemsMap[item.key] = item;
    clipItems[clip.getLinkName()] = item;
}

void CloudsAct::addVisualSystem(CloudsVisualSystemPreset preset, float startTime, float duration){
    visualSystems.push_back(preset);
    visualSystemsMap[preset.getID()] = preset;
    
    cout<<"added " <<preset.getID()<< " to VS map "<<endl;
    ActTimeItem item;
    // start the visual system halfway through the clip
    float vsStartTime  = startTime;
    item.type = VS;
    item.key =preset.getID();
    item.startTime = vsStartTime;
    item.endTime = vsStartTime + duration;
    item.introEndTime = vsStartTime + preset.introDuration;
    item.outroStartTime = item.endTime -preset.outroDuration;
    
    duration = MAX(item.endTime, duration);
    
    actItems.push_back(item);
    actItemsMap[item.key] = item;
    visualSystemItems[preset.getID()] = item;
}

void CloudsAct::addGapForVisualSystem(float startTime){
    ActTimeItem item;
    item.type = Gap;
    item.startTime = startTime;
    item.endTime = startTime + visualSystemDuration;
    
    duration = MAX(item.endTime, duration);
    
    actItems.push_back(item);
    
}
void CloudsAct::addClipPreRollFlag(float preRollFlagTime, float clipHandleLength, string clipName){
    ActTimeItem item;
    item.type = PreRoll;
    item.key = "%" + clipName;
    item.startTime = preRollFlagTime;
    item.endTime = preRollFlagTime;
    item.handleLength = clipHandleLength;
    
    actItemsMap[item.key] = item;
    actItems.push_back(item);
}

void CloudsAct::addQuestion(CloudsClip clip, float startTime){
    ActTimeItem item;
    item.type = Question;
    
    //making the key the first question for now
    //TODO: MAKE THIS LESS ARBITRARY
    item.key = clip.getQuestionsVector()[ofRandom(clip.getQuestionsVector().size()-1)];
    
    item.startTime = startTime;
    //dont care about end time as it will end with visual system;
    item.endTime = startTime + 10;
    
    questionsMap[item.key] = clip;
    actItems.push_back(item);
    
}

void CloudsAct::removeQuestionAtTime(float startTime, float duration){
    float endTime = startTime + duration;
    for(int i =0; i<actItems.size(); i++){
        if(actItems[i].type == Question){
            if(actItems[i].startTime > startTime && actItems[i].startTime < endTime){
                questionsMap.erase(actItems[i].key);
                actItems.erase(actItems.begin() + i);
                
            }
        }
    }
}

CloudsClip& CloudsAct :: getClipForQuestion(string question){
    if(questionsMap.find(question) == questionsMap.end()){
        ofLogError() << "Couldn't find Clip Item with Starting Question " << question;
        return dummyClip;
    }
    return questionsMap[question];
}

vector<CloudsClip>& CloudsAct::getAllClips(){
    return clips;
}

string CloudsAct::getTopicForClip(CloudsClip& clip){
    return topicMap[ clip.getLinkName() ];
}

//vector<string>& CloudsAct::getAllTopics(){
//    return topicHistory;
//}

//void CloudsAct::addTopicToHistory(string topic){
//    topicHistory.push_back(topic);
//}

void CloudsAct::setTopicForClip(string topic, string clipName)
{
    topicMap[clipName] = topic;
}

void CloudsAct::clear(){
    clips.clear();
    //    topicHistory.clear();
    clipMap.clear();
    visualSystemsMap.clear();
    topicMap.clear();
    actItems.clear();
    timeline.reset();
    duration = 0;
}

CloudsEvents& CloudsAct::getEvents(){
	return events;
}