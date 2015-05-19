//
//  CloudsRun.cpp
//  CloudsTransitionTest
//
//  Created by James George on 9/1/13.
//
//

#include "CloudsRun.h"
#include "CloudsGlobal.h"
#include "ofxXmlSettings.h"
#include "CloudsFCPParser.h"

void CloudsRun::clear(){
    actCount = 0;
    clipHistory.clear();
    presetHistory.clear();
    topicHistory.clear();
    
    //only for the last run
    accumuluatedTopics.clear();
}

bool CloudsRun::load(CloudsFCPParser* parser){
    
    clear();
    
    string runFilePath = GetCloudsDataPath() + "runs/run.xml";
    if(!ofFile(runFilePath).exists()){
        return false;
    }
    ofxXmlSettings settings;
    settings.load(runFilePath);
    
    settings.pushTag("run");
    actCount = settings.getValue("actCount", 0);
    questionsAsked = settings.getValue("questionsAsked", 0);
    
    settings.pushTag("clips");
    int numClips = settings.getNumTags("clip");
    for(int i = 0; i < numClips; i++){
        clipHistory.push_back(parser->getClipWithID(settings.getValue("clip", "", i)));
    }
    settings.popTag(); //clips

    ///////////////////////////

    settings.pushTag("presets");
    int numPresets = settings.getNumTags("preset");
    for(int i = 0; i < numPresets; i++){
        presetHistory.push_back(settings.getValue("preset", "", i));
    }
    settings.popTag(); //presets
    
    ///////////////////////////
    
    settings.pushTag("topics");
    int numTopics = settings.getNumTags("topic");
    for(int i = 0; i < numTopics; i++){
        topicHistory.push_back(settings.getValue("topic", "", i));
    }
    settings.popTag(); //topics
    
    
    settings.popTag(); // run
    
    return true;
}

void CloudsRun::save(){
    saveAs(GetCloudsDataPath() + "runs/run.xml");
}

void CloudsRun::saveAs(string filePath){
    
    string runFilePath = GetCloudsDataPath() + "runs/run.xml";
    ofxXmlSettings settings;
    
    settings.addTag("run");
    settings.pushTag("run");
    
    settings.addValue("actCount", actCount);
    settings.addValue("questionsAsked", questionsAsked);

    settings.addTag("clips");
    settings.pushTag("clips");
    for(int i = 0; i < clipHistory.size(); i++){
        settings.addValue("clip", clipHistory[i]->getID());
    }
    settings.popTag(); //clips
    
    ///////////////////////////
    
    settings.addTag("presets");
    settings.pushTag("presets");
    for(int i = 0; i < presetHistory.size(); i++){
        settings.addValue("preset", presetHistory[i]);
    }
    settings.popTag(); //presets

    ///////////////////////////
 
    settings.addTag("topics");
    settings.pushTag("topics");
    for(int i = 0; i < topicHistory.size(); i++){
        settings.addValue("topic", topicHistory[i]);
    }
    settings.popTag(); //topics

    settings.popTag(); //run;

    settings.save(filePath);
}

void CloudsRun::archive(){
    saveAs( GetCloudsDataPath() + "runs/run_" + ofGetTimestampString() + ".xml");
}

void CloudsRun::actBegan(CloudsActEventArgs &args){
    actCount++;
}

void CloudsRun::actEnded(CloudsActEventArgs &args){
    save();
}

void CloudsRun::clipBegan(CloudsClipEventArgs &args){
 
    for(int i = 0; i < args.chosenClip->getKeywords().size(); i++){
        accumuluatedTopics[ args.chosenClip->getKeywords()[i] ]++;
    }

	clipHistory.push_back(args.chosenClip);
}

void CloudsRun::visualSystemBegan(CloudsVisualSystemEventArgs& args){
    
    if( ! ofContains(presetHistory, args.preset.getID())){
        presetHistory.push_back(args.preset.getID());
    }
    
}

void CloudsRun::topicChanged(CloudsTopicEventArgs& args){
    
    if(!ofContains(topicHistory, args.topic)){
        topicHistory.push_back(args.topic);
    }
}

set<string> CloudsRun::getVisitedTopics(){
    return set<string>(topicHistory.begin(), topicHistory.end());
}

set<string> CloudsRun::getVisitedPeople(){
    set<string> peopleSet;
    for(int i = 0; i < clipHistory.size(); i++){
        peopleSet.insert(clipHistory[i]->person);
    }
    return peopleSet;
}

set<string> CloudsRun::getVisitedVisuals(){
    set<string> visualset;
    for(int i = 0; i < presetHistory.size(); i++){
        visualset.insert(ofSplitString(presetHistory[i], "_",true,true)[0]);
    }
    return visualset;
}


bool CloudsRun::historyContainsClip(CloudsClip* clip){
    for(int i = 0; i < clipHistory.size(); i++){
        if(clip->getID() == clipHistory[i]->getID()){
            return true;
        }
    }
    return false;
}

void CloudsRun::visualSystemEnded(CloudsVisualSystemEventArgs& args){}
void CloudsRun::questionProposed(CloudsQuestionEventArgs& args){}
void CloudsRun::questionSelected(CloudsQuestionEventArgs& args){}
void CloudsRun::preRollRequested(CloudsPreRollEventArgs& args){}

