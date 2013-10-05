//
//  CloudsStoryEngine.cpp
//  CloudsFCPParser
//
//  Created by James George on 3/17/13.
//
//

#include "CloudsStoryEngine.h"
#include "CloudsAct.h"

bool logsort(pair<float,string> a, pair<float,string> b ){
    return a.first > b.first;
}

CloudsStoryEngine::CloudsStoryEngine(){
	parser = NULL;
	visualSystems = NULL;
    
	isSetup = false;
	printDecisions = true;
	combinedClipsOnly = false;
	printCriticalDecisions = false;
    
    //	fixedClipDelay = 5;
	maxTimesOnTopic = 4;
    
    systemMaxRunTime = 60*2;
    maxVisualSystemGapTime = 60*3;
    longClipThreshold = 30;
    longClipFadeInPercent = .5;
    actLength = 10 * 60;
    
	lastClipSharesTopicBoost = 10;
	twoClipsAgoSharesTopicBoost = 10;
    
	topicRelevancyMultiplier = 100;
    topicsInCommonMultiplier = 10;
    topicsinCommonWithPreviousMultiplier = 5;
    samePersonOccurrenceSuppressionFactor = 4;
    dichotomyWeight = 2;
    linkFactor = 20;
    maxTimeWithoutQuestion =120;
    gapLengthMultiplier = 0.5;
    preRollDuration = 5;
    minClipDurationForStartingOffset = 30;
    offTopicFactor = 0;
	distantClipSuppressionFactor = 0;
	
	genderBalanceFactor = 5;
    goldClipFactor = 50;
	easyClipScoreFactor = 60;
    
	visualSystemPrimaryTopicBoost = 10;
	visualSystemSecondaryTopicBoost = 5;
    
    
    
}

CloudsStoryEngine::~CloudsStoryEngine(){
    delete gui;
    delete clipGui;
}

void CloudsStoryEngine::setup(){
	if(!isSetup){
		isSetup = true;
        
		initGui();
		
        CloudsDichotomy d;
        dichotomyThreshold = 3;
        
		dichotomies.clear();
		
        d.left = "#art";
        d.right = "#tech";
        d.balance =0;
        dichotomies.push_back(d);
        
        d.left = "#emotional";
        d.right = "#logical";
        d.balance =0;
        dichotomies.push_back(d);
        
        d.left = "#breakthrough";
        d.right = "#obstacle";
        d.balance =0;
        dichotomies.push_back(d);
        
        d.left = "#inspiring";
        d.right = "#discouraging";
        d.balance =0;
        dichotomies.push_back(d);
        
        d.left = "#fun";
        d.right = "#serious";
        d.balance =0;
        dichotomies.push_back(d);
        
        d.left = "#sincere";
        d.right = "#ironic";
        d.balance =0;
        dichotomies.push_back(d);
        
        d.left = "#mindblowing";
        d.right = "#mundane";
        d.balance =0;
        dichotomies.push_back(d);
        
        d.left = "#rational";
        d.right = "#surreal";
        d.balance =0;
        dichotomies.push_back(d);
		
	}
}

