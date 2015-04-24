//
//  CloudsAct.cpp
//  CloudsStoryEngine
//
//  Created by Surya Mattu on 04/07/13.
//
//

#include "CloudsAct.h"
#include "CloudsAudioEvents.h"

bool delta_sort(pair<string,float> a, pair<string,float> b){
	return a.second > b.second;
}

CloudsAct::CloudsAct(){
    
	timelinePopulated = false;
    duration = 0;
	defaulPrerollDuration = 2.0;
    defaultAudioFade = 2.0;
	incrementalQuesitonTime = 0.0;
    paused = false;
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
	ofNotifyEvent(GetCloudsAudioEvents()->fadeAudioUp, defaultAudioFade);
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
	
    clipsTrack = timeline.addFlags("Clips");
	notesTrack = timeline.addFlags("Notes");
	
    difficultyTrack = timeline.addFlags("Clip Difficulty");
    clipPreRollTrack = timeline.addFlags("Clip PreRoll Flags");
    questionsTrack = timeline.addFlags("Questions");
    
    timeline.setInPointAtSeconds(0);
    string previousTopic = "";
    string currentTopic = "";
    float currentTopicStartTime = 0;
    for(int i=0; i < actItems.size(); i++){
        ActTimeItem& item = actItems[i];
        float itemStartTime = MAX(0, item.startTime); //make sure it's positive for this part
		float itemEndTime = MAX(0, item.endTime); //make sure it's positive for this part
        
		if(item.type == Clip){
            currentTopic = topicMap[item.key];
            
			//cout << "***ADDING CLIP WITH TIME " << itemStartTime << endl;
            clipsTrack->addFlagAtTime(item.key, itemStartTime * 1000);
            clipsTrack->addFlagAtTime(" ", item.endTime * 1000);
            
            difficultyTrack->addFlagAtTime(clipDifficultyMap[item.key], itemStartTime * 1000);
            if(currentTopic != previousTopic){
                topicsTrack->addFlagAtTime(currentTopic, itemStartTime * 1000);
				if(previousTopic != ""){
					topicDurationMap[previousTopic] =  itemStartTime - currentTopicStartTime;
				}
				topicHistory.push_back(previousTopic);
				currentTopicStartTime = itemStartTime;
            }
            
            previousTopic = currentTopic;
        }
        else if(item.type == VS){
            if(item.startTime != item.introEndTime){
                visualSystemsTrack->addFlagAtTime("start :" + item.key, itemStartTime * 1000);
                visualSystemsTrack->addFlagAtTime("intro :" + item.key, item.introEndTime * 1000);
            }
            else{
                visualSystemsTrack->addFlagAtTime("start :" + item.key, itemStartTime * 1000);
            }
            
            if(item.endTime != duration && item.endTime != item.outroStartTime){
                visualSystemsTrack->addFlagAtTime("outro :" + item.key, item.outroStartTime * 1000);
                visualSystemsTrack->addFlagAtTime("end :" + item.key, item.endTime * 1000);
            }
        }
        else if(item.type == PreRoll){
            clipPreRollTrack->addFlagAtTime(item.key, itemStartTime * 1000);
        }
        else if(item.type == Question){
            questionsTrack->addFlagAtTime(item.key, itemStartTime*1000);
        }
    }
	
	for(int i = 0; i < notes.size(); i++){
		notesTrack->addFlagAtTime(notes[i].first,notes[i].second*1000);
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
	cues.clear();
	CloudsSoundCue introCue;
	
    //TODO Only intro cue on the first act...?
	CloudsClip* startClip = clips[0];
	if(startClip->hasStartingQuestion() && startClip->getTopicsWithQuestions().size() > 0){
		string startTopic    = startClip->getTopicsWithQuestions()[0];
		string startQuestion = startClip->getQuestionForTopic(startTopic);
		introCue.soundQuestionKey = startTopic + ":" + startQuestion;
	}
    
	int introCueEndIndex = clips.size() > 2 ? 2 : 1;
	introCue.mixLevel = 2;
	introCue.dichotomies = dichotomiesMap[startClip->getLinkName()];
	//if the first system has sound...
    if(visualSystems.size() > 0 &&
       visualSystems[0].hasSound() &&
       visualSystemItems[visualSystems[0].getID()].startTime == 0)
    {
        introCue.startTime = visualSystemItems[ visualSystems[0].getID() ].endTime;
        if(visualSystems.size() > 1){
			introCue.duration = visualSystemItems[ visualSystems[0].getID() ].startTime - introCue.startTime;
            cout << "systemend time is [" << visualSystemItems[visualSystems[0].getID()].startTime
				 << " - " << visualSystemItems[visualSystems[0].getID()].endTime << endl;
        }
        else {
            introCue.duration = duration - introCue.startTime;
        }
		//skip the intro cue if it overlaps a sound system
    }
    else {
        introCue.startTime = clipItems[startClip->getLinkName()].startTime;
    }

	
	//calculate the 2 largest delta shifts;
	vector< pair<string,float> > clipDeltas;
	map<string, vector<CloudsDichotomy> >::iterator it;
	vector<float> lastValues;
	lastValues.resize(dichotomiesBase.size());
	cout << "INTRO CUE ENDS AT INDEX " << introCueEndIndex << endl;
	bool firstLoop = true;
	for(int i = introCueEndIndex; i < clips.size()-2; i++){
		
		string clipID = clips[i]->getLinkName();
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
//				if(change > 1){
					deltaChange += change;
//				}
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
	cout << "CALCULATED CLIP DELTAS: " << endl;
	for(int i = 0; i < clipDeltas.size(); i++){
		cout << "	clip Delta is " << clipDeltas[i].first << " d: " << clipDeltas[i].second << endl;
	}
	
	//MOOD CUES
	ofxTLFlags* sections = timeline.addFlags("Sections");
	if(clipDeltas.size() >= 2){
		float firstFlagTime = clipItems[clipDeltas[0].first].startTime;
		sections->addFlagAtTime(ofToString(clipDeltas[0].second), firstFlagTime*1000.);
		energyShiftClipIDs.push_back(clipDeltas[0].first);
		
		float secondFlagTime = clipItems[clipDeltas[1].first].startTime;
		
		//if the flag are too close jump
		bool addEnergyShift = true;
		if(abs(firstFlagTime-secondFlagTime) < 60){
			if(clipDeltas.size() > 2 && abs(firstFlagTime-clipItems[clipDeltas[2].first].startTime) > 60 ){
				cout << "ENERGY SHIFT: skipping to third flag " << clipDeltas[2].first << " at time " << clipItems[clipDeltas[2].first].startTime << endl;
				energyShiftClipIDs.push_back(clipDeltas[2].first);
				secondFlagTime = clipItems[clipDeltas[2].first].startTime;
				sections->addFlagAtTime(ofToString(clipDeltas[2].second), secondFlagTime*1000.);
			}
			else{
				//don't add the scrunched up sound, let the existing one just play out
				addEnergyShift = false;
			}
		}
		else{
			cout << "ENERGY SHIFT: second flag " << clipDeltas[1].first << " at time " << clipItems[clipDeltas[1].first].startTime << endl;
			energyShiftClipIDs.push_back(clipDeltas[1].first);
			sections->addFlagAtTime(ofToString(clipDeltas[1].second), secondFlagTime*1000.);
		}
		
		//Add 2 cues, act begun and mood shift
		//End of intro cue
//		CloudsSoundCue actCue;
//		actCue.mixLevel = 1;
//		actCue.startTime = clipItems[clips[introCueEndIndex].getLinkName()].startTime;
//		actCue.duration = clipItems[ energyShiftClipIDs[0] ].startTime - actCue.startTime;
//		actCue.dichotomies = dichotomiesMap[ clips[introCueEndIndex].getLinkName() ];
//		cues.push_back(actCue);
//		
//		CloudsSoundCue energyShift;
//		energyShift.mixLevel = 1;
//		energyShift.startTime = clipItems[ energyShiftClipIDs[0] ].startTime;
//		energyShift.duration = duration - energyShift.startTime;
//		energyShift.dichotomies = dichotomiesMap[startClip.getLinkName()];
//		cues.push_back(energyShift);
		
		
		introCue.duration = clipItems[ energyShiftClipIDs[0] ].startTime - introCue.startTime;
		cues.push_back(introCue);

		CloudsSoundCue actCue;
		actCue.mixLevel = 1;
		actCue.startTime = clipItems[ energyShiftClipIDs[0] ].startTime;
		if(addEnergyShift){
			actCue.duration  = clipItems[ energyShiftClipIDs[1] ].startTime - actCue.startTime;
		}
		else{
			actCue.duration = duration - actCue.startTime;
		}
		actCue.dichotomies = dichotomiesMap[ energyShiftClipIDs[0] ];
		cout << "DICHOTOMIES CONTAINED FOR ACT? " << (dichotomiesMap.find(energyShiftClipIDs[0]) != dichotomiesMap.end() ? "YES" : "NO") << endl;
		cues.push_back(actCue);
		
		if(addEnergyShift){
			CloudsSoundCue energyShift;
			energyShift.mixLevel = 1;
			energyShift.startTime = clipItems[ energyShiftClipIDs[1] ].startTime;
			energyShift.duration = duration - energyShift.startTime;
			energyShift.dichotomies = dichotomiesMap[ energyShiftClipIDs[1] ];
			cout << "DICHOTOMIES CONTAINED FOR ENERGY SHIFT? " << (dichotomiesMap.find(energyShiftClipIDs[1]) != dichotomiesMap.end() ? "YES" : "NO") << endl;
			cues.push_back(energyShift);
		}
	}
	else {
		//change at the one delta we calculated
		if(clipDeltas.size() > 0){
			introCue.duration = duration - clipItems[ clipDeltas[0].first ].startTime;
			cues.push_back(introCue);

			CloudsSoundCue energyShift;
			energyShift.mixLevel = 1;
			energyShift.startTime = clipItems[ clipDeltas[0].first ].startTime;
			energyShift.duration = duration - energyShift.startTime;
			energyShift.dichotomies = dichotomiesMap[ clipDeltas[0].first ];
			cues.push_back(energyShift);
			cout << "DICHOTOMIES CONTAINED FOR ENERGY SHIFT? " << (dichotomiesMap.find(clipDeltas[0].first) != dichotomiesMap.end() ? "YES" : "NO") << endl;
		}
		//or we have no deltas so just play the intor
		else{
			introCue.duration = duration - introCue.startTime;
			cues.push_back(introCue);
		}
		ofLogError("CloudsAct::populateTime") << "Not enough clips to create section markers";
	}
	
    silenceTrack = timeline.addFlags("Silence");
    //adjust cues to fit with sound gaps
    for(int i = 0; i < silenceCues.size(); i++){
        silenceTrack->addFlagAtTime("fade_down", silenceCues[i].min*1000);
        silenceTrack->addFlagAtTime("fade_up", silenceCues[i].max*1000);
    }
    
	timeline.setCurrentPage(0);
}


void CloudsAct::pause(){
    if(!paused){
        paused = true;
        timeline.stop();
    }
}

void CloudsAct::unpause(){
    if(paused){
        paused = false;
        timeline.play();
    }
}

bool CloudsAct::startsWithVisualSystem(){
    return visualSystems.size() > 0 && visualSystemItems[visualSystems[0].getID() ].startTime == 0;
}

bool CloudsAct::isClipEnergyShift(CloudsClip* clip){
	return ofContains(energyShiftClipIDs, clip->getLinkName());
}

void CloudsAct::timelineEventFired(ofxTLBangEventArgs& bang){
    if(bang.track == clipsTrack){
        if(bang.flag !=" "){
            cout << "CloudsAct::timelineEventFired SENDING CLIP BEGAN" << endl;
            CloudsClipEventArgs args(clipMap[bang.flag], topicMap[bang.flag],getDichotomiesForClip(bang.flag));
            ofNotifyEvent(events.clipBegan, args);
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
		CloudsClip* questionClip = questionsMap[bang.flag];
		string topic = ofSplitString( bang.flag, "??")[1];
		string question = questionClip->getQuestionForTopic(topic);
//		cout << "** story engine :: creating question " << question << " with topic " << topic << endl;
        CloudsQuestionEventArgs args(questionClip, question, topic);
        ofNotifyEvent(events.questionProposed, args);
    }
    else if(bang.track == clipPreRollTrack){
        vector<string> clipName = ofSplitString(bang.flag, "%");
        cout<<"sending clip: "<<clipName[1] <<" with Offset: "<< actItemsMap[bang.flag].handleLength << endl;
        CloudsPreRollEventArgs args(clipMap[clipName[1]], actItemsMap[bang.flag].handleLength);
        ofNotifyEvent(events.preRollRequested, args);
    }
	else if(bang.track == topicsTrack){
//        cout<<"sending topic: "<<bang.flag <<" with duration: "<< topicDurationMap[bang.flag] << endl;
		CloudsTopicEventArgs args(bang.flag, topicDurationMap[bang.flag] );
		ofNotifyEvent(events.topicChanged, args);
	}
    else if(bang.track == silenceTrack){
        if(bang.flag == "fade_down"){
            ofNotifyEvent(GetCloudsAudioEvents()->fadeAudioDown, defaultAudioFade);
        }
        else if(bang.flag == "fade_up"){
            ofNotifyEvent(GetCloudsAudioEvents()->fadeAudioUp, defaultAudioFade);
        }
    }
}

void CloudsAct::terminateAct(){
    CloudsActEventArgs args(this);
    ofNotifyEvent(events.actEnded, args);
}

void CloudsAct::timelineStopped(ofxTLPlaybackEventArgs& event){
    if(!paused){
        CloudsActEventArgs args(this);
        ofNotifyEvent(events.actEnded, args);
    }
}

float CloudsAct::getActDuration(){
    return duration;
}

vector<CloudsSoundCue>& CloudsAct::getSoundCues(){
	return cues;
}

vector<CloudsDichotomy>& CloudsAct::getDichotomiesForClip(CloudsClip* clip){
	return getDichotomiesForClip(clip->getLinkName());
}

vector<CloudsDichotomy>& CloudsAct::getDichotomiesForClip(string clipName){
    if(dichotomiesMap.find(clipName) != dichotomiesMap.end()){
        return dichotomiesMap[clipName];
    }
    return dummyDichotomies;
}

vector<CloudsVisualSystemPreset>& CloudsAct::getAllVisualSystemPresets(){
    return visualSystems;
}

void CloudsAct::drawDebug(){
    timeline.setOffset(ofVec2f(0,ofGetHeight()*.5));
    timeline.draw();
}

CloudsClip* CloudsAct::getClip(int index){
    return clips[index];
}

CloudsClip* CloudsAct::getClipAtTime(float time){
    for(int i=0; i< clips.size(); i++){
        ActTimeItem item = getItemForClip(clips[i]);
        if(time >= item.startTime && time <= item.endTime){
            return clips[i];
        }
    }
    ofLogError("CloudsAct::getClipAtTime") << "No clip found at time: " << time;
    return &dummyClip;
}

CloudsVisualSystemPreset& CloudsAct::getVisualSystemInAct(int index){
    return visualSystems[index];
}

float CloudsAct::getClipStartTime(CloudsClip* clip){
	return getItemForClip(clip).startTime;
}

float CloudsAct::getClipEndTime(CloudsClip* clip){
	return getItemForClip(clip).endTime;
}

ActTimeItem& CloudsAct::getItemForClip(CloudsClip* clip){
    if(clipMap.find(clip->getLinkName()) == clipMap.end()){
        ofLogError() << "Couldn't find Act Item for cilp " << clip->getLinkName();
        return dummy;
    }
    return clipItems[clip->getLinkName()];
}

ActTimeItem& CloudsAct::getItemForVisualSystem(CloudsVisualSystemPreset& preset){
    if(visualSystemItems.find(preset.getID()) == visualSystemItems.end()){
        ofLogError()<<"Can't find Act item for Visual System"<<endl;
    }
    return visualSystemItems[preset.getID()];
}

float CloudsAct::addClip(CloudsClip* clip, string topic, float startTime){
	return addClip(clip, topic, startTime, CloudsDichotomy::getDichotomies());
}

float CloudsAct::addClip(CloudsClip* clip, string topic, float startTime, vector<CloudsDichotomy> currentDichotomiesBalance){
    clips.push_back(clip);
    clipMap[clip->getLinkName()] = clip;
    topicMap[clip->getLinkName()] = topic;

    string clipDifficulty;
    if (clip->hasSpecialKeyword("easy")) {
        clipDifficulty = "easy";
    }
    else if(clip->hasSpecialKeyword("hard")){
        clipDifficulty = "hard";
    }
    else{
        clipDifficulty = "medium";
    }
	
	//currently set to 1.0 which is the handle length on all of our clips
	float clipOffsetTime = 1.0;
    ActTimeItem item;
    item.type = Clip;
    item.key = clip->getLinkName();
    item.startTime = startTime + clipOffsetTime;
    item.endTime = startTime + clip->getDuration() + clipOffsetTime;
    duration = MAX(item.endTime, duration);
    
    actItems.push_back(item);
    actItemsMap[item.key] = item;
    dichotomiesMap[item.key] = currentDichotomiesBalance;
    clipItems[clip->getLinkName()] = item;
    clipDifficultyMap[clip->getLinkName()] = clipDifficulty;
	
	//Preroll the clip
    ActTimeItem prerollItem;
    prerollItem.type = PreRoll;
    prerollItem.key = "%" + clip->getLinkName();
    prerollItem.startTime = MAX(0, item.startTime - defaulPrerollDuration);
    prerollItem.endTime = prerollItem.startTime;
    prerollItem.handleLength = clipOffsetTime;
    
    actItemsMap[prerollItem.key] = prerollItem;
    actItems.push_back(prerollItem);
	
	incrementalQuesitonTime = 0;

	return duration;
}

float CloudsAct::addVisualSystem(CloudsVisualSystemPreset& preset, float startTime, float endTime){
	
    visualSystemIndeces[preset.getID()] = visualSystems.size();
    visualSystems.push_back(preset);
    
	float outroLeadTime = 1.0;
	float introLeadTime = 1.0;

	ActTimeItem item;
    item.type = VS;
    item.key = preset.getID();
    item.startTime = startTime;
    item.endTime = endTime;
	item.introEndTime = startTime + introLeadTime;
    item.outroStartTime = item.endTime - outroLeadTime;
    
    duration = MAX(item.endTime, duration);
    
    //debugging time respect
    if(preset.indefinite){
        addNote("INDEFINITE SCHED: " + ofToString(endTime - startTime, 3) , startTime);
    }
    else{
        addNote("DEFINITE DUR: " + ofToString(preset.duration,3) + " SCHED: " + ofToString(endTime - startTime, 3), startTime);
        if( preset.duration + .3 < endTime-startTime){
            ofLogError("CloudsAct::addVisualSystem") << "Scheduled definite visual system for too long";
        }
    }

    actItems.push_back(item);
    actItemsMap[item.key] = item;
    visualSystemItems[preset.getID()] = item;
    
    if(preset.hasSound()){
        silenceCues.push_back(ofRange(item.startTime, item.endTime));
    }
    
	return duration;
}

void CloudsAct::addQuestion(CloudsClip* clip, string topic, float startTime){
    ActTimeItem item;
    item.type = Question;
    
    item.key = clip->getID() + "??" + topic;
    
    item.startTime = startTime + 5 + incrementalQuesitonTime;;
    //dont care about end time as it will end with visual system;
    item.endTime = startTime;
    incrementalQuesitonTime += 1;
	
    questionClips.push_back( make_pair(clip, topic));
    
    questionsMap[item.key] = clip;
    actItems.push_back(item);
}

void CloudsAct::addNote(string note, float time){
	notes.push_back(make_pair(note,time));
}

void CloudsAct::addSilenceRange(ofRange range){
    silenceCues.push_back(range);
}

vector<CloudsClip*>& CloudsAct::getAllClips(){
    return clips;
}

vector< pair<CloudsClip*, string> >& CloudsAct::getAllQuestions(){
    return questionClips;
}

string CloudsAct::getTopicForClip(CloudsClip* clip){
    return topicMap[ clip->getLinkName() ];
}

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