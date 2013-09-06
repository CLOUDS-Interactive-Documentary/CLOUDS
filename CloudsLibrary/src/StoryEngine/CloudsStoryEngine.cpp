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
	
//    displayGui(false);
    
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
//        
//        clipScore["linkScore"] = 0;
//        clipScore["topicInCommonScore"] = 0;
//        clipScore["topicsInCommonWithPreviousScore"] = 0;
//        clipScore["samePersonOccuranceScore"] = 0;
//        clipScore["dichotomiesScore"] = 0;
//        clipScore["voiceOverScore"] = 0;
//        
//        topicScore["lastClipCommonality"]=0;
//        topicScore["twoClipsAgoCommonality"]=0;
//        topicScore["relevancyScore"]= 0;
//        topicScore["cohesionIndex"]= 0;
//        topicScore["distance"] = 0;
        
	}
}

//void CloudsStoryEngine::updateLogs(bool addHeader){
//    string headerString = "";
//    if(addHeader){
//        map<string,int>::iterator it;
//        for(it = clipScore.begin(); it != clipScore.end(); it++){
//            headerString += it->first +",";
//        }
//        
//        for(it = topicScore.begin(); it != topicScore.end(); it++){
//            headerString += it->first +",";
//        }
//        headerString += "\n";
//        
//    }
//}

void CloudsStoryEngine::initGui(){
	
	actGui =  new ofxUISuperCanvas("ACT PARAMS", OFX_UI_FONT_SMALL);
	
	vector<CloudsClip> startingNodes = parser->getClipsWithKeyword("#start");
	vector<string> questions;
	for(int i = 0; i < startingNodes.size(); i++){
		CloudsClip& clip = startingNodes[i];
		if(!clip.hasQuestion()){
			continue;
		}
		map<string,string> clipQuestions = clip.getAllQuestionTopicPairs();
		questions.push_back( clipQuestions.begin()->first + " : " + clipQuestions.begin()->second );
	}
	
	actGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	ofxUIDropDownList* ddQuestions = actGui->addDropDownList("STARTING QUESTIONS", questions, 400);
	ddQuestions->setAutoClose(true);
	ddQuestions->setShowCurrentSelected(true);
	ddQuestions->setAllowMultiple(false);
	actGui->autoSizeToFitWidgets();
	
    clipGui = new ofxUISuperCanvas("CLIP SCORE PARAMS", OFX_UI_FONT_SMALL);
    clipGui->setPosition(0,0);
	clipGui->addSlider("MAX TIMES ON TOPIC", 2, 7, &maxTimesOnTopic);
    clipGui->addSlider("TOPICS IN COMMON CURRENT MULTIPLIER", 0, 50, &topicsInCommonMultiplier);
    clipGui->addSlider("TOPICS IN COMMON PREVIOUS MULTIPLIER", 0, 10, &topicsinCommonWithPreviousMultiplier);
    clipGui->addSlider("SAME PERSON SUPPRESSION FACTOR", 0, 10, &samePersonOccurrenceSuppressionFactor);
    clipGui->addSlider("LINK FACTOR",0,50, &linkFactor);
    clipGui->addSlider("DICHOTOMY WEIGHT", 0,10, &dichotomyWeight);
    clipGui->autoSizeToFitWidgets();
    
	topicGui = new ofxUISuperCanvas("TOPIC SCORE PARAMS", OFX_UI_FONT_SMALL);
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
	actGui->setPosition(0,0);
	clipGui->setPosition(actGui->getRect()->getMaxX(), actGui->getRect()->y);
	topicGui->setPosition(clipGui->getRect()->getMaxX(), clipGui->getRect()->y);
	gui->setPosition(topicGui->getRect()->getMaxX(), topicGui->getRect()->y);
	vsGui->setPosition(gui->getRect()->getMaxX(), gui->getRect()->y);
}

void CloudsStoryEngine::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    if(name == "STARTING QUESTIONS"){
	    ofxUIDropDownList* b = (ofxUIDropDownList*) e.widget;
		if(! b->isOpen() && b->getSelectedIndeces().size() > 0){
			CloudsClip clip = parser->getClipsWithKeyword("#start")[ b->getSelectedIndeces()[0] ];
			string topic = clip.getAllQuestionTopicPairs().begin()->first;
			cout << "SELECTED CLIP ** " << clip.getLinkName() << " WITH TOPIC " << topic << endl;
			CloudsRun run;
			buildAct(run, clip, topic);
		}
    }
}