void CloudsStoryEngine::initGui(){
	
	runGui = new ofxUISuperCanvas("RUN INFORMATION",OFX_UI_FONT_SMALL);
    runGui->addSpacer();
    runGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    actGui = new ofxUISuperCanvas("ACT PARAMS", OFX_UI_FONT_SMALL);
	
	vector<CloudsClip> startingNodes = parser->getClipsWithKeyword("#start");
	vector<string> questions;
	for(int i = 0; i < startingNodes.size(); i++){
		CloudsClip& clip = startingNodes[i];
		if(!clip.hasQuestion()){
			questions.push_back("#START CLIP " + ofToUpper(clip.getLinkName()) + " HAS NO QUESTION **");
		}
		else{
			map<string,string> clipQuestions = clip.getAllQuestionTopicPairs();
			questions.push_back( clip.getLinkName() + ", " + clipQuestions.begin()->first + ", " + clipQuestions.begin()->second );
		}
	}
    
	actGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	ofxUIDropDownList* ddQuestions = actGui->addDropDownList("STARTING QUESTIONS", questions, 700);
	ddQuestions->setAutoClose(true);
	ddQuestions->setShowCurrentSelected(true);
	ddQuestions->setAllowMultiple(false);
	actGui->autoSizeToFitWidgets();
	
    clipGui = new ofxUISuperCanvas("CLIP SCORE PARAMS", OFX_UI_FONT_SMALL);
    clipGui->setPosition(0,0);
    clipGui->addSpacer();
	clipGui->addSlider("MAX TIMES ON TOPIC", 2, 7, &maxTimesOnTopic);
    clipGui->addSlider("TOPICS IN COMMON CURRENT MULTIPLIER", 0, 50, &topicsInCommonMultiplier);
    clipGui->addSlider("TOPICS IN COMMON PREVIOUS MULTIPLIER", 0, 10, &topicsinCommonWithPreviousMultiplier);
    clipGui->addSlider("OFF TOPIC FACTOR", 0, 20, &offTopicFactor);
    clipGui->addSlider("SAME PERSON SUPPRESSION FACTOR", 0, 10, &samePersonOccurrenceSuppressionFactor);
    clipGui->addSlider("LINK FACTOR",0,50, &linkFactor);
    clipGui->addSlider("DICHOTOMY WEIGHT", 0,10, &dichotomyWeight);
    clipGui->addSlider("GENDER BALANCE", 0, 10, &genderBalanceFactor);
	clipGui->addSlider("DISTANT CLIP SUPRRESSION", 0, 100, &distantClipSuppressionFactor);
    clipGui->addSlider("GOLD CLIP FACTOR", 10, 100, &goldClipFactor);
    clipGui->addSlider("EASY CLIP FACTOR", 10, 100, &easyClipScoreFactor);
	
	clipGui->autoSizeToFitWidgets();
    
	topicGui = new ofxUISuperCanvas("TOPIC SCORE PARAMS", OFX_UI_FONT_SMALL);
	topicGui->addSpacer();
    topicGui->addSlider("RELEVANCY MULTIPLIER", 0, 200, &topicRelevancyMultiplier);
	topicGui->addSlider("LAST CLIP SHARES TOPIC BOOST", 0, 20, &lastClipSharesTopicBoost);
	topicGui->addSlider("TWO CLIPS AGO SHARES TOPIC BOOST",0, 20, &twoClipsAgoSharesTopicBoost);
    topicGui->autoSizeToFitWidgets();
	
    gui = new ofxUISuperCanvas("TIMING PARAMTERS", OFX_UI_FONT_SMALL);
    gui->setPosition(clipGui->getRect()->x + 100, clipGui->getRect()->y + 10);
    gui->addSpacer();
    gui->addSlider("GAP LENGTH MULTIPLIER", 0.01, 0.1, &gapLengthMultiplier);
    gui->addSlider("ACT LENGTH", 60, 1200,&actLength);
    gui->addSlider("PREROLL FLAG TIME", 1, 10, &preRollDuration);
    gui->addSlider("MIN CLIP DURATION FOR START OFFSET", 0, 100, &minClipDurationForStartingOffset);
    gui->autoSizeToFitWidgets();
    
    vsGui = new ofxUISuperCanvas("VISUAL SYSTEM PARAMS", OFX_UI_FONT_SMALL);
    vsGui->setPosition(gui->getRect()->x + 100, gui->getRect()->y + 10);
    vsGui->addSpacer();
	vsGui->addSlider("PRIMARY TOPIC BOOST", 0, 20, &visualSystemPrimaryTopicBoost);
	vsGui->addSlider("SECONDARY TOPIC BOOST", 0, 20, &visualSystemSecondaryTopicBoost);
    vsGui->addSlider("MAX VS RUNTIME", 0, 480,&systemMaxRunTime);
    vsGui->addSlider("MAX VS GAPTIME", 0, 60, &maxVisualSystemGapTime);
    vsGui->addSlider("LONG CLIP THRESHOLD", 0, 100,&longClipThreshold);
    vsGui->addSlider("LONG CLIP FAD IN %", 0.0, 1.0, &longClipFadeInPercent);
    vsGui->autoSizeToFitWidgets();
    
    string filePath;
    filePath = getDataPath() +"storyEngineParameters/gui.xml";
    if(ofFile::doesFileExist(filePath))gui->loadSettings(filePath);
    
    filePath = getDataPath() +"storyEngineParameters/clipGui.xml";
    if(ofFile::doesFileExist(filePath))clipGui->loadSettings(filePath);
    
    filePath = getDataPath() +"storyEngineParameters/vsGui.xml";
    if(ofFile::doesFileExist(filePath))vsGui->loadSettings(filePath);
    
	filePath = getDataPath() +"storyEngineParameters/topicGui.xml";
    if(ofFile::doesFileExist(filePath))topicGui->loadSettings(filePath);
    
	ofAddListener(actGui->newGUIEvent, this, &CloudsStoryEngine::guiEvent);
    ofAddListener(gui->newGUIEvent, this, &CloudsStoryEngine::guiEvent);
    ofAddListener(clipGui->newGUIEvent, this, &CloudsStoryEngine::guiEvent);
    ofAddListener(vsGui->newGUIEvent, this, &CloudsStoryEngine::guiEvent);
    ofAddListener(topicGui->newGUIEvent, this, &CloudsStoryEngine::guiEvent);
	
	positionGuis();
	toggleGuis();
	
}

void CloudsStoryEngine::positionGuis(){
	clipGui->setPosition(0,0);
	topicGui->setPosition(clipGui->getRect()->getMaxX(), clipGui->getRect()->y);
	gui->setPosition(topicGui->getRect()->getMaxX(), topicGui->getRect()->y);
	vsGui->setPosition(gui->getRect()->getMaxX(), gui->getRect()->y);
	actGui->setPosition(0,clipGui->getRect()->getMaxY());
    runGui->setPosition(vsGui->getRect()->getMaxX(),0);
}

void CloudsStoryEngine::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    if(name == "STARTING QUESTIONS"){
	    ofxUIDropDownList* b = (ofxUIDropDownList*) e.widget;
		if(! b->isOpen() && b->getSelectedIndeces().size() > 0){
            
			CloudsClip clip = parser->getClipsWithKeyword("#start")[ b->getSelectedIndeces()[0] ];
			
			cout << "Selected index  is " << b->getSelectedIndeces()[0] << endl;
			
			string topic = clip.getAllQuestionTopicPairs().begin()->first;
			cout << "SELECTED CLIP ** " << clip.getLinkName() << " WITH TOPIC " << topic << endl;
            
			buildAct(runTest, clip, topic);
            updateRunData();
		}
    }
}

void CloudsStoryEngine::updateRunData(){
    
    
    if(runTest.timesOnCurrentTopicHistory.size()>0){
        runGui->removeWidgets();
        
        map<string, int>::iterator it;
        runTopicCount.clear();
        for(it =runTest.timesOnCurrentTopicHistory.begin(); it != runTest.timesOnCurrentTopicHistory.end(); it++){
            string topicString = it->first + " : " + ofToString(it->second);
            runTopicCount.push_back(topicString);
        }
        runGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
        ofxUIDropDownList* topicCount   = runGui->addDropDownList("RUN TOPIC COUNT", runTopicCount);
        
        topicCount->setAutoClose(true);
        topicCount->setShowCurrentSelected(true);
        topicCount->setAllowMultiple(false);
        //        runGui->autoSizeToFitWidgets();
    }
    
    //    runGui->
}
void CloudsStoryEngine::saveGuiSettings(){
    gui->saveSettings(getDataPath() +"storyEngineParameters/gui.xml");
    clipGui->saveSettings(getDataPath() +"storyEngineParameters/clipGui.xml");
    vsGui->saveSettings(getDataPath() +"storyEngineParameters/vsGui.xml");
	topicGui->saveSettings(getDataPath() +"storyEngineParameters/topicGui.xml");
    runGui->saveSettings(getDataPath() +"storyEngineParameters/runGui.xml");
}

void CloudsStoryEngine::toggleGuis(){
	actGui->toggleVisible();
	clipGui->toggleVisible();
	topicGui->toggleVisible();
	gui->toggleVisible();
	vsGui->toggleVisible();
    runGui->toggleVisible();
}

#pragma mark INIT ACT
CloudsAct* CloudsStoryEngine::buildAct(CloudsRun run, CloudsClip& seed){
    return buildAct(run, seed, seed.getKeywords()[ ofRandom(seed.getKeywords().size()) ]);
}

CloudsAct* CloudsStoryEngine::buildAct(CloudsRun run, CloudsClip& seed, string topic){
    CloudsAct* act = new CloudsAct();
    float seconds = actLength;
    float totalSecondsEnqueued = 0;
    bool freeTopic = false;
    bool deadEnd = false;
    
    //the run now listens to act events and is updated thorugh them.
    vector<CloudsClip> localClipHistory = run.clipHistory;
    vector<string> localPresetHistory = run.presetHistory;
	vector<string> localTopicHistory = run.topicHistory;
    map<string, int> localTimesOnCurrentTopicHistory = run.timesOnCurrentTopicHistory;
	
    //	cout << "*** RUN DEBUG " << clipHistory.size() << " and size of presets " << presetHistory.size() << endl;
    //VS Stuff
    float lastVisualSystemEnded = 0;
    bool systemRunning = false;
    float visualSystemStartTime =0;
    string previousTopic = " ";
    float visualSystemDuration  = 0;
    float maxTimeRemainingForVisualSystem =0;
    bool isPresetIndefinite = false;
    float definitePresetEndTime = 0;
    
	//logging
    stringstream clipScoreStream;
    stringstream topicScoreStream;
    stringstream vsScoreStream;
    
	clipScoreStream<<"SAME PERSON SUPPRESSION FACTOR,LINK FACTOR,DICHOTOMY WEIGHT,GENDER BALANCE,LAST CLIP COMMONALITY MULTIPLIER,TWO CLIPS COMMONALITY MULTIPLIER,GOLD CLIP FACTOR, EASY CLIP FACTOR"<< endl;
	clipScoreStream<<samePersonOccurrenceSuppressionFactor<<","<<linkFactor<<","<<dichotomyWeight<<","<<genderBalanceFactor<<","<<topicsInCommonMultiplier<<","<<topicsinCommonWithPreviousMultiplier<<","<<goldClipFactor<<","<<easyClipScoreFactor<< endl;
    clipScoreStream<<"Selected Clip,Current Topic, Potential Next Clip,Total Score,linkScore,topicsInCommonScore,topicsInCommonWithPreviousScore,offTopicScore,samePersonOccurrenceScore,dichotomyWeight,genderBalance,distanceClipSuppression,voiceOverScore,goldFactorScore,clipDifficulty"<<endl;
    
	topicScoreStream << "MAX TIMES ON TOPIC,LAST CLIP SHARES TOPIC BOOST,TWO CLIPS AGO SHARES TOPIC BOOST,TOPIC RELEVANCY MULTIPLIER"<< endl;
	topicScoreStream << maxTimesOnTopic <<","<< lastClipSharesTopicBoost <<","<< twoClipsAgoSharesTopicBoost <<","<< topicRelevancyMultiplier<<"," <<endl;
	topicScoreStream << "Current Topic,Potential Topic,Total Score,Last Clip Commonality,Two Clips Ago Commonality,Relevancy Score,Cohesion Score"<<endl;
    
	vsScoreStream << "Topic,Clip,System Preset,Total Score,Main Topic Score,Secondary Topic Score,Keywords Matched"<<endl;
    clearDichotomiesBalance();
	CloudsVisualSystemPreset currentPreset;
    CloudsClip clip = seed;
    
	int moreMenThanWomen = 0;
    int timeForNewQuesiton = 0;
	float preRollFlagTime  = 0;
	float clipHandleDuration = getHandleForClip(clip);
	act->addClipPreRollFlag(preRollFlagTime, clipHandleDuration, clip.getLinkName());
	totalSecondsEnqueued = preRollDuration;
    act->addClip(clip, topic, totalSecondsEnqueued, clipHandleDuration, getCurrentDichotomyBalance());
	localClipHistory.push_back(clip);
	localTopicHistory.push_back(topic);
	
    totalSecondsEnqueued += clip.getDuration()+( gapLengthMultiplier * clip.getDuration() ) * clipHandleDuration*2;
    localTimesOnCurrentTopicHistory[topic]++;
    int timesOnCurrentTopic = 0;
   
    while( totalSecondsEnqueued < seconds ){
        //scoreStream<<clip.getLinkName()<<","<<topic<<","<<" "<<","<<" "<<","<<" "<<","<<" "<<","<<" "<<","<<" "<<","<<" "<<endl;
        clipScoreStream<<clip.getLinkName()<<","<<topic<<endl;
		
        freeTopic |= timesOnCurrentTopic > maxTimesOnTopic;
        
        if(freeTopic){
			string topicLog="";
			topicScoreStream<<topic<<","<<timesOnCurrentTopic<<" times,,,,"<<endl;
            string newTopic = selectTopic(act, clip, localTopicHistory, topic,topicLog);
			topicScoreStream << topicLog;
            if(newTopic == topic){
				//We landed on the same topic, we are totally stuck -- end the act!
                break;
            }
            //switching to a new topic. update the timesOnCurrenTopic history for the current topic before it changes.
            //            timesOnCurrentTopicHistory[topic] += timesOnCurrentTopic;
            //            cout<<"Switching from old topic : "<<topic<<"  occurences: "<<timesOnCurrentTopic<<endl;
            topic = newTopic;
            timesOnCurrentTopic = 1;
            freeTopic = false;
            localTopicHistory.push_back(topic);
        }
        //storing a copy of current topic for each clip
        
		///////////////// 1 GATHER CLIPS
        vector<CloudsClip> nextOptions = parser->getClipsWithKeyword(topic);
        
        //add all manual links
        vector<CloudsLink>& links = parser->getLinksForClip( clip );
        for(int i = 0; i < links.size(); i++){
            
            bool valid = true;
            for(int c = 0; c < nextOptions.size(); c++){
                if(nextOptions[c].getLinkName() == links[i].targetName){
                    //don't add clips that are already included
                    valid = false;
                    break;
                }
            }
            
            if(valid){
                nextOptions.push_back( parser->getClipWithLinkName(links[i].targetName) );
            }
        }
        
        //remove suppressions
        vector<CloudsLink>& suppressions = parser->getSuppressionsForClip( clip );
        for(int i = 0; i < suppressions.size(); i++){
            for(int nt = nextOptions.size()-1; nt >= 0; nt--){
                if(nextOptions[nt].getLinkName() == suppressions[i].targetName){
                    nextOptions.erase(nextOptions.begin() + nt);
                }
            }
        }
        
		/////////////////SELECTION
        vector<pair<float,string> > scoreLogPairs;
        float topScore = 0;
        for(int i = 0; i < nextOptions.size(); i++){
            CloudsClip& nextClipOption = nextOptions[ i ];
            string log = "";
            
            float score = scoreForClip(localClipHistory, nextClipOption, topic,log, systemRunning, isPresetIndefinite, moreMenThanWomen,localTimesOnCurrentTopicHistory[topic]);
            
            scoreLogPairs.push_back( make_pair(score,log) );
            //            totalPoints += score;
            topScore = MAX(topScore, score);
            nextClipOption.currentScore = score;
        }
        
        sort(scoreLogPairs.begin(), scoreLogPairs.end(), logsort);
        
        for (int i = 0; i < scoreLogPairs.size(); i++) {
            clipScoreStream << scoreLogPairs[i].second;
        }
        
        if(topScore == 0){
            //Dead end!  start over with a free topic and see what happens
            cout << "Dead end, no clips left" << endl;
			topicScoreStream<<topic<<" HIT DEAD END"<<endl;
            freeTopic = true;
            continue;
        }
        
        vector<CloudsClip> winningClips;
        for(int i = 0; i < nextOptions.size(); i++){
            if(nextOptions[i].currentScore == topScore){
                winningClips.push_back(nextOptions[i]);
            }
        }
        
        //select next clip
        clip = winningClips[ofRandom(winningClips.size())];
//		cout << "clip " << clip.getLinkName() << " has speaker gender " << clip.getSpeakerGender() << endl;
        if(clip.getSpeakerGender() == "male"){
			moreMenThanWomen++;
		}
		else{
			moreMenThanWomen--;
		}
		
        if (!clip.hasKeyword(topic) ) {
			clipScoreStream << "ERROR " << clip.getLinkName() << " does not have current topic " << topic << ". making free" << endl;
            freeTopic = true;
        }
		
		///////////////// DIOCHOTOMIES
        updateDichotomies(clip);
        
		///////////////// QUESTIONS
        //adding all option clips with questions
        for(int k = 0; k < nextOptions.size(); k++){
            if(nextOptions[k].getQuestionsVector().size() > 0 &&
			   parser->clipLinksTo(clip.getLinkName(), nextOptions[k].getLinkName()))
			{
				act->addQuestion(nextOptions[k], totalSecondsEnqueued);
            }
        }
        
		//add clip to act
		clipHandleDuration = getHandleForClip(clip);
        act->addClip(clip,topic,totalSecondsEnqueued, clipHandleDuration,getCurrentDichotomyBalance());
		float preRollFlagTime  = totalSecondsEnqueued - preRollDuration;
        act->addClipPreRollFlag(preRollFlagTime, clipHandleDuration, clip.getLinkName());

        localClipHistory.push_back(clip);
        
        
        //add clip topic history to run
        vector<string> topics = clip.getKeywords();
        for (int i =0; i < topics.size(); i++) {
            localTimesOnCurrentTopicHistory[topics[i]]++;
        }

		///////////////// VISUAL SYSTEMS
		//check to see if we want to add a visual system
        float clipStartTime = act->getItemForClip(clip).startTime;
        float clipEndTime = act->getItemForClip(clip).endTime;
        if( systemRunning ) {
            visualSystemDuration = clipStartTime - visualSystemStartTime;
            
            if(isPresetIndefinite){
                if(visualSystemDuration > systemMaxRunTime || topic != previousTopic){
                    if(clip.getDuration() > longClipThreshold){
                        visualSystemDuration += clip.getDuration()*longClipFadeInPercent;
                    }
                    
                    act->addVisualSystem(currentPreset, visualSystemStartTime, visualSystemDuration);
                    act->removeQuestionAtTime(visualSystemStartTime, visualSystemDuration);
                    systemRunning = false;
                    lastVisualSystemEnded = visualSystemStartTime + visualSystemDuration;
                }
            }
            else{
                if(visualSystemDuration > definitePresetEndTime ){
                    definitePresetEndTime = 0;
                    act->addVisualSystem(currentPreset, visualSystemStartTime, definitePresetEndTime);
                    act->removeQuestionAtTime(visualSystemStartTime, definitePresetEndTime);
                    systemRunning = false;
                    isPresetIndefinite = true;
                    lastVisualSystemEnded = visualSystemStartTime + definitePresetEndTime;
                }
            }
        }
        else {
            float timeSinceLastVisualSystem = clipEndTime - lastVisualSystemEnded;
            
            //if the clip is shorter than the 30 seconds dont start the VS during the clip.
            if(timeSinceLastVisualSystem > maxVisualSystemGapTime && clip.getDuration() > longClipThreshold ){
                
                visualSystemStartTime = clipStartTime + clip.getDuration() * longClipFadeInPercent;
                maxTimeRemainingForVisualSystem = systemMaxRunTime;
				
                string log;
                currentPreset = getVisualSystemPreset(topic, clip, localPresetHistory, log);
                //The run now listens to act events and is updated via them
                localPresetHistory.push_back(currentPreset.getID());
                
                isPresetIndefinite = currentPreset.indefinite;
                if (!isPresetIndefinite) {
                    definitePresetEndTime = visualSystemStartTime + currentPreset.duration;
                }
				
				vsScoreStream << topic <<","<< clip.getLinkName() <<","<< currentPreset.getID() << endl;
				vsScoreStream << log << endl;
                
                systemRunning = true;
            }
        }
        
        previousTopic = topic;
        totalSecondsEnqueued += clip.getDuration() + ( gapLengthMultiplier * clip.getDuration() ) + clipHandleDuration * 2;
        timesOnCurrentTopic++;

    }
	
    //add the history of the last topic in the act to the timesOnCurrentTopicHistory map of the CloudsRun.
    //    timesOnCurrentTopicHistory[topic] += timesOnCurrentTopic;
    
    
    //TODO: be aware if you have ended on a fixed duration VS to respect its duration
    if(systemRunning){
		
        float clipStartTime = act->getItemForClip(act->getClip(act->getAllClips().size()-1)).startTime;
        float clipEndTime = act->getItemForClip(act->getClip(act->getAllClips().size()-1)).endTime;
        
        if(isPresetIndefinite){
            if(visualSystemDuration > systemMaxRunTime || topic != previousTopic){
                if(clip.getDuration() > longClipThreshold){
                    visualSystemDuration += clip.getDuration()*longClipFadeInPercent;
                }
                
                act->addVisualSystem(currentPreset, visualSystemStartTime, visualSystemDuration);
                act->removeQuestionAtTime(visualSystemStartTime, visualSystemDuration);
                systemRunning = false;
                lastVisualSystemEnded = visualSystemStartTime + visualSystemDuration;
            }
        }
        else{
            if(visualSystemDuration > definitePresetEndTime ){
                definitePresetEndTime = 0;
                act->addVisualSystem(currentPreset, visualSystemStartTime, definitePresetEndTime);
                act->removeQuestionAtTime(visualSystemStartTime, definitePresetEndTime);
                systemRunning = false;
                isPresetIndefinite = true;
                lastVisualSystemEnded = visualSystemStartTime + definitePresetEndTime;
            }
        }
    }
    
    act->populateTime();
	
	ofBuffer scoreBuffer;
    scoreBuffer.set(clipScoreStream);
    ofBufferToFile(getDataPath() + "logs/clipScore.csv", scoreBuffer);
	
	ofBuffer topicBuffer;
	topicBuffer.set(topicScoreStream);
	ofBufferToFile(getDataPath() + "logs/topicScores.csv", topicBuffer);
    
	ofBuffer visualSystemBuffer;
	visualSystemBuffer.set(vsScoreStream);
	ofBufferToFile(getDataPath() + "logs/visualSystemScores.csv", visualSystemBuffer);
    
    run.actCount++;
	
    CloudsActEventArgs args(act);
    ofNotifyEvent(events.actCreated, args);
    
    return act;
}

#pragma mark VISUAL SYSTEMS
CloudsVisualSystemPreset CloudsStoryEngine::getVisualSystemPreset(string keyword, CloudsClip& currentClip, vector<string>& presetHistory, string& log){
    
	bool adjascentTried = false;
	float topScore = 0;
	vector< pair<float, string> > scoreLogPairs;
	vector<CloudsVisualSystemPreset> presets = visualSystems->getPresetsForKeywords( currentClip.getKeywords() );
	CloudsVisualSystemPreset preset;
	while(topScore == 0){
		
		for(int i = 0; i < presets.size(); i++){
			string presetLog;
			presets[i].currentScore = scoreForVisualSystem(presets[i], presetHistory, keyword, currentClip.getKeywords(), presetLog);
			topScore = MAX(presets[i].currentScore, topScore);
			scoreLogPairs.push_back( make_pair(presets[i].currentScore, presetLog) );
		}
		
		if(topScore == 0){
			//we didn't find any visual systems for this clip
			log += ",ERROR,No valid presets clip keywords\n";
			log += ",,"+ofJoinString(currentClip.getKeywords(),"\n,,") + "\n";
			if(adjascentTried){
				break;
			}
			adjascentTried = true;
			vector<string> adjacentTopics = parser->getAdjacentKeywords(keyword, 5);
			if(adjacentTopics.size() == 0){
				log += ",ERROR,No topics adjacent to " + keyword + "\n";
				break;
			}
			//Try the 5 nearby clips to start with
			log += ",searching adjacent keywords:\n";
			log += +",,"+ofJoinString(adjacentTopics, "\n,,") + "\n";
			
			presets = visualSystems->getPresetsForKeywords( currentClip.getKeywords() );
		}
	}
	
	sort(scoreLogPairs.begin(), scoreLogPairs.end(), logsort);
	for(int i = 0; i < scoreLogPairs.size(); i++){
		log += scoreLogPairs[i].second + "\n";
	}
	
	if(topScore != 0){
		vector<CloudsVisualSystemPreset> winningPresets;
		for(int i = 0; i < presets.size(); i++){
			if(presets[i].currentScore == topScore){
				winningPresets.push_back(presets[i]);
			}
		}
		preset = winningPresets[ ofRandom(winningPresets.size()) ];
	}
	else{
		preset = visualSystems->getRandomVisualSystem();
		log += ",ERROR,no presets found! " + preset.getID() + "\n";
	}
	
    return preset;
}

float CloudsStoryEngine::scoreForVisualSystem(CloudsVisualSystemPreset& preset, vector<string>& presetHistory, string currentTopic, vector<string>& seconardyTopics, string& log){
	log += ",,"+preset.getID() + ",";
	if(!preset.enabled){
		log += "rejected because it's disabled";
		return 0;
	}
	
	if(ofContains(presetHistory, preset.getID())){
		log += "rejected because we've seen it before";
		return 0;
	}
	
	vector<string> keywords = visualSystems->keywordsForPreset(preset);
	if(keywords.size() == 0 ){
		log += "rejected because it has no keywords";
		return 0;
	}
	
	float mainTopicScore = 0;
	float secondaryTopicScore = 0;
	vector<string> keywordsMatched;
	for(int i = 0; i < keywords.size(); i++){
		if(keywords[i] == currentTopic){
			mainTopicScore += visualSystemPrimaryTopicBoost;
			keywordsMatched.push_back(keywords[i]);
		}
		else if(ofContains(seconardyTopics, keywords[i])){
			secondaryTopicScore += visualSystemSecondaryTopicBoost;
			keywordsMatched.push_back(keywords[i]);
		}
	}
	float totalScore = mainTopicScore + secondaryTopicScore;
	log += ofToString(totalScore,1) +","+ ofToString(mainTopicScore,1) +","+ ofToString(secondaryTopicScore,1) +","+ ofJoinString(keywordsMatched, "; ");
	return totalScore;
}

void CloudsStoryEngine::clearDichotomiesBalance(){
    for(int i = 0; i < dichotomies.size(); i++){
        dichotomies[i].balance = 0;
    }
}

void CloudsStoryEngine::updateDichotomies(CloudsClip& clip){
    vector<string> specialkeywords= clip.getSpecialKeywords();
    
    for(int i=0; i <dichotomies.size();i++){
        for(int j=0; j<specialkeywords.size();j++){
            
            if(dichotomies[i].left == specialkeywords[j]){
                dichotomies[i].balance -= 1;
                //                cout<<dichotomies[i].left<<": +1"<<endl;
            }
            else if (dichotomies[i].right == specialkeywords[j]){
                //                cout<<dichotomies[i].right<<": +1"<<endl;
                dichotomies[i].balance += 1;
            }
        }
    }
}

//intentionally by copy so that we can store them as we go
vector<CloudsDichotomy> CloudsStoryEngine::getCurrentDichotomyBalance(){
    return dichotomies;
}

#pragma mark TOPIC SCORES
string CloudsStoryEngine::selectTopic(CloudsAct* act, CloudsClip& clip, vector<string>& topicHistory, string topic, string& log){
    
    //vector<string>& topics = clip.getKeywords();
	vector<string>& topics = parser->getKeywordFamily( topic );
	
    vector<float> topicScores;
    topicScores.resize(topics.size());
    float topicHighScore = 0;
    for(int i = 0; i < topics.size(); i++){
        topicScores[i] = scoreForTopic(topicHistory, act->getAllClips(), topic, topics[i], log);
        topicHighScore = MAX(topicHighScore,topicScores[i]);
    }
	
    if(topicHighScore == 0){
        //Dead end!
        cout << "Dead end, no topics left" << endl;
        return topic;
    }
    
    vector<string> winningTopics;
    for(int i = 0; i < topics.size(); i++){
        if(topicScores[i] == topicHighScore){
            winningTopics.push_back(topics[i]);
        }
    }
    
    //    cout << "topic changed from " << topic;
    topic = winningTopics[ ofRandom(winningTopics.size()) ];
    //  cout << " to " << topic << endl;
    
    return topic;
}

float CloudsStoryEngine::scoreForTopic(vector<string>& topicHistory, vector<CloudsClip>& history,
                                       string currentTopic, string newTopic, string& log)
{
    stringstream logstr;
    if(currentTopic == newTopic){
        if(printDecisions) cout << "	REJECTED Topic " << currentTopic << ": same as new topic" << endl;
        return 0;
    }
    
    if(ofContains(topicHistory, newTopic)){
        if(printDecisions) cout << "	REJECTED Topic " << newTopic << ": has already been explored" << endl;
        return 0;
    }
    
    float score = 0;
    
    float lastClipCommonality = 0;
    float twoClipsAgoCommonality = 0;
    if(history.size() > 1 && ofContains( history[history.size()-2].getKeywords(), newTopic) ){
        if(printDecisions) cout << "	LAST CLIP " << history[history.size()-2].getLinkName() << " shares topic " << newTopic << endl;
        lastClipCommonality = lastClipSharesTopicBoost;
    }
    
    if(history.size() > 2 && ofContains( history[history.size()-3].getKeywords(), newTopic) ){
        if(printDecisions) cout << "	TWO CLIPS AGO " << history[history.size()-3].getLinkName() << " shares topic " << newTopic << endl;
        twoClipsAgoCommonality = twoClipsAgoSharesTopicBoost;
    }
    
    int sharedClips = parser->getNumberOfSharedClips(currentTopic,newTopic);
    int totalClips = parser->getNumberOfClipsWithKeyword(newTopic);
    float relevancyScore = (1.0 * sharedClips / totalClips) * topicRelevancyMultiplier;
	
    float distanceBetweenKeywords = parser->getDistanceFromAdjacentKeywords(currentTopic, newTopic);
    float cohesionIndexForKeywords = parser->getCohesionIndexForKeyword(newTopic);
    float cohesionScore = 0; //TODO:Add cohesion score to favor nearby topics
	
    score  = lastClipCommonality +     twoClipsAgoCommonality +      relevancyScore  +   cohesionScore;
    logstr<<","<<newTopic<< "," << score<<",";
	logstr<< lastClipCommonality<<","<<twoClipsAgoCommonality<<",("<< sharedClips << "/" << totalClips << ") = " << relevancyScore<<","<<cohesionScore<<endl;
	log += logstr.str();
	
    //	cout << "	TOPIC " << newTopic << " SCORE " << relevancyScore << " : " << sharedClips << "/" << totalClips << endl;
    return score;
}


