//
//  CloudsAct.cpp
//  CloudsStoryEngine
//
//  Created by Surya Mattu on 04/07/13.
//
//

#include "CloudsAct.h"

bool delta_sort(pair<string,float> a, pair<string,float> b){
	return a.second > b.second;
}

CloudsAct::CloudsAct(){
    
	timelinePopulated = false;
    duration = 0;
	defaulPrerollDuration = 2.0;
}

CloudsAct::~CloudsAct(){
	if(timelinePopulated){
		ofRemoveListener(timeline.events().bangFired, this, &CloudsAct::timelineEventFired);
		ofRemoveListener(timeline.events().playbackEnded, this, &CloudsAct::timelineStopped);
	}
}

void CloudsAct::play(){
    
    CloudsActEventArgs args(this);
	
    ofNotifyEvent(events.actBegan, args);
	
	timeline.setCurrentTimeMillis(0);
	timeline.play();
}

void CloudsAct::populateTime(){
	
	timelinePopulated = true;
	
	timeline.setSpacebarTogglePlay(false);
    timeline.setAutosave(false);
    timeline.setup();
	timeline.setMinimalHeaders(true);
	timeline.disableEvents();
    timeline.clear();
    
	timeline.setLoopType(OF_LOOP_NONE);
    timeline.setDurationInSeconds(duration);
   	timeline.setInOutRange(ofRange(0,1.0));
 
	timeline.setPageName("story");
    topicsTrack = timeline.addFlags("Topics");
    visualSystemsTrack = timeline.addFlags("Visual Systems");
    vsGapsTrack = timeline.addFlags("VS Gap");
    clipsTrack = timeline.addFlags("Clips");
    difficultyTrack = timeline.addFlags("Clip Difficulty");
    clipPreRollTrack = timeline.addFlags("Clip PreRoll Flags");
    questionsTrack = timeline.addFlags("Questions");
    
    timeline.setInPointAtSeconds(0);
    string previousTopic = "";
    string currentTopic = "";
    float currentTopicStartTime = 0;
    for(int i=0; i < actItems.size(); i++){
        ActTimeItem& item = actItems[i];
        
        if(item.type == Clip){
            currentTopic = topicMap[item.key];
            
            clipsTrack->addFlagAtTime(item.key, item.startTime * 1000);
            clipsTrack->addFlagAtTime(" ", item.endTime * 1000);
            
            difficultyTrack->addFlagAtTime(clipDifficultyMap[item.key], item.startTime * 1000);
            if(currentTopic != previousTopic){
                topicsTrack->addFlagAtTime(currentTopic, item.startTime * 1000);
				if(previousTopic != ""){
					topicDurationMap[previousTopic] =  item.startTime - currentTopicStartTime;
				}
				topicHistory.push_back(previousTopic);
				currentTopicStartTime = item.startTime;
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
                visualSystemsTrack->addFlagAtTime("end :" + item.key, item.endTime * 1000);
            }
        }
        else if(item.type == PreRoll){
            clipPreRollTrack->addFlagAtTime(item.key, item.startTime * 1000);
        }
        else if (item.type == Gap){
            vsGapsTrack->addFlagAtTime("gap start:" + item.key, item.startTime * 1000);
            vsGapsTrack->addFlagAtTime("gap end:" + item.key, item.endTime * 1000);
            // SM: Just doing this to display on the timeline. No events sent to the playback controller.
        }
        else if(item.type == Question){
            questionsTrack->addFlagAtTime(item.key, item.startTime*1000);
        }
    }
	
	topicDurationMap[previousTopic] = duration - currentTopicStartTime;
    ofAddListener(timeline.events().bangFired, this, &CloudsAct::timelineEventFired);
    ofAddListener(timeline.events().playbackEnded, this, &CloudsAct::timelineStopped);

	timeline.addPage("dichotomies", true);
	dichotomyClips = timeline.addFlags("DichotomyClips");
	vector<CloudsDichotomy> dichotomiesBase = CloudsDichotomy::getDichotomies();
    for(int d = 0;  d < dichotomiesBase.size(); d++){
		string trackName = dichotomiesBase[d].left + "_" + dichotomiesBase[d].right;
		dichotomyTracks[trackName] = timeline.addCurves(trackName, ofRange(-5,5), 0);
	}
    
    if(clips.size() < 2){
        ofLogError("CloudsAct::populateTime") << "Not enough clips in act to create sections";
        return;
    }
	
	///////////////CALCULATE SOUND CUES
	CloudsClip& startClip = clips[0];
	
	cues.clear();
	CloudsSoundCue introCue;
	if(startClip.hasStartingQuestion() && startClip.getTopicsWithQuestions().size() > 0){
		string startTopic    = startClip.getTopicsWithQuestions()[0];
		string startQuestion = startClip.getQuestionForTopic(startTopic);
		introCue.soundQuestionKey = startTopic + ":" + startQuestion;
	}
	introCue.mixLevel = 1;
	introCue.startTime = clipItems[startClip.getLinkName()].startTime;
	introCue.duration = clipItems[clips[1].getLinkName()].startTime;
	introCue.dichotomies = dichotomiesMap[startClip.getLinkName()];
	cues.push_back(introCue);
	
	//calculate the 2 largest delta shifts;
	vector< pair<string,float> > clipDeltas;
	map<string, vector<CloudsDichotomy> >::iterator it;
	vector<float> lastValues;
	lastValues.resize(dichotomiesBase.size());
	
	bool firstLoop = true;
	for(int i = 1; i < clips.size()-2; i++){
		
		string clipID = clips[i].getLinkName();
		float startTime = clipItems[clipID].startTime;
		dichotomyClips->addFlagAtTime(clipID, startTime*1000);
		if(dichotomiesMap.find(clipID) == dichotomiesMap.end()){
			continue;
		}
		
		vector<CloudsDichotomy>& clipDichotomy = dichotomiesMap[clipID];
		float deltaChange = 0;
		for(int d = 0; d < dichotomiesBase.size(); d++){
			string trackName = dichotomiesBase[d].left + "_" + dichotomiesBase[d].right;
			dichotomyTracks[trackName]->addKeyframeAtMillis(clipDichotomy[d].balance, startTime*1000.0);
			if(!firstLoop){
				//deltaChange = MAX(abs(clipDichotomy[d].balance-lastValues[d]),deltaChange);
				int change = abs(clipDichotomy[d].balance - lastValues[d]);
				if(change > 1){
					deltaChange += change;
				}
			}
			lastValues[d] = clipDichotomy[d].balance;
		}

//		cout << "Delta for clip " << clipItems[clipID].key << " " << deltaChange << endl;
		
		if(!firstLoop){
			clipDeltas.push_back( make_pair(clipItems[clipID].key, deltaChange) );
		}
		firstLoop = false;
	}
	
	sort(clipDeltas.begin(), clipDeltas.end(), delta_sort);

	//MOOD CUES
	ofxTLFlags* sections = timeline.addFlags("Sections");
	if(clipDeltas.size() >= 2){
		float firstFlagTime = clipItems[clipDeltas[0].first].startTime;
		sections->addFlagAtTime(ofToString(clipDeltas[0].second), firstFlagTime*1000.);
		energyShiftClipIDs.push_back(clipDeltas[0].first);
		
		float secondFlagTime = clipItems[clipDeltas[1].first].startTime;
		//if the flag are too close jump
		if(abs(firstFlagTime-secondFlagTime) < 60 && clipDeltas.size() > 2){
			
			energyShiftClipIDs.push_back(clipDeltas[2].first);
			secondFlagTime = clipItems[clipDeltas[2].first].startTime;
			sections->addFlagAtTime(ofToString(clipDeltas[2].second), secondFlagTime*1000.);
			
		}
		else{
			energyShiftClipIDs.push_back(clipDeltas[1].first);
			sections->addFlagAtTime(ofToString(clipDeltas[1].second), secondFlagTime*1000.);
		}
		
		//Add 2 cues, act begun and mood shift
		//End of intro cue
		CloudsSoundCue actCue;
		actCue.mixLevel = 1;
		actCue.startTime = clipItems[clips[1].getLinkName()].startTime;
		actCue.duration = clipItems[ energyShiftClipIDs[0] ].startTime;
		actCue.dichotomies = dichotomiesMap[ clips[1].getLinkName() ];
		cues.push_back(actCue);
		
		CloudsSoundCue energyShift;
		energyShift.mixLevel = 1;
		energyShift.startTime = clipItems[ energyShiftClipIDs[0] ].startTime;
		energyShift.duration = duration - energyShift.startTime;
		energyShift.dichotomies = dichotomiesMap[startClip.getLinkName()];
		cues.push_back(energyShift);
		
	}
	else {
		ofLogError("CloudsAct::populateTime") << "Not enough clips to create section markers";
	}

	//create sound cue timeline debug
	ofxTLFlags* soundQueues = timeline.addFlags("Sound Cues");
	for(int i = 0; i < cues.size(); i++){
		soundQueues->addFlagAtTime( "cue", cues[i].startTime*1000 );
	}
	
	timeline.setCurrentPage(0);
	
}