void CloudsStoryEngine::saveGuiSettings(){
    gui->saveSettings(getDataPath() +"storyEngineParameters/gui.xml");
    clipGui->saveSettings(getDataPath() +"storyEngineParameters/clipGui.xml");
    vsGui->saveSettings(getDataPath() +"storyEngineParameters/vsGui.xml");
	topicGui->saveSettings(getDataPath() +"storyEngineParameters/topicGui.xml");
}

void CloudsStoryEngine::toggleGuis(){
	actGui->toggleVisible();
	clipGui->toggleVisible();
	topicGui->toggleVisible();
	gui->toggleVisible();
	vsGui->toggleVisible();
}

CloudsAct* CloudsStoryEngine::buildAct(CloudsRun& run, CloudsClip& seed){
    return buildAct(run, seed, seed.getKeywords()[ ofRandom(seed.getKeywords().size()) ]);
}

CloudsAct* CloudsStoryEngine::buildAct(CloudsRun& run, CloudsClip& seed, string topic){
    CloudsAct* act = new CloudsAct();
    float seconds = actLength;
    float totalSecondsEnqueued = 0;
    bool freeTopic = false;
    bool deadEnd = false;
    
    vector<CloudsClip>& clipHistory = run.clipHistory;
    vector<CloudsVisualSystemPreset>& presetHistory = run.presetHistory;
	vector<string>& topicHistory = run.topicHistory;
	
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
    stringstream scoreStream;
    stringstream topicScoreStream;
    
	scoreStream<<"SAME PERSON SUPPRESSION FACTOR,LINK FACTOR,DICHOTOMY WEIGHT,LAST CLIP COMMONALITY MULTIPLIER,TWO CLIPS COMMONALITY MULTIPLIER"<< endl;
	scoreStream<<samePersonOccurrenceSuppressionFactor<<","<<linkFactor<<","<<dichotomyWeight<<","<<topicsInCommonMultiplier<<","<<topicsinCommonWithPreviousMultiplier<< endl;
    scoreStream<<"Selected Clip,Current Topic, Potential Next Clip,Total Score,linkScore,topicsInCommonScore,topicsInCommonWithPreviousScore,( - ) samePersonOccurrenceScore,dichotomyWeight,voiceOverScore"<<endl;

	topicScoreStream << "MAX TIMES ON TOPIC,LAST CLIP SHARES TOPIC BOOST,TWO CLIPS AGO SHARES TOPIC BOOST,TOPIC RELEVANCY MULTIPLIER"<< endl;
	topicScoreStream << maxTimesOnTopic <<","<< lastClipSharesTopicBoost <<","<< twoClipsAgoSharesTopicBoost <<","<< topicRelevancyMultiplier<<"," <<endl;
	topicScoreStream<<"Current Topic,Potential Topic,Total Score,Last Clip Commonality,Two Clips Ago Commonality,Relevancy Score,Cohesion Score"<<endl;

    clearDichotomiesBalance();
	CloudsVisualSystemPreset currentPreset;
    CloudsClip clip = seed;

    int timeForNewQuesiton = 0;
	float preRollFlagTime  = 0;
	float clipHandleDuration = getHandleForClip(clip);
	act->addClipPreRollFlag(preRollFlagTime, clipHandleDuration, clip.getLinkName());
	totalSecondsEnqueued = preRollDuration;
    act->addClip(clip, topic, totalSecondsEnqueued, clipHandleDuration, getCurrentDichotomyBalance());
	clipHistory.push_back(clip);
	topicHistory.push_back(topic);
	
    totalSecondsEnqueued += clip.getDuration()+( gapLengthMultiplier * clip.getDuration() ) * clipHandleDuration*2;
    
    int timesOnCurrentTopic = 0;
    while( totalSecondsEnqueued < seconds ){
        //scoreStream<<clip.getLinkName()<<","<<topic<<","<<" "<<","<<" "<<","<<" "<<","<<" "<<","<<" "<<","<<" "<<","<<" "<<endl;
        scoreStream<<clip.getLinkName()<<","<<topic<<endl;
		
        freeTopic |= timesOnCurrentTopic > maxTimesOnTopic;
        
        if(freeTopic){
			string topicLog="";
			topicScoreStream<<topic<<","<<timesOnCurrentTopic<<" times,,,,"<<endl;
            string newTopic = selectTopic(act, clip, topicHistory, topic,topicLog);
			topicScoreStream << topicLog;
            if(newTopic == topic){
				//We landed on the same topic, we are totally stuck -- end the act!
                break;
            }
            topic = newTopic;
            timesOnCurrentTopic = 1;
            freeTopic = false;
            topicHistory.push_back(topic);
        }
        //storing a copy of current topic for each clip
        
        //get all meta data options
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
        
        vector<pair<int,string> > scoreLogPairs;
        //remove clips that share just one keyword...
        float topScore = 0;
        for(int i = 0; i < nextOptions.size(); i++){
            CloudsClip& nextClipOption = nextOptions[ i ];
            string log = "";
            
            float score = scoreForClip(clipHistory, nextClipOption, topic,log, systemRunning, isPresetIndefinite);
            
            scoreLogPairs.push_back( make_pair(score,log));
            totalPoints += score;
            topScore = MAX(topScore, score);
            nextClipOption.currentScore = score;
        }
        
        sort(scoreLogPairs.begin(), scoreLogPairs.end(), logsort);

        for (int i=0; i<scoreLogPairs.size(); i++) {
            scoreStream<<scoreLogPairs[i].second;
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
        
        if (! clip.hasKeyword(topic) ) {
            freeTopic = true;
        }
        updateDichotomies(clip);
        
        //adding all option clips with questions
        for(int k=0; k<nextOptions.size(); k++){
            
            if(nextOptions[k].getQuestionsVector().size() > 0 &&
			   parser->clipLinksTo(clip.getLinkName(), nextOptions[k].getLinkName()))
			{
				act->addQuestion(nextOptions[k], totalSecondsEnqueued);
            }
        }
        
		clipHandleDuration = getHandleForClip(clip);
        act->addClip(clip,topic,totalSecondsEnqueued, clipHandleDuration,getCurrentDichotomyBalance());
		float preRollFlagTime  = totalSecondsEnqueued - preRollDuration;
        act->addClipPreRollFlag(preRollFlagTime, clipHandleDuration, clip.getLinkName());
        clipHistory.push_back(clip);
        
        // populating Visual Systems
        //topic = act->getTopicForClip(clip);
        
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
                
                currentPreset = getVisualSystemPreset(topic);
                presetHistory.push_back(currentPreset);
                
                isPresetIndefinite = currentPreset.indefinite;
                
                if (!isPresetIndefinite) {
                    definitePresetEndTime = visualSystemStartTime + currentPreset.duration;
                }
                systemRunning = true;
            }
        }
        
        previousTopic = topic;
        totalSecondsEnqueued += clip.getDuration() + ( gapLengthMultiplier * clip.getDuration() ) + clipHandleDuration * 2;
        timesOnCurrentTopic++;

    }
	
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
    scoreBuffer.set(scoreStream);
    ofBufferToFile("clipScore.csv", scoreBuffer);
	
	ofBuffer topicBuffer;
	topicBuffer.set(topicScoreStream);
	ofBufferToFile("topicScores.csv", topicBuffer);
	
    run.actCount++;
	
    CloudsActEventArgs args(act);
    ofNotifyEvent(events.actCreated, args);
    
    return act;
}