#pragma mark CLIP SCORES
float CloudsStoryEngine::scoreForClip(vector<CloudsClip>& history, CloudsClip& potentialNextClip, string topic, string& log, bool visualSystemRunning, bool isPresetIndefinite, int moreMenThanWomen, int timesOnCurrentTopic){
    
    CloudsClip& currentlyPlayingClip = history[history.size()-1];
    
    //rejection criteria -- flat out reject clips on some basis
    if(combinedClipsOnly && !potentialNextClip.hasCombinedVideo){
        if(printDecisions) cout << "	REJECTED Clip " << potentialNextClip.getLinkName() << ": no combined video file" << endl;
        return 0;
    }
    
    //    if(potentialNextClip.cluster.Id == ""){
    //		if(printDecisions) cout << "	REJECTED Clip " << potentialNextClip.getLinkName() << ": disconnect from cluster map =(" << endl;
    //        return 0;
    //    }
    
    bool link = parser->clipLinksTo( currentlyPlayingClip.getLinkName(), potentialNextClip.getLinkName() );
    if(!link && potentialNextClip.person == currentlyPlayingClip.person){
        if(printDecisions) cout << "	REJECTED Clip " << potentialNextClip.getLinkName() << ": same person" << endl;
        return 0;
    }
    
    //reject any nodes we've seen already
    if(historyContainsClip(potentialNextClip, history)){ //TODO discourage...
        if(printDecisions) cout << "	REJECTED Clip " << potentialNextClip.getLinkName() << ": already visited" << endl;
        return 0;
    }
    
	//TODO: make this smarter
    int occurrences = occurrencesOfPerson(potentialNextClip.person, 20, history);
    if(occurrences > 4){
        if(printDecisions) cout << "	REJECTED Clip " << potentialNextClip.getLinkName() << ": person appeared more than 4 times in the last 20 clips" << endl;
        return 0;
    }
    
    //If a VS is not running reject clips that do not have video footage
    if(potentialNextClip.hasSpecialKeyword("#vo") && ! visualSystemRunning){
        return 0;
    }
    
    //If a VS is running and is of a definite duration do not use voice over clips
    if(potentialNextClip.hasSpecialKeyword("#vo") && visualSystemRunning && ! isPresetIndefinite){
        return 0;
    }
    
    // Ignore clip if it has been tagged as a  #dud
    if(potentialNextClip.hasSpecialKeyword("#dud")){
        if(printDecisions) cout<< "           REJECTED Clip "<<potentialNextClip.getLinkName()<< " : this clip has been marked with the #dud keyword"<<endl;
        return 0;
    }
    
    //  can't get a #hard clip until the topic has been heard 2 times by any clip
    if (timesOnCurrentTopic < 2 &&  potentialNextClip.hasSpecialKeyword("#hard")  ) {
        if (printDecisions) cout<< "         REJECTED Clip "<<potentialNextClip.getLinkName()<<" : this clip is labeled hard/medium and rejected as current times on topic is : "<<timesOnCurrentTopic<<endl;
    }
    
    //Base score
    float totalScore = 0;
	float offTopicScore = 0; //negative if this is a link & off topic
    float topicsInCommonScore = 0;
    float topicsInCommonWithPreviousScore = 0;
    float topicsInCommonWithPreviousAveraged = 0;
    float linkScore = 0;
    float samePersonOccuranceScore = 0;
    float dichotomiesScore = 0;
    float voiceOverScore = 0;
	float genderBalanceScore = 0;
	float distantClipSuppressionScore = 0;
    float goldClipScore = 0;
    float easyClipScore = 0;
	
	//need to consider discouraging clips with a lot of topics
    float topicsInCommon = parser->getSharedKeywords(currentlyPlayingClip, potentialNextClip).size();
    topicsInCommonScore += topicsInCommon * topicsInCommonMultiplier;
    
    if(history.size() > 1){
        float topicsInCommonWithPrevious = parser->getSharedKeywords(history[history.size()-2], potentialNextClip ).size();
        topicsInCommonWithPreviousAveraged = (parser->getNumberOfSharedKeywords(history[history.size()-2], potentialNextClip) /  potentialNextClip.getKeywords().size() );
        topicsInCommonWithPreviousScore += topicsInCommonWithPrevious * topicsinCommonWithPreviousMultiplier;
    }
	
	//penalize for clip missing the current topic.
    if(!ofContains(potentialNextClip.getKeywords(), topic)){
		offTopicScore = -offTopicFactor;
	}
	
	
    if( link ){
        linkScore += linkFactor;
    }
    
    //penalize for the person occurring
	//TODO: make this a little smarter
    samePersonOccuranceScore = -occurrences * samePersonOccurrenceSuppressionFactor;
    
	
    //history should contain #keywords dichotomies, and then augment score
    vector<string> specialKeywords = potentialNextClip.getSpecialKeywords();
    
    for(int i = 0; i < dichotomies.size(); i++){
        for( int k = 0; k < specialKeywords.size(); k++){
            if(dichotomies[i].right == specialKeywords[k]){
                
                if(dichotomies[i].balance > dichotomyThreshold){
                    dichotomies[i].balance = -1;
                }
                else{
                    dichotomiesScore += dichotomies[i].balance * dichotomyWeight;
                }
                
            }
            else if(dichotomies[i].left == specialKeywords[k]){
                
                if(dichotomies[i].balance < -dichotomyThreshold){
                    dichotomies[i].balance = 1;
                }
                else{
                    dichotomiesScore += -dichotomies[i].balance * dichotomyWeight;
                }
            }
        }
    }
    
    if( visualSystemRunning && potentialNextClip.hasSpecialKeyword("#vo") ){
        //TODO: Make voice over score less arbitrary
        voiceOverScore = 15;
    }
    
	//gender balance scorec
	genderBalanceScore = (potentialNextClip.getSpeakerGender() == "male" ? -1 : 1 ) * genderBalanceFactor * moreMenThanWomen;
    
	//discourage distant clips
	distantClipSuppressionScore = -currentlyPlayingClip.networkPosition.distance( potentialNextClip.networkPosition ) * distantClipSuppressionFactor;
	
    //gold clip score
    goldClipScore = (potentialNextClip.hasSpecialKeyword("gold") ? goldClipFactor : 0);
    
    //if topic is unbreached boost easy clips
    if(timesOnCurrentTopic < 1 && potentialNextClip.hasSpecialKeyword("easy")){
        easyClipScore  = easyClipScoreFactor;
    }
    
    string clipDifficulty;
    
    if (potentialNextClip.hasSpecialKeyword("easy")) {
        clipDifficulty = "easy";
    }
    else if(potentialNextClip.hasSpecialKeyword("hard")){
        clipDifficulty = "hard";
    }
    else{
        clipDifficulty = "medium";
    }
	
	//ADD IT UP
    totalScore = linkScore +
				 topicsInCommonScore +
				 topicsInCommonWithPreviousScore +
				 offTopicScore +
				 samePersonOccuranceScore +
				 dichotomiesScore +
				 genderBalanceScore +
				 distantClipSuppressionScore +
				 voiceOverScore +
				 goldClipScore +
				 easyClipScore;
    
    
    stringstream ss;
    string linkName =potentialNextClip.getLinkName();
    ofStringReplace(linkName, ",", ":");
    
    ss<<" "<<","<<" "<<","<<linkName<<","<< totalScore<<","<<linkScore<<","<<topicsInCommonScore<<","<<topicsInCommonWithPreviousScore<<","<<offTopicScore<<","<<samePersonOccuranceScore<<","<<dichotomiesScore<<","<<genderBalanceScore<<","<<distantClipSuppressionScore<<","<<voiceOverScore<<","<<goldClipScore<<","<<clipDifficulty<<endl;
    
    log = ss.str();
    
    if(printDecisions) {
        cout << "	ACCEPTED " << (link ? "LINK " : "") << totalScore << " Clip " << potentialNextClip.getLinkName() << " occurrences " << occurrences << " and " << topicsInCommon << " topics in common " <<dichotomiesScore<<" dichotomiesScore "<<voiceOverScore<<" voiceOverScore "<< endl;
        
        //        cout<<" Score Breakdown: " << totalScore <<" = "<<topicsInCommonScore<< " + " << topicsInCommonWithPreviousScore << " - " << samePersonOccuranceScore << " + "<<dichotomiesScore<<endl;
    }
    
    return MAX(totalScore, 0);
}


//TODO: timing of transitions per clip based on clip length
float CloudsStoryEngine::getHandleForClip(CloudsClip& clip){
	// if clip is longer than minimum length for long clip allow the 2 second intro
    //	if (clip.getDuration()>minClipDurationForStartingOffset) {
    //		return 0;
    //	}
	return 1; //temp just 1 for now
}

bool CloudsStoryEngine::historyContainsClip(CloudsClip& m, vector<CloudsClip>& history){
    string clipLinkName = m.getLinkName();
    
    vector<string> overlappingClips;
    if(m.hasOverlappingClips() ){
        overlappingClips = m.getOverlappingClips();
    }
    
    for(int i = 0; i < history.size(); i++){
        if(clipLinkName == history[i].getLinkName()){
            return true;
        }
        
        if (! overlappingClips.empty()) {
            if(ofContains(overlappingClips, history[i].getLinkName())){
                
                cout << "	REJECTED Clip " << m.getLinkName() << ": it overlaps with clip " <<history[i].getLinkName()<<" which has already been visited"<<endl;
                return true;
            }
        }
    }
    

    return false;
}

int CloudsStoryEngine::occurrencesOfPerson(string person, int stepsBack, vector<CloudsClip>& history){
    int occurrences = 0;
    int startPoint = history.size() - MIN(stepsBack, history.size() );
    //	cout << "finding occurrences ... " << person << " " << clipHistory.size() << " steps back " << stepsBack << " start point " << startPoint << "/" << clipHistory.size() << endl;
    for(int i = startPoint; i < history.size()-1; i++){ // -1 because the current clip is part of history
        //		cout << "COMPARING " << clipHistory[i].person << " to " << person << endl;
        if(history[i].person == person){
            occurrences++;
        }
    }
    return occurrences;
}

CloudsEvents& CloudsStoryEngine::getEvents(){
    return events;
}