bool CloudsAct::isClipEnergyShift(CloudsClip& clip){
	return ofContains(energyShiftClipIDs, clip.getLinkName());
}

void CloudsAct::timelineEventFired(ofxTLBangEventArgs& bang){
    if(bang.track == clipsTrack){
        if(bang.flag !=" "){
            CloudsClipEventArgs args(clipMap[bang.flag], topicMap[bang.flag],getDichotomiesForClip(bang.flag));
            ofNotifyEvent(events.clipBegan, args);
            cout << "CloudsAct::timelineEventFired SENDING CLIP BEGAN" << endl;
        }
    }
    else if(bang.track == visualSystemsTrack){
        //split string on %, send VS either began or ended
        vector <string> presetId;
        presetId = ofSplitString(bang.flag, ":");
        if(presetId.size() > 1){
			CloudsVisualSystemEventArgs args(visualSystems[ visualSystemIndeces[presetId[1]] ]);
			if(presetId[0] == "start " ){
				cout<<"Starting Visual System " << visualSystems[ visualSystemIndeces[presetId[1]] ].getID() << endl;
				ofNotifyEvent(events.visualSystemBegan, args);
			}
			else if(presetId[0] == "outro "){
				cout<<"Ending Visual System " << visualSystems[ visualSystemIndeces[presetId[1]] ].getID()<<endl;
				ofNotifyEvent(events.visualSystemEnded, args);
			}
		}
    }
    else if(bang.track == questionsTrack){
		CloudsClip& questionClip = questionsMap[bang.flag];
		string topic = ofSplitString( bang.flag, "??")[1];
		string question = questionClip.getQuestionForTopic(topic);
		cout << "** story engine :: creating question " << question << " with topic " << topic << endl;
        CloudsQuestionEventArgs args(questionClip,question,topic);
        ofNotifyEvent(events.questionAsked, args);
    }
    else if(bang.track == clipPreRollTrack){
        vector<string> clipName = ofSplitString(bang.flag, "%");
        cout<<"sending clip: "<<clipName[1] <<" with Offset: "<< actItemsMap[bang.flag].handleLength << endl;
        CloudsPreRollEventArgs args(clipMap[clipName[1]],actItemsMap[bang.flag].handleLength);
        ofNotifyEvent(events.preRollRequested, args);
    }
	else if(bang.track == topicsTrack){
        cout<<"sending topic: "<<bang.flag <<" with duration: "<< topicDurationMap[bang.flag] << endl;
		CloudsTopicEventArgs args(bang.flag, topicDurationMap[bang.flag] );
		ofNotifyEvent(events.topicChanged, args);
	}
}