//TODO: timing of transitions per clip based on clip length
float CloudsStoryEngine::getHandleForClip(CloudsClip& clip){
	// if clip is longer than minimum length for long clip allow the 2 second intro
    //	if (clip.getDuration()>minClipDurationForStartingOffset) {
    //		return 0;
    //	}
	return 1; //temp just 1 for now
}

CloudsVisualSystemPreset CloudsStoryEngine::getVisualSystemPreset(string keyword){
    vector<CloudsVisualSystemPreset> presets = visualSystems->getPresetsForKeyword(keyword);
    CloudsVisualSystemPreset preset;
	
	//TODO: need some way of flagging which presets have already been seen in this run
    if(presets.size() > 0){
        preset = presets[ofRandom(presets.size() -1)];
//        cout<<"Using Preset "<<preset.getID()<<" for keyword "<<keyword<<endl;
    }
    else{
        vector<string> adjacentTopics = parser->getAdjacentKeywords(keyword, 5);
        ofLogError() <<"No Presets for Keyword: " << keyword << " searching adjacent keywords " << ofJoinString(adjacentTopics, ", ") << endl;
        bool foundPreset = false;
        for (int i =0; i < adjacentTopics.size(); i++) {
            if(visualSystems->getPresetsForKeyword(adjacentTopics[i]).size() ) {
                preset = visualSystems->getPresetsForKeyword(adjacentTopics[i])[0];
                foundPreset = true;
                cout<<"Using Preset "<<preset.getID()<<" for keyword "<<keyword<<endl;
                break;
            }
        }
        
        if (! foundPreset) {
            preset = visualSystems->getRandomVisualSystem();
            ofLogError() << "No Presets found! using random preset " << preset.getID() << endl;
        }
    }
    return preset;
}

void CloudsStoryEngine::clearDichotomiesBalance(){
    for(int i=0; i <dichotomies.size();i++){
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

//TODO: use coehsion and map distance to fix dead ends
string CloudsStoryEngine::selectTopic(CloudsAct* act, CloudsClip& clip, vector<string>& topicHistory, string topic, string& log){
    
    vector<string>& topics = clip.getKeywords();
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

float CloudsStoryEngine::scoreForClip(vector<CloudsClip>& history, CloudsClip& potentialNextClip, string topic, string& log, bool visualSystemRunning, bool isPresetIndefinite){
    
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
    //Base score
    float totalScore = 0;
    float topicsInCommonScore = 0;
    float topicsInCommonWithPreviousScore = 0;
    float topicsInCommonWithPreviousAveraged = 0;
    float linkScore = 0;
    float samePersonOccuranceScore = 0;
    float dichotomiesScore = 0;
    float voiceOverScore = 0;
	
	//need to consider discouraging clips with a lot of topics
    float topicsInCommon = parser->getSharedKeywords(currentlyPlayingClip, potentialNextClip).size();
    topicsInCommonScore += topicsInCommon * topicsInCommonMultiplier;
    
    if(history.size() > 1){
        float topicsInCommonWithPrevious = parser->getSharedKeywords(history[history.size()-2], potentialNextClip ).size();
        topicsInCommonWithPreviousAveraged = (parser->getNumberOfSharedKeywords(history[history.size()-2], potentialNextClip) /  potentialNextClip.getKeywords().size() );
        topicsInCommonWithPreviousScore += topicsInCommonWithPrevious * topicsinCommonWithPreviousMultiplier;
    }
    
    if( link ){
        linkScore += linkFactor;
    }
    
    //penalize for the person occurring
	//TODO: make this a little smarter
    samePersonOccuranceScore = -occurrences * samePersonOccurrenceSuppressionFactor;
    
    //history should contain #keywords dichotomies, and then augment score
    vector<string> specialKeywords = potentialNextClip.getSpecialKeywords();
    
    for(int i=0; i< dichotomies.size(); i++){
        for( int k=0; k<specialKeywords.size(); k++){
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
        //TODO: Make less arbitrary
        voiceOverScore = 15;
    }
    
    totalScore = linkScore + topicsInCommonScore + topicsInCommonWithPreviousScore + samePersonOccuranceScore + dichotomiesScore + voiceOverScore;
    
    stringstream ss;
    string linkName =potentialNextClip.getLinkName();
    ofStringReplace(linkName, ",", ":");
    
    ss<<" "<<","<<" "<<","<<linkName<<","<< totalScore<<","<<linkScore<<","<<topicsInCommonScore<<","<<topicsInCommonWithPreviousScore<<","<<samePersonOccuranceScore<<","<<dichotomiesScore<<","<<voiceOverScore<<endl;
    
    log = ss.str();
    
    if(printDecisions) {
        cout << "	ACCEPTED " << (link ? "LINK " : "") << totalScore << " Clip " << potentialNextClip.getLinkName() << " occurrences " << occurrences << " and " << topicsInCommon << " topics in common " <<dichotomiesScore<<" dichotomiesScore "<<voiceOverScore<<" voiceOverScore "<< endl;
        
        //        cout<<" Score Breakdown: " << totalScore <<" = "<<topicsInCommonScore<< " + " << topicsInCommonWithPreviousScore << " - " << samePersonOccuranceScore << " + "<<dichotomiesScore<<endl;
    }
    
    return MAX(totalScore, 0);
}

bool CloudsStoryEngine::historyContainsClip(CloudsClip& m, vector<CloudsClip>& history){
    string clipLinkName = m.getLinkName();
    for(int i = 0; i < history.size(); i++){
        if(clipLinkName == history[i].getLinkName()){
            return true;
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