void CloudsAct::timelineStopped(ofxTLPlaybackEventArgs& event){
	CloudsActEventArgs args(this);
    ofNotifyEvent(events.actEnded, args);
}

float CloudsAct::getActDuration(){
    return duration;
}

vector<CloudsSoundCue>& CloudsAct::getSoundCues(){
	return cues;
}

vector<CloudsDichotomy>& CloudsAct::getDichotomiesForClip(CloudsClip& clip){
	return getDichotomiesForClip(clip.getLinkName());
}

vector<CloudsDichotomy>& CloudsAct::getDichotomiesForClip(string clipName){
    
    if(dichotomiesMap.find(clipName) != dichotomiesMap.end()){
        return dichotomiesMap[clipName];
    }
    return dummyDichotomies;
}

vector< ofPtr<CloudsVisualSystem> > CloudsAct::getAllVisualSystems(){
	vector< ofPtr<CloudsVisualSystem> > vs;
	for(int i = 0; i < visualSystems.size(); i++){
		if(!ofContains(vs, visualSystems[i].system)){
			vs.push_back(visualSystems[i].system);
		}
	}
	return vs;
}

vector<CloudsVisualSystemPreset>& CloudsAct::getAllVisualSystemPresets(){
    return visualSystems;
}

void CloudsAct::drawDebug(){
	
    timeline.setOffset(ofVec2f(0,ofGetHeight()*.5));
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

float CloudsAct::getClipStartTime(CloudsClip& clip){
	return getItemForClip(clip).startTime;
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

float CloudsAct::addClip(CloudsClip& clip, string topic, float startTime){
	return addClip(clip, topic, startTime, 0, CloudsDichotomy::getDichotomies());
}

float CloudsAct::addClip(CloudsClip& clip, string topic, float startTime, float handleLength, vector<CloudsDichotomy> currentDichotomiesBalance){
    clips.push_back(clip);
    clipMap[clip.getLinkName()] = clip;
    topicMap[clip.getLinkName()] = topic;

    string clipDifficulty;
    if (clip.hasSpecialKeyword("easy")) {
        clipDifficulty = "easy";
    }
    else if(clip.hasSpecialKeyword("hard")){
        clipDifficulty = "hard";
    }
    else{
        clipDifficulty = "medium";
    }
	
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
    clipDifficultyMap[clip.getLinkName()] = clipDifficulty;
	
	//Preroll the clip
    ActTimeItem prerollItem;
    prerollItem.type = PreRoll;
    prerollItem.key = "%" + clip.getLinkName();
    prerollItem.startTime = item.startTime - defaulPrerollDuration;
    prerollItem.endTime = prerollItem.startTime;
    prerollItem.handleLength = handleLength;
    
    actItemsMap[prerollItem.key] = prerollItem;
    actItems.push_back(prerollItem);
	
	return duration;
}

void CloudsAct::updateClipStartTime(CloudsClip clip, float startTime,float handleLength, string topic){
    for(int i =0; i<actItems.size(); i++){
        if(actItems[i].type == Clip && actItems[i].key == clip.getLinkName()){
            
            cout<<"updating clip start time for clip : "<<clip.getLinkName()<<" to account for VS gap"<<endl;
            actItems[i].startTime = startTime;
            //defaulting handle length to 1
            actItems[i].endTime = startTime+clip.getDuration() + 1;
        }
    
    }
}

float CloudsAct::addVisualSystem(CloudsVisualSystemPreset& preset, float startTime, float duration){
    visualSystemIndeces[preset.getID()] = visualSystems.size();
    visualSystems.push_back(preset);
    
    //cout<<"added " <<preset.getID()<< " to VS map "<<endl;
    
	ActTimeItem item;
    // start the visual system halfway through the clip
    float vsStartTime = startTime;
    item.type = VS;
    item.key = preset.getID() ;
    item.startTime = vsStartTime;
    item.endTime = vsStartTime + duration;
    item.introEndTime = vsStartTime + preset.introDuration;
    item.outroStartTime = item.endTime -preset.outroDuration;
    
    duration = MAX(item.endTime, duration);
    
    actItems.push_back(item);
    actItemsMap[item.key] = item;
    visualSystemItems[preset.getID()] = item;

	return duration;
}

void CloudsAct::addGapForCadence(CloudsVisualSystemPreset& preset,float startTime,float duration){
    ActTimeItem item;
    item.key = preset.getID();
    item.type = Gap;
    item.startTime = startTime;
    item.endTime = startTime + duration;
    updateVsEndTime(preset, duration);
    actItems.push_back(item);
    
}

void CloudsAct::updateVsEndTime(CloudsVisualSystemPreset& preset, float newEndTime){
    for(int i =0; i<actItems.size(); i++){
        if(actItems[i].type == VS && actItems[i].key == preset.getID()){
			actItems[i].outroStartTime += newEndTime;
			actItems[i].endTime += newEndTime;
        }
    }
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

void CloudsAct::addQuestion(CloudsClip clip, string topic, float startTime){
    ActTimeItem item;
    item.type = Question;
    
    item.key = clip.getID() + "??" + topic;
    
    item.startTime = startTime;
    //dont care about end time as it will end with visual system;
    item.endTime = startTime + 10;
    
    questionsMap[item.key] = clip;
    actItems.push_back(item);
    
}

vector<CloudsClip>& CloudsAct::getAllClips(){
    return clips;
}

string CloudsAct::getTopicForClip(CloudsClip& clip){
    return topicMap[ clip.getLinkName() ];
}

//
vector<string>& CloudsAct::getAllTopics(){
    return topicHistory;
}

void CloudsAct::setTopicForClip(string topic, string clipName){
    topicMap[clipName] = topic;
}

void CloudsAct::clear(){
    clips.clear();
    topicHistory.clear();
    clipMap.clear();
    visualSystemIndeces.clear();
	visualSystems.clear();
    topicMap.clear();
    actItems.clear();
    timeline.reset();
	topicDurationMap.clear();
    duration = 0;
}

CloudsStoryEvents& CloudsAct::getEvents(){
	return events;
}