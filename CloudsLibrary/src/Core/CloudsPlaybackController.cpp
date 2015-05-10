
#include "CloudsPlaybackController.h"

#include "CloudsLocalization.h"
#include "CloudsVisualSystemVisuals.h"
#include "CloudsIntroSequence.h"
#include "CloudsVisualSystemClusterMap.h"
#include "CloudsVisualSystemRGBD.h"
#include "CloudsVisualSystemTwitter.h"

#ifdef KINECT_INPUT
#include "CloudsInputKinectOSC.h"
#endif

#ifdef MOUSE_INPUT
#include "CloudsInputMouse.h"
#endif

bool listsort(pair<int,string> a, pair<int,string> b){
    return a.first > b.first;
}

//--------------------------------------------------------------------
CloudsPlaybackController::CloudsPlaybackController(){
	
	loading = false;
	loadFinished = false;
	currentAct = NULL;
	currentClip = NULL;
	numClipsPlayed = 0;
	
	shouldLoadAct = shouldPlayAct = shouldClearAct = shouldPlayClusterMap = showingClusterMapNavigation = false;
	//selectedQuestion = NULL;
	//selectedQuestionClip = NULL;
	forceCredits = false;
	resumingActFromIntro = false;
    
    numActsCreated = 0;
    
    cachedTransition = false;
    currentVisualSystem = NULL;
	rgbdVisualSystem = NULL;
	introSequence = NULL;
	clusterMap = NULL;
	interludeSystem = NULL;
    visualsMap = NULL;
    peopleMap = NULL;

    
    interludeStartTime = 0.0;
	
	forceInterludeReset = false;
	loadingAct = false;
	currentPresetIndex = 0;
    actCreatedTime = 0.0;
	crossfadeValue = 0.0;
	
	eventsRegistered = false;

	userReset = false;
	returnToIntro = false;

	badIdle = false;
	badIdleStartTime = false;

	showingIntro = false;
	showingVisualSystem = false;
	showingClusterMap = false;
    showingInterlude = false;
    //exitedInterlude = false;
    actJustBegan = false;
    
    showingVisualLoop = false;
	
    showingExploreMap = false;
    showingExplorePeople = false;
    showingExploreVisuals = false;
 
    researchModeTopic  = false;
    researchModePerson = false;
    researchModeVisual = false;
    showingResearchScreen = false;
    
	bQuestionAsked = false;
	interludeExitBarWidth = 0.0;
	interludeHoveringContinue = false;
	interludeHoveringReset = false;
	interludeBarHoverStartTime = 0.0;
	interludeBarHoverHoldTime = 0.0;
	interludeBarHoverPercentComplete = 0.0;
	interludeContinueSelected = false;
	interludeResetSelected = false;
	interludeTimedOut = false;
	interludeArcRadius = 0.0;
	interludeArcBaseAlpha = 0.0;
	interludeForceOnTimer = 0.0;

	interludeStartTime = 0;
    numActsCreated = 0;
    crossfadeValue = 0;
    loadingAct = false;
	shouldLoadAct = false;
	
	loading = false;
	showingResearchMode = false;
    
    cachedTransition = false;
    showedClusterMapNavigation = false;

    canReturnToAct = false;
    resumingAct = false;
    getVisualLevel();
    
	resetInterludeVariables();
	

}

void CloudsPlaybackController::resetInterludeVariables(){
	
	interludeExitBarWidth = 200;
	interludeBarHoverHoldTime = 3.0; //seconds
	interludeArcRadius = 40.;
	interludeArcBaseAlpha = 1.0;

	interludeHoveringContinue = false;
	interludeHoveringReset = false;
	interludeBarHoverStartTime = 0;
	interludeBarHoverPercentComplete = 0;
	interludeContinueSelected = false;
	interludeResetSelected = false;
	interludeTimedOut = false;
	interludeForceOnTimer = 50;
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::clearAct(){
	
	if(currentAct == NULL){
		return;
	}
    
    
	CloudsVisualSystem::getRGBDVideoPlayer().stop();
	
	vector<CloudsVisualSystemPreset>& currentPresets = currentAct->getAllVisualSystemPresets();
    if( CloudsVisualSystemManager::HasSystemRegistered(currentVisualSystem) ){
        currentVisualSystem = NULL;
    }
    showingVisualSystem = false;
    
    CloudsVisualSystemManager::DeallocateSystems();
    
	currentAct->unregisterEvents(this);
	currentAct->unregisterEvents(&run);
	currentAct->unregisterEvents(&hud);
	currentAct->unregisterEvents(&oscSender);
	
    delete currentAct;
    currentAct = NULL;
	clusterMap->setAct(NULL);
    
    //This is in the case we selected to explore map from a story
    canReturnToAct = false;
    
    clearRenderTarget();
	
    numActsCreated++;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::exit(ofEventArgs & args){
	
	waitForThread(true);
	
	if(eventsRegistered){
		
		eventsRegistered = false;
		
		ofUnregisterMouseEvents(this);
		ofUnregisterKeyEvents(this);
		
		ofRemoveListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
		ofRemoveListener(storyEngine.getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
        
		ofRemoveListener(ofEvents().update, this, &CloudsPlaybackController::update);
		ofRemoveListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
        
        ofRemoveListener(CloudsIntroSequence::events.portalHoverBegan, this, &CloudsPlaybackController::portalHoverBegan);
        ofRemoveListener(CloudsIntroSequence::events.portalHoverEnded, this, &CloudsPlaybackController::portalHoverEnded);
        
        ofRemoveListener(CloudsVisualSystemRGBD::events.portalHoverBegan, this, &CloudsPlaybackController::portalHoverBegan);
        ofRemoveListener(CloudsVisualSystemRGBD::events.portalHoverEnded, this, &CloudsPlaybackController::portalHoverEnded);
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::setup(){
    
	loading = true;



    if(!eventsRegistered){
		
		eventsRegistered = true;
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
		
		ofAddListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
        ofAddListener(storyEngine.getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
		
		ofAddListener(ofEvents().update, this, &CloudsPlaybackController::update);
		ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
        
        ofAddListener(CloudsIntroSequence::events.portalHoverBegan, this, &CloudsPlaybackController::portalHoverBegan);
        ofAddListener(CloudsIntroSequence::events.portalHoverEnded, this, &CloudsPlaybackController::portalHoverEnded);
        
        ofAddListener(CloudsVisualSystemRGBD::events.portalHoverBegan, this, &CloudsPlaybackController::portalHoverBegan);
        ofAddListener(CloudsVisualSystemRGBD::events.portalHoverEnded, this, &CloudsPlaybackController::portalHoverEnded);
        
	}
	
    interludeInterfaceFont.loadFont(GetMediumFontPath(), 14);

	cout << "*****LOAD STEP*** STARTING INTRO" << endl;
	introSequence = new CloudsIntroSequence();
	introSequence->setup();
	introSequence->setDrawToScreen(false);
    
	cout << "*****LOAD STEP*** STARTING RGBD" << endl;
	rgbdVisualSystem = new CloudsVisualSystemRGBD();
	rgbdVisualSystem->setup();
	rgbdVisualSystem->setDrawToScreen(false);

	cout << "*****LOAD STEP*** STARTING CLUSTER" << endl;
	clusterMap = new CloudsVisualSystemClusterMap();
	clusterMap->setup();
	clusterMap->setDrawToScreen(false);

    cout << "******LOAD STEP*** STARTING TWITTER" << endl;
    peopleMap = new CloudsVisualSystemTwitter();
    peopleMap->setup();
    peopleMap->setDrawToScreen(false);
    
    cout << "******LOAD STEP*** STARTING VISUALS" << endl;
    visualsMap = new CloudsVisualSystemVisuals();
    visualsMap->setup();
    visualsMap->setDrawToScreen(false);
    
	cout << "*****LOAD STEP*** STARTING HUD" << endl;
	hud.setup();

#ifdef OCULUS_RIFT
    // Link the HUD.
    rgbdVisualSystem->hud = &hud;
    rgbdVisualSystem->setupHUDGui();
    
    introSequence->hud = &hud;
    introSequence->setupHUDGui();
#endif
	
	cout << "*****LOAD STEP*** SHOWING INTRO" << endl;
	showIntro();

	cout << "*****LOAD STEP*** STARTING THREAD" << endl;
	startThread();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::threadedFunction(){
    introSequence->percentLoaded = 0.0;

    
	cout << "*****LOAD STEP PARSER" << endl;

	///START THREADED
	parser.loadFromFiles();
	if(!isThreadRunning()) return;
    introSequence->percentLoaded = 0.2;
	
	cout << "*****LOAD STEP MEDIA" << endl;

    parser.loadMediaAssets();
    
    cout << "******* LOAD STEP RUN" << endl;
    if(run.load(&parser)){
        introSequence->firstPlay = false;
    }
    else{
        introSequence->firstPlay = true;
    }

    introSequence->percentLoaded = 0.3;

	if(!isThreadRunning()) return;
	
	visualSystems.loadPresets();
	visualSystems.loadCachedDataForSystems();
    introSequence->percentLoaded = 0.5;
	
	if(!isThreadRunning()) return;
	
//	///SOUND
	cout << "*****LOAD STEP SOUND" << endl;
#ifdef TONIC_SOUNDS
	mixer.setup();
#endif
	sound.setup(storyEngine);
	sound.enterTunnel();
	
	if(!isThreadRunning()) return;
    introSequence->percentLoaded = 0.6;
	
#ifndef OCULUS_RIFT
	////COMMUNICATION
	oscSender.setup();
#endif
	
	clusterMap->buildEntireCluster(parser);
    introSequence->percentLoaded = 0.7;
    
    hud.setTopics(parser.getMasterTopics());
    hud.populateSpeakers();
    hud.setVisuals(visualsMap->getAvailableSystems());
    
    introSequence->percentLoaded = 0.8;

	populateRGBDPresets();
    
    introSequence->percentLoaded = .9;
	
	//END THREADED
	loading = false;
	loadFinished = true;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::finishSetup(){
	
	clusterMap->allocateFlickerTexture();
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	storyEngine.printDecisions = false;
	storyEngine.combinedClipsOnly = true;
	storyEngine.setup();
	
	startingNodes = storyEngine.getStartingQuestions();
	introSequence->setStartQuestions(startingNodes);
    introSequence->loadingFinished();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::populateRGBDPresets(){
#ifdef OCULUS_RIFT
    
    switch (getVisualLevel()) {
        case FAST:
    
            basePreset = "RGBD_OC_BASE_fast";
            
            backgroundPresets.push_back("RGBD_OC_ACT1_fast");
            pointcloudPresets.push_back("RGBD_OC_ACT1_fast");
            
            backgroundPresets.push_back("RGBD_OC_ACT2_fast");
            pointcloudPresets.push_back("RGBD_OC_ACT2_fast");
            
            backgroundPresets.push_back("RGBD_OC_ACT3_fast");
            pointcloudPresets.push_back("RGBD_OC_ACT3_fast");
            
            break;
            
        case PRETTY:
            
            basePreset = "RGBD_OC_BASE_pretty";
            
            backgroundPresets.push_back("RGBD_OC_ACT1_pretty");
            pointcloudPresets.push_back("RGBD_OC_ACT1_pretty");
            
            backgroundPresets.push_back("RGBD_OC_ACT2_pretty");
            pointcloudPresets.push_back("RGBD_OC_ACT2_pretty");
            
            backgroundPresets.push_back("RGBD_OC_ACT3_pretty");
            pointcloudPresets.push_back("RGBD_OC_ACT3_pretty");
            
            break;
            
    }
    
#else
    
    switch (getVisualLevel()) {
            
        case FAST:
            
            basePreset = "RGBD_BASE_fast";
            
            backgroundPresets.push_back("RGBD_ACT1_fast");
            pointcloudPresets.push_back("RGBD_ACT1_fast");
            
            backgroundPresets.push_back("RGBD_ACT2_fast");
            pointcloudPresets.push_back("RGBD_ACT2_fast");
            
            backgroundPresets.push_back("RGBD_ACT3_fast");
            pointcloudPresets.push_back("RGBD_ACT3_fast");
            
            break;
            
        case PRETTY:
            
            basePreset = "RGBD_BASE_pretty";
            
            backgroundPresets.push_back("RGBD_ACT1_pretty");
            pointcloudPresets.push_back("RGBD_ACT1_pretty");
            
            backgroundPresets.push_back("RGBD_ACT2_pretty");
            pointcloudPresets.push_back("RGBD_ACT2_pretty");
            
            backgroundPresets.push_back("RGBD_ACT3_pretty");
            pointcloudPresets.push_back("RGBD_ACT3_pretty");
            
            break;
    }
#endif

}

//--------------------------------------------------------------------
CloudsRGBDVideoPlayer& CloudsPlaybackController::getSharedVideoPlayer(){
	return CloudsVisualSystem::getRGBDVideoPlayer();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showIntro(){
	
    
    float ftime = 0.1;
    ofNotifyEvent(GetCloudsAudioEvents()->fadeAudioUp, ftime);

	resetInterludeVariables();
    showingResearchMode = false;
	userReset = false;
	
#ifdef OCULUS_RIFT
    switch (getVisualLevel()) {
        case FAST:
            introSequence->loadPresetGUISFromName("Oculus_fast");
            break;
            
        case PRETTY:
            introSequence->loadPresetGUISFromName("Oculus_pretty");
            break;
    }
    
#else
    switch (getVisualLevel()) {
        case FAST:
            introSequence->loadPresetGUISFromName("TunnelWarp_fast");
        break;
        case PRETTY:
            introSequence->loadPresetGUISFromName("TunnelWarp_pretty");
        break;
    }
#endif
	introSequence->playSystem();
	
	currentVisualSystem = introSequence;
		
	numActsCreated = 0;
    
	showingVisualSystem = true;
	showingIntro = true;
    
    oscSender.reset();
    
    hud.clearQuestion();
	hud.animateOff();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::loadCurrentAct(){
	
	currentPresetIndex = 0;
	loadingAct = true;

#ifdef VHX_MEDIA
    if(currentAct != NULL){
        currentAct->fetchClipVhxUrls();
    }
#endif
}

//--------------------------------------------------------------------
void CloudsPlaybackController::updateLoadingAct(){
	
	if(currentAct == NULL){
		ofLogError("CloudsPlaybackController::updateLoadingAct") << "Current Act is NULL";
		return;
	}
	
	if(currentPresetIndex < currentAct->getAllVisualSystemPresets().size()){
		CloudsVisualSystemPreset& preset = currentAct->getAllVisualSystemPresets()[currentPresetIndex];
		
		preset.system = CloudsVisualSystemManager::InstantiateSystem(preset.systemName);
		if(preset.system != NULL){
			preset.system->setup();
		}
		else{
			ofLogError("CloudsPlaybackController::updateLoadingAct") << preset.systemName << " NULL right after instantiaton.";
		}

		currentPresetIndex++;
	}
	
	if(currentPresetIndex == currentAct->getAllVisualSystemPresets().size()){
		loadingAct = false;
		shouldPlayAct = true;
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::playCurrentAct(){
	
	currentAct->registerEvents(this);
    currentAct->registerEvents(&run);
	currentAct->registerEvents(&hud);
	currentAct->registerEvents(&oscSender);
	
	currentAct->play();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::keyPressed(ofKeyEventArgs & args){
    
	if(args.key == '\\'){
		if(showingIntro){
			introSequence->autoSelectQuestion();
		}
        else if(showingClusterMap){
            clusterMap->getTimeline()->stop();
        }
        else if(showingInterlude){
            cout << "***** FORCING INTERLUDE OFF" << endl;
            interludeSystem->getTimeline()->stop();
        }
        else{
			if(currentAct == NULL){
				return;
			}
			
            for(int i = 0; i < currentAct->getAllClips().size(); i++){
                CloudsClip* clip = currentAct->getAllClips()[i];
                for(int i = 0; i < clip->getKeywords().size(); i++){
                    run.accumuluatedTopics[clip->getKeywords()[i]]++;
                }
                run.clipHistory.push_back( clip );
            }
            for(int i = 0; i < currentAct->getAllVisualSystemPresets().size(); i++){
                run.presetHistory.push_back( currentAct->getAllVisualSystemPresets()[i].getID() );
            }
            for(int i = 0; i < currentAct->getAllTopics().size(); i++){
                run.topicHistory.push_back( currentAct->getAllTopics()[i] );
            }
            CloudsVisualSystem::getRGBDVideoPlayer().stop();
            currentAct->getTimeline().stop();
        }
	}


#ifdef OCULUS_RIFT
    if(args.key == OF_KEY_RETURN){
        if(showingInterlude){
            interludeSystem->getTimeline()->stop();
        }
        else if(showingClusterMap){
            clusterMap->getTimeline()->stop();
        }
        else if(!showingIntro){
            returnToIntro = true;
            CloudsVisualSystem::getRGBDVideoPlayer().stop();
            currentAct->getTimeline().stop();
        }
    }
#endif
	
#ifdef CLOUDS_SCREENING
	if(args.key == 'Q'){
		forceCredits = true;
	}
#endif
	
    if(args.key == 'R'){
        oscSender.reset();
    }
	
	if(args.key == 'B'){
		GetCloudsAudioEvents()->respawn = true;
	}
    
	if(args.key == 'E'){
		SetLanguage("ENGLISH");
	}

	if(args.key == 'J'){
		SetLanguage("JAPANESE");
	}

}

//--------------------------------------------------------------------
void CloudsPlaybackController::createInterludeSoundQueue(){
    CloudsSoundCue cue;
	vector<int> validInterludePresetIndices;
	ofRange validRange(56,65);


	for(int i = 0; i < sound.presets.size(); i++){
		if(validRange.contains( sound.presets[i].slotnumber) ){
			validInterludePresetIndices.push_back(i);
		}
	}
	if(validInterludePresetIndices.size() == 0){
		ofLogError("CloudsPlaybackController::createInterludeSoundQueue") << "No Valid presets for interlude";
		return;
	}
	
	lukePreset& interludePreset = sound.presets[ validInterludePresetIndices[ ofRandom(validInterludePresetIndices.size()) ] ];
	cue.startTime = 0;
	cue.duration = 60*2;
	cue.mixLevel = 2;
    
//    if(LUKEDEBUG) cout << "===============" << endl;
//    if(LUKEDEBUG) cout << "INTERLUDE MUSIC!!!" << endl;
//    if(LUKEDEBUG) cout << "===============" << endl;
//
//    if(LUKEDEBUG) cout << "TOTAL DURATION: " << cue.duration+5.0 << endl;
//    else cout << "SOUND: MUSIC STARTED." << endl;
	
    
#ifdef RTCMIX
	sound.startMusicFX(0, cue.duration+5.0);
#else
    float ftime = 0.1;
    ofNotifyEvent(GetCloudsAudioEvents()->fadeAudioUp, ftime);
#endif
	
//    if(LUKEDEBUG) cout << "   preset: " << interludePreset.slotnumber << endl;
//	sound.schedulePreset(interludePreset, cue.startTime, cue.duration, cue.mixLevel);
//#endif

	sound.schedulePreset(interludePreset, cue.startTime, cue.duration, cue.mixLevel, 0);

//    if(LUKEDEBUG) cout << "====================" << endl;
//    if(LUKEDEBUG) cout << "DONE INTERLUDE MUSIC!!!" << endl;
//    if(LUKEDEBUG) cout << "====================" << endl;

}

//--------------------------------------------------------------------
void CloudsPlaybackController::keyReleased(ofKeyEventArgs & args){
	
}

void CloudsPlaybackController::mouseDragged(ofMouseEventArgs & args){
	
}

void CloudsPlaybackController::mouseMoved(ofMouseEventArgs & args){
    //	cursorMovedTime = ofGetElapsedTimef();
}

void CloudsPlaybackController::mousePressed(ofMouseEventArgs & args){
#ifdef MOUSE_INPUT
#endif
}

void CloudsPlaybackController::mouseReleased(ofMouseEventArgs & args){
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::update(ofEventArgs & args){


    GetCloudsInput()->bUserBegan = !showingIntro || (showingIntro && introSequence->userHasBegun());

	if(loading){
		return;
	}
	
	if(loadFinished){
		finishSetup();
		loadFinished = false;
	}
	
    ////////////////////
	//OS CURSOR
#ifdef CLOUDS_RELEASE
    ofHideCursor();
#endif
    
	////////////////////
	//INTRO
	if(showingIntro){
    
        if(introSequence->isResearchModeSelected()){
            transitionController.transitionToExploreMap(1.0, 3.0);
            showingResearchMode = true;
        }
        else if(introSequence->isAboutScreenSelected()){
            hud.showAbout();
            //TODO: way to hide about...
        }
		else if(introSequence->isStartQuestionSelected()){
            transitionController.transitionFromIntro(1.0);
		}
	}
	
	////////////////////
	//CLUSTER MAP
	else if(shouldPlayClusterMap){
		shouldPlayClusterMap = false;
		transitionController.transitionToClusterMap(1.0);
	}
	else if(showingClusterMap){
        if( (showingClusterMapNavigation && clusterMap->isQuestionSelected() ) ||
            (!showingClusterMapNavigation && clusterMap->finishedTraversing) )
        {
            if(showingClusterMapNavigation){
                run.questionsAsked++;
            }
            showingClusterMap = false;
            transitionController.transitionFromClusterMap(1.0);
        }
	}
    
    ////////////////////
	//INTERLUDE
	else if(showingInterlude){
    #ifdef OCULUS_RIFT
		bool stopInterlude = updateInterludeInterface();
		
        if(stopInterlude){

            sound.stopMusic();
            
            if(interludeContinueSelected){
                transitionController.transitionFromInterlude(1.0);
            }
            else{
                transitionController.transitionToIntro(1.0);
            }
            
            showingInterlude = false;
        }
    #endif
    }
    
	////////////////////
	// RGBD SYSTEM
    if(currentVisualSystem == rgbdVisualSystem){
        if(!transitionController.isTransitioning() &&
           !bQuestionAsked &&
           rgbdVisualSystem->isQuestionSelected())
        {
            currentAct->pause();
            bQuestionAsked = true;
            run.questionsAsked++;
            transitionController.transitionWithQuestion(2.0, 0.1);
        }
    }

	getSharedVideoPlayer().showingLowerThirds = currentVisualSystem == rgbdVisualSystem;

    if(rgbdVisualSystem->getRGBDVideoPlayer().clipJustFinished()){
        hud.clipEnded();
    }
    
    ////////// HUD UPDATE AND PAUSE
    hud.update();
    if(!transitionController.isTransitioning()){
        updateHUD();
    }
    
    if(returnToIntro){
        returnToIntro = false;
        transitionController.transitionToIntro(1.0);
    }
    
    /////////////// RESEARCH MODE
    if(showingExploreMap){
        
        if(clusterMap->selectionChanged()){
            hud.selectTopic(clusterMap->getSelectedKeyword());
        }
        
        string selectedTopic = hud.getSelectedItem();
        if(selectedTopic != ""){
            
            clusterMap->setCurrentTopic(selectedTopic);
            hud.setResearchClickAnchor( clusterMap->getTopicScreenLocation() );

            if(hud.isItemConfirmed()){
                showingExploreMap = false;
                hud.animateOff();
                exploreMapSelectedTopic = selectedTopic;
                //Transition into new act based on topic
                transitionController.transitionFromExploreMap(1.0);
            }
        }
    }
    
    if(showingExplorePeople){
        string selectedSpeakerID = hud.getSelectedItem();
        
        if(peopleMap->selectionChanged()){
            string selectedTwitterID = peopleMap->getSelectedPerson();
            cout << "SELECTED TWITTER ID " << selectedTwitterID << endl;
            hud.selectPerson(CloudsSpeaker::twitterHandleToSpeaker[selectedTwitterID].fcpID);
        }
        
        if(selectedSpeakerID != ""){
            
            peopleMap->selectPerson(CloudsSpeaker::speakers[selectedSpeakerID].twitterHandle);
            hud.setResearchClickAnchor( peopleMap->getSelectedPersonScreenPosition() );

            if(hud.isItemConfirmed()){
                showingExplorePeople = false;
                hud.animateOff();
                explorePeopleSelectedSpeakerID = selectedSpeakerID;
                //Transition into new act based on topic
                transitionController.transitionFromExplorePeople(1.0);
            }
        }
    }
    
    if(showingExploreVisuals){
        string selectedVisualSystem = hud.getSelectedItem();
        if(selectedVisualSystem != ""){
            
            visualsMap->selectSystem(selectedVisualSystem);
            
            if(hud.isItemConfirmed()){
                showingExploreVisuals = false;
                hud.animateOff();
                exploreVisualsSelectedSystem = selectedVisualSystem;
                transitionController.transitionToVisualLoop(1.0, 1.0);
            }
        }
    }
    /////////////// RESEARCH MODE
    
    
	if(shouldLoadAct){
		loadCurrentAct();
		shouldLoadAct = false;
	}

	if(loadingAct){
		updateLoadingAct();
	}

	if(shouldPlayAct){
		playCurrentAct();
		shouldPlayAct = false;
	}
	
	updateTransition();
    
}

//--------------------------------------------------------------------
void CloudsPlaybackController::updateHUD(){
    
    if(hud.didPause()){
        if(currentAct != NULL){
            currentAct->pause();
        }
        if(currentVisualSystem != NULL){
            currentVisualSystem->getTimeline()->stop();
        }
        rgbdVisualSystem->getRGBDVideoPlayer().pause();
    }
    else if(hud.didUnpause()){
        if(currentAct != NULL){
            currentAct->unpause();
        }
        if(currentVisualSystem != NULL){
            currentVisualSystem->getTimeline()->play();
        }
        rgbdVisualSystem->getRGBDVideoPlayer().unpause();
    }
    /////////////////// END HUD UPDATE
    
    //////////// WAS NEXT HIT?
    if(hud.isNextHit()){
        if(showingVisualLoop){
            sound.stopMusic();
            transitionController.transitionFromVisualLoop(1.0, 1.0);
        }
        else if(showingInterlude){
            sound.stopMusic();
            transitionController.transitionFromInterlude(1.0);
        }
        else if(currentAct != NULL){
            hud.clipEnded();
            CloudsVisualSystem::getRGBDVideoPlayer().stop();
            currentAct->next();
        }
        else{
            //some stuck state, go to interlude
            transitionController.transitionToInterlude(1.0, 1.0);
        }
    }
    
    //////////// GO TO EXPLORE THE MAP FROM INTERVIEW
    if(hud.isExploreMapHit()){
        canReturnToAct = true;
        resumeState = currentVisualSystem == rgbdVisualSystem ? TRANSITION_INTERVIEW_IN : TRANSITION_VISUALSYSTEM_IN;
        if(currentClip != NULL){
            hud.selectPerson(currentClip->person);
        }
        hud.selectTopic(currentTopic);
        hud.animateOff();
        
        transitionController.transitionToExploreMap(1.0, 2.0);
    }
    
    if(hud.isSeeMorePersonHit()){
        canReturnToAct = true;
        resumeState = currentVisualSystem == rgbdVisualSystem ? TRANSITION_INTERVIEW_IN : TRANSITION_VISUALSYSTEM_IN;
        if(currentClip != NULL){
            hud.selectPerson(currentClip->person);
        }
        hud.selectTopic(currentTopic);
        hud.animateOff();
        
        transitionController.transitionToExplorePeople(1.0, 2.0);
    }
    
    ////////// GO TO DIFFERENT EXPLORE TABS
    
    if(hud.selectedMapTab()){
        transitionController.transitionToExploreMap(1.0, 1.0);
    }
    
    if(hud.selectedPeopleTab()){
        transitionController.transitionToExplorePeople(1.0, 1.0);
    }
    
    if(hud.selectedVisualsTab()){
        transitionController.transitionToExploreVisuals(1.0, 1.0);
    }
    
    if(hud.isResumeActHit() && canReturnToAct){
        canReturnToAct = false;
        resumingAct = true;
        hud.animateOff();
        transitionController.transitionBackToAct(1.0, 1.0, resumeState);
    }
    /////////////////////////////////
 
    //////////// WAS RESET HIT?
    if(!showingIntro && !showingClusterMap && !userReset &&
       (hud.isResearchResetHit() || hud.isResetHit() || rgbdVisualSystem->isResetSelected()) )
    {
        hud.unpause();
        hud.animateOff();
        
        userReset = true;
        returnToIntro = true;
#ifdef OCULUS_RIFT
        transitionController.transitionWithQuestion(2.0, 0.1);
#else
        if(currentAct != NULL){
            CloudsVisualSystem::getRGBDVideoPlayer().stop();
            currentAct->terminateAct();
        }
#endif
        
    }
}

//--------------------------------------------------------------------
void CloudsPlaybackController::updateTransition(){
	
	transitionController.update();
    
	string topic;

	if(transitionController.isStateNew()){
        
        if(transitionController.fadedOut() && shouldClearAct){
            shouldClearAct = false;
            clearAct();
        }
        
        CloudsClip* selectedQuestionClip = NULL;
        CloudsPortal* selectedQuestion = NULL;
        CloudsTransitionState lastState = transitionController.getPreviousState();
        CloudsTransitionState newState  = transitionController.getCurrentState();
        
        //first clean up the state that just finished
        switch (lastState) {

            ///LEAVING
            case TRANSITION_INTRO_IN:
                sound.enterTunnel();
                break;
                
            ///LEAVING
            case TRANSITION_INTRO_OUT:
                
                introSequence->stopSystem();
                introSequence->exit();

                showingVisualSystem = false;
                
                clearRenderTarget();

                if(introSequence->isStartQuestionSelected()){
                    
                    shouldPlayClusterMap = true;
                    
                    selectedQuestion = introSequence->getSelectedQuestion();
                    selectedQuestionClip = selectedQuestion->clip;
                    
                    //resume
                    if(selectedQuestionClip == NULL){
                        storyEngine.buildAct(run, run.clipHistory.back(), run.topicHistory.back(), true);
                    }
                    //new
                    else{
                        if(introSequence->shouldArchiveAct){
                            run.archive();
                            run.clear();
                        }
                        storyEngine.buildAct(run, selectedQuestionClip, selectedQuestion->topic, true);
                    }
                }
                
                break;
                
            ///LEAVING
            case TRANSITION_INTERVIEW_IN:
                break;
                
            ///LEAVING
            case TRANSITION_INTERVIEW_OUT:
                    
                if(bQuestionAsked){
                    
                    selectedQuestion = rgbdVisualSystem->getSelectedQuestion();
                    selectedQuestionClip = selectedQuestion->clip;
                    topic = selectedQuestion->topic;
                    
                    rgbdVisualSystem->transtionFinished();
                    rgbdVisualSystem->clearQuestions();
                    rgbdVisualSystem->stopSystem();
                    
                    CloudsVisualSystem::getRGBDVideoPlayer().stop();
                    CloudsVisualSystem::getRGBDVideoPlayer().maxVolume = 1.0;
                    
                    crossfadeValue = 0;
                    
                    shouldPlayClusterMap = run.questionsAsked % 3 == 2;
                    
                    storyEngine.buildAct(run, selectedQuestionClip, topic);
                    
                    bQuestionAsked = false;
                }
                
                rgbdVisualSystem->transtionFinished();
                rgbdVisualSystem->stopSystem();
                
                break;
                
                ///LEAVING
            case TRANSITION_VISUALSYSTEM_IN:
                
                if(currentVisualSystem == clusterMap){
                    playNextVisualSystem();
                    ofLogError("TRANSITIONED WITHOUT SYSTEM STARTING");
                }
                
                break;
                
                ///LEAVING
            case TRANSITION_INTERLUDE_OUT:
                
                if(interludeSystem == NULL){
                    ofLogError("CloudsPlaybackController::updateTransition") << "Transitioning out of NULL interlude";
                }
                else{
                    cachedTransition = true;
                    cachedTransitionType = interludeSystem->getTransitionType();
                    cleanupInterlude();
                }
                
                //build the next clip based on the history
#ifdef CLOUDS_SCREENING
                if(run.questionsAsked > 2 && !showedClusterMapNavigation){
                    createInterludeSoundQueue();
                    shouldPlayClusterMap = true;
                    showingClusterMapNavigation = true;
                }
                else{
                    storyEngine.buildAct(run);
                }
#else
                storyEngine.buildAct(run);
#endif
                break;
                
                ///LEAVING
            case TRANSITION_VISUALSYSTEM_OUT:
                //if(!canReturnToAct){
                hideVisualSystem();
                //}
                break;
                
                ///LEAVING
            case TRANSITION_CLUSTERMAP_OUT:
                    
                shouldLoadAct = true;
                shouldPlayAct = false;
                
                showingVisualSystem = false;
                clusterMap->stopSystem();
                clearRenderTarget();
                
                //For screenings only
                if(showingClusterMapNavigation){
                    showingClusterMapNavigation = false;
                    vector<CloudsClip*>& screeningQueue = storyEngine.screeningQuestionClips;
                    CloudsClip* interludeExitClip = clusterMap->getSelectedQuestion()->clip;
                    if(ofContains(screeningQueue,interludeExitClip)){
                        storyEngine.screeningQuestionClips.erase(screeningQueue.begin() + ofFind(screeningQueue, interludeExitClip));
                    }
                    
                    //build the next act
                    storyEngine.buildAct(run,
                                         clusterMap->getSelectedQuestion()->clip,
                                         clusterMap->getSelectedQuestion()->topic,
                                         true);
                }
                
                break;
                
                ///LEAVING
            case TRANSITION_EXPLORE_MAP_OUT:
                
                showingExploreMap = false;
                hideVisualSystem();
                //clusterMap->stopSystem();
                
                if(hud.isItemConfirmed()){
                    hud.clearSelection();
                    storyEngine.buildActWithTopic(run, exploreMapSelectedTopic);
                }
                    
                break;
                
                ///LEAVING
            case TRANSITION_EXPLORE_PEOPLE_OUT:
                    
                showingExplorePeople = false;
                hideVisualSystem();
                //peopleMap->stopSystem();
                
                if(hud.isItemConfirmed()){
                    hud.clearSelection();
                    storyEngine.buildActWithPerson(run, explorePeopleSelectedSpeakerID);
                }
                break;
                
                ///LEAVING
            case TRANSITION_EXPLORE_VISUALS_OUT:

                showingExploreVisuals = false;
                hideVisualSystem();
                //visualsMap->stopSystem();
                
                break;
                
                ///LEAVING
            case TRANSITION_VISUAL_LOOP_OUT:
                
                showingVisualLoop = false;
                
                if(interludeSystem == NULL){
                    ofLogError("CloudsPlaybackController::updateTransition") << "Transitioning out of NULL visual loop";
                }

                cleanupInterlude();
                
            default:
                break;
        }
        
        ///////////////////////
        /////////////NEW STATE
        //////////////////////
        switch (newState) {
                
                //starting
            case TRANSITION_INTERVIEW_OUT:
				if(bQuestionAsked || rgbdVisualSystem->isResetSelected()){
                    
                    currentAct->terminateAct();
                    hud.animateOff(CLOUDS_HUD_NEXT);
                    hud.animateOff(CLOUDS_HUD_HOME);
                    hud.animateOff(CLOUDS_HUD_LOWER_THIRD);
					rgbdVisualSystem->startTransitionOut( CloudsVisualSystem::QUESTION );
				}
				else{
                    rgbdVisualSystem->startTransitionOut( currentVisualSystem->getTransitionType() );
				}
                
                break;
                
                //starting
            case TRANSITION_INTRO_OUT:
                
                showingIntro = false;
                break;
                
                //starting
			case TRANSITION_INTRO_IN:
                                
				clusterMap->clearTraversal();
                
                if(introSequence != NULL){
                    delete introSequence;
                }
                
                introSequence = new CloudsIntroSequence();
                introSequence->setup();
				introSequence->setStartQuestions(startingNodes);
                introSequence->firstPlay = false;
                
#ifdef OCULUS_RIFT
                introSequence->hud = &hud;
                introSequence->setupHUDGui();
#endif
                introSequence->setDrawToScreen(false);
                
                hud.setHudEnabled(true);
                
                showIntro();
                introSequence->loadingFinished();
 
                break;
                
                //starting
            case TRANSITION_CLUSTERMAP_IN:
				
				showClusterMap();
				break;

                //starting
            case TRANSITION_VISUALSYSTEM_IN:
                
                playNextVisualSystem();
                break;
                
                //starting
            case TRANSITION_VISUALSYSTEM_OUT:
                
                // no need to do anything special, the crossfade value will take care of this

                break;
                
                //starting
            case TRANSITION_INTERVIEW_IN:
                
                hideVisualSystem();
                showRGBDVisualSystem();
                
                break;
                
                //starting
            case TRANSITION_INTERLUDE_OUT:
            
                break;
                
                //starting
            case TRANSITION_INTERLUDE_IN:
                
				interludeStartTime = ofGetElapsedTimef();
				
                CloudsVisualSystem::getRGBDVideoPlayer().getPlayer().stop();
                
                showInterlude();
                
				createInterludeSoundQueue();
                
                break;
            
                //starting
            case TRANSITION_EXPLORE_MAP_IN:
                showExploreMap();
                break;
                
                //starting
            case TRANSITION_EXPLORE_PEOPLE_IN:
                showExplorePeople();
                break;
                
                //starting
            case TRANSITION_EXPLORE_VISUALS_IN:
                showExploreVisuals();
				break;
                
                //starting
            case TRANSITION_VISUAL_LOOP_IN:
                if(!hud.isItemConfirmed()){
                    ofLogError("CloudsPlaybackController::updateTransition") << "HUD not confirmed going into visual loop";
                }

                hud.clearSelection();
                
                hud.animateOn(CLOUDS_HUD_NEXT);

                showingVisualLoop = true;
                
                showInterlude();
                
                createInterludeSoundQueue();
                
                break;
                
            default:
                break;
        }

		if(transitionController.fadedOut()){
			clearRenderTarget();
		}
	}
    
    crossfadeValue = transitionController.getFadeValue();

	if(bQuestionAsked){	
        CloudsVisualSystem::getRGBDVideoPlayer().maxVolume = crossfadeValue;
        //don't start fading out right away
        crossfadeValue = ofMap(crossfadeValue, .2, 0.0, 1.0, 0.0, true);
    }
	rgbdVisualSystem->visualSystemFadeValue = crossfadeValue;
    
	if(transitionController.transitioning){
		rgbdVisualSystem->updateTransition( transitionController.getInterviewTransitionPoint() );
	}   
}

//--------------------------------------------------------------------
void CloudsPlaybackController::clearRenderTarget(){
	CloudsVisualSystem::getStaticRenderTarget().begin();
	ofClear(0,0,0);
	CloudsVisualSystem::getStaticRenderTarget().end();
}

//--------------------------------------------------------------------
bool CloudsPlaybackController::updateInterludeInterface(){
	
#ifdef OCULUS_RIFT

    interludeContinueSelected = interludeHoveringContinue;
	interludeResetSelected = interludeHoveringReset;

	if(currentVisualSystem->resetNode.finished){
		interludeResetSelected = true;
		return true;
	}
	if(currentVisualSystem->continueNode.finished){
		interludeContinueSelected = true;
		return true;
	}
	if(ofGetElapsedTimef() - interludeStartTime > 30 && !interludeHoveringContinue && !interludeHoveringReset){
		interludeResetSelected = true;
		return true;
	}
	return false;
#else
    /*
	#ifdef CLOUDS_SCREENING
    if( currentVisualSystem->getSystemName() == "Balloons" ){
        hud.clearQuestion();
        return false;
    }
	#endif
	interludeTimedOut = ofGetElapsedTimef() - interludeStartTime > interludeForceOnTimer;
	
	if(GetCloudsInputX() > interludeSystem->getCanvasWidth() - interludeExitBarWidth)
	{
		if(!interludeHoveringContinue){
			interludeHoveringContinue = true;
			interludeBarHoverStartTime = ofGetElapsedTimef();
			//TODO PLAY SOUND
			CloudsVisualSystem::getClick()->setPosition(0);
			CloudsVisualSystem::getClick()->play();
		}
	}
	else{
		interludeHoveringContinue = false;
	}
	
	if(GetCloudsInputX() < interludeExitBarWidth){
		if(!interludeHoveringReset){
			interludeHoveringReset = true;
			interludeBarHoverStartTime = ofGetElapsedTimef();
			CloudsVisualSystem::getClick()->setPosition(0);
			CloudsVisualSystem::getClick()->play();
		}
	}
	else{
		interludeHoveringReset = false;
	}
	
	if(interludeHoveringContinue || interludeHoveringReset){
		interludeBarHoverPercentComplete = ofMap(ofGetElapsedTimef(),
												 interludeBarHoverStartTime,interludeBarHoverStartTime+interludeBarHoverHoldTime,
												 0.0, 1.0, true);
//		cout << " interludeBarHoverPercentComplete " << interludeBarHoverPercentComplete << endl;
		bool selectionComplete = interludeBarHoverPercentComplete == 1.0; 
		#ifdef MOUSE_INPUT
		selectionComplete |= ofGetMousePressed();
		#endif
		if(selectionComplete){
			
			///one of these will be true
			interludeContinueSelected = interludeHoveringContinue;
			interludeResetSelected = interludeHoveringReset;
			CloudsVisualSystem::getSelectMid()->setPosition(0);
			CloudsVisualSystem::getSelectMid()->play();
			return true;
		}
	}
	else {
		//slowly attenuate it back down
		interludeBarHoverPercentComplete *= 0.995;
		interludeBarHoverPercentComplete = MAX(0.0,interludeBarHoverPercentComplete-.001);
//		interludeBarHoverPercentComplete = 0;
	}
	
	if( interludeSystem->getSystemName() != "Balloons" && ofGetElapsedTimef() - interludeStartTime > 60){
		interludeResetSelected = true;
		return true;
	}
    
	return false;
     */
#endif
	
}

////--------------------------------------------------------------------
//void CloudsPlaybackController::updateCompletedInterlude(){
//
//}

/*
//--------------------------------------------------------------------
void CloudsPlaybackController::drawInterludeInterface(){
    
#ifdef CLOUDS_SCREENING
    if(currentVisualSystem->getSystemName() == "Balloons"){
        return;
    }
#endif

	ofPushStyle();
	ofSetColor(255);
	ofEnableAlphaBlending();

	ofRectangle hoverRect;
	bool hovering = false;
	string promptType;
	int tracking;
	if(interludeSystem != NULL){
        interludeInterfaceFont.setLineLength(interludeExitBarWidth);

		hoverRect = ofRectangle(interludeSystem->getCanvasWidth(), 0,
								-interludeExitBarWidth, interludeSystem->getCanvasHeight());
		hovering = interludeHoveringContinue;
		promptType = GetTranslationForString("CONTINUE");
		tracking = 6;
		drawInterludePanel(hoverRect, promptType, hovering, tracking);
        
		hoverRect = ofRectangle(0, 0, interludeExitBarWidth, interludeSystem->getCanvasHeight());
		hovering = interludeHoveringReset;
		promptType = GetTranslationForString("RESET");
		tracking = 11;
		drawInterludePanel(hoverRect, promptType, hovering, tracking);

	}
	ofDisableAlphaBlending();
	ofPopStyle();
}
*/

/*
void CloudsPlaybackController::drawInterludePanel(ofRectangle hoverRect, string promptType, bool hovering, int tracking ){

	ofPushStyle();
	ofEnableAlphaBlending();

	ofFill();
	
	float forceExtendPercent = ofMap(ofGetElapsedTimef(),
									 interludeStartTime+interludeForceOnTimer,
									 interludeStartTime+interludeForceOnTimer+2.0,
									 0.0, 1.0, true);
	float percentExtended = MAX(interludeBarHoverPercentComplete, forceExtendPercent);
	float alpha;
	if(hovering){
		hoverRect.width *= ofxTween::map(percentExtended, 0.0, 0.2, 0.0, 1.0, true, ofxEasingQuad(), ofxTween::easeOut);
		alpha = ofxTween::map(percentExtended, 0.0, .3, 0.0, 1.0, true, ofxEasingQuad(), ofxTween::easeOut);
	}
	else{
		hoverRect.width *= ofxTween::map(percentExtended, 0.7, 0.9, 0.0, 1.0, true, ofxEasingQuad(), ofxTween::easeOut);
		alpha = ofxTween::map(percentExtended, 0.7, 0.9, 0.0, 1.0, true, ofxEasingQuad(), ofxTween::easeOut);
	}
	ofFloatColor shelfColor = ofFloatColor(1.0, alpha*powf(crossfadeValue,2.0f) * .2);
	if(interludeSystem->getBgColor().getBrightness() > 100){
		ofSetColor(0, shelfColor.a*255);
	}
	else{
		ofSetColor(255, shelfColor.a*255);
	}
	ofRect(hoverRect);
	ofVec2f cursorPosition = GetCloudsInputPosition();
	float halfWidth = hoverRect.getStandardized().getWidth()*.5;
	cursorPosition.x = ofClamp(cursorPosition.x, halfWidth, interludeSystem->getCanvasWidth() - halfWidth);
	cursorPosition.y = ofClamp(cursorPosition.y, interludeArcRadius*2.2, interludeSystem->getCanvasHeight() - interludeArcRadius*2.2);
	
	//force extend after interludeForceOnTimer times out
	if(percentExtended > 0.0){
		//draw basic circle
		ofNoFill();
		ofFloatColor arcColor = ofFloatColor(1.0, 100/255.0,100/255.0, crossfadeValue*powf(crossfadeValue,2.0f) );
		if(hovering){
			ofSetColor(arcColor, 100*crossfadeValue*alpha);
		}else{
			ofSetColor(1.0, 100*crossfadeValue*alpha) ;
		}
		ofCircle(cursorPosition, interludeArcRadius*crossfadeValue);
		interludeInterfaceFont.setLetterSpacing(tracking);
		float typeWidth  = interludeInterfaceFont.stringWidth(promptType);
		float typeHeight = interludeInterfaceFont.stringHeight(promptType);
		//draw type
		ofSetColor(255, 255*alpha*crossfadeValue);
		interludeInterfaceFont.drawString(promptType,
										  hoverRect.getCenter().x - typeWidth/2 - 10,
										  interludeSystem->getCanvasHeight()*.5 + typeHeight + 10);

		if(hovering){
			float arcPercent = ofxTween::map(interludeBarHoverPercentComplete,
											 0.0, 1.0, 0.0, 1.0,
											 true, ofxEasingQuad(), ofxTween::easeOut);
			
			ofPath arc;
			arc.setFilled(false);
			arc.setStrokeWidth(4);
			arc.setStrokeColor(arcColor);
			float expandedArcRadius = interludeArcRadius + powf(1.0-crossfadeValue,2.0f) * 40; //expand it beyond when it's finished
			arc.moveTo(cursorPosition.x, cursorPosition.y - expandedArcRadius, 0);
			arc.arc(cursorPosition, expandedArcRadius, expandedArcRadius, -90, 360*arcPercent-90, true);
			arc.draw();
		}
		
	}
	
	ofPopStyle();
}
*/

#ifdef KINECT_INPUT
//--------------------------------------------------------------------
void CloudsPlaybackController::drawKinectFeedback(){
    ofPtr<CloudsInputKinectOSC> kinectInput = dynamic_pointer_cast<CloudsInputKinectOSC>(GetCloudsInput());
    kinectInput->draw();
}
#endif

//This is where everything in clouds is drawn
//--------------------------------------------------------------------
void CloudsPlaybackController::draw(ofEventArgs & args){
    
    ofBackground(0);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_DEPTH_TEST);
	
    drawRenderTarget();
    
    //drawInterludeInterface();
    
#ifdef KINECT_INPUT
    drawKinectFeedback();
#endif
    
	drawDebugOverlay();
	
	glPopAttrib();
}

void CloudsPlaybackController::drawRenderTarget(){
	if(currentVisualSystem != NULL){
		
        ofPushStyle();
        ofEnableAlphaBlending();
		ofDisableDepthTest();
        
		//cout << "crosffade value is " << crossfadeValue << " showing intro? " << showingIntro << endl;
		if(loading){
			crossfadeValue = 1.0;
		}
		ofSetColor(255, crossfadeValue*255 );
		
		currentVisualSystem->selfPostDraw();
        
        ofEnableAlphaBlending();
        
#ifndef OCULUS_RIFT
        CloudsVisualSystem::getRGBDVideoPlayer().drawSubtitles();
		hud.draw();
#endif

#ifdef MOUSE_INPUT
		if (currentVisualSystem->bPromptForInteraction) {
			bool drawBrightText = currentVisualSystem->getBgColor().getBrightness() < .5;
			ofPtr<CloudsInputMouse> mouseInput = dynamic_pointer_cast<CloudsInputMouse>(GetCloudsInput());
			mouseInput->drawFeedback(
				currentVisualSystem->getCanvasWidth(), 
				currentVisualSystem->getCanvasHeight()-400, drawBrightText);
        }
#endif

		ofPopStyle();
	}
    else{
		ofBackground(0, 0, 0);
    }
}



//--------------------------------------------------------------------
void CloudsPlaybackController::drawDebugOverlay(){
    
	if(currentAct == NULL){
		return;
	}
	
	if(ofGetKeyPressed('-')){
		currentAct->getTimeline().enableEvents();
		currentAct->drawDebug();
	}
	else{
		currentAct->getTimeline().disableEvents();
	}
	
}

#pragma story engine events
//--------------------------------------------------------------------
void CloudsPlaybackController::actCreated(CloudsActEventArgs& args){
	
    if(currentAct != NULL){
        clearAct();
    }
    
	rgbdVisualSystem->clearQuestions();
	
	numClipsPlayed = 0;
	currentAct = args.act;
	if(shouldPlayClusterMap){
		clusterMap->setAct(currentAct);
	}
	else {
		shouldLoadAct = true;
		shouldPlayAct = false;
	}

}

//--------------------------------------------------------------------
void CloudsPlaybackController::actBegan(CloudsActEventArgs& args){
	
	resetInterludeVariables();
    
    if(args.act->startsWithVisualSystem()){
        actJustBegan = true;
    }
    else {
        actJustBegan = false;
        transitionController.transitionToFirstInterview(1.0);
    }
}

//--------------------------------------------------------------------
void CloudsPlaybackController::actEnded(CloudsActEventArgs& args){

    //not sure why acts shouldn't be cleared on return to intro
    shouldClearAct = true;
    
    if(!bQuestionAsked && !returnToIntro) {
        if(showingResearchMode){
            transitionBackToResearch();
        }
        else{
            transitionController.transitionToInterlude(1.0,1.0);
        }
    }
}

void CloudsPlaybackController::transitionBackToResearch(){
    
    hud.animateOff();
    if(researchModeTopic){
        transitionController.transitionToExploreMap(1.0,1.0);
    }
    else if(researchModePerson){
        transitionController.transitionToExplorePeople(1.0,1.0);
    }
    else if(researchModeVisual){
      transitionController.transitionToExploreVisuals(1.0,1.0);
    }
    else{
        ofLogError("CloudsPlaybackController::actEnded") << "Act ended from research mode without any of the three views set";
    }
    
}

//--------------------------------------------------------------------
//void CloudsPlaybackController::resumeAct(){
//    if(!canReturnToAct || resumeVisualSystem == NULL){
//        ofLogError("CloudsPlaybackController::resumeAct") << "Resume act failed!";
//    }
//    
//    hud.pause();
//    
//    if(resumeVisualSystem == rgbdVisualSystem){
//        transitionController.transitionToFirstInterview(1.0);
//    }
//    else{
//        transitionController.transitionToFirstVisualSystem(1.0);
//    }
//}

//--------------------------------------------------------------------
void CloudsPlaybackController::clipBegan(CloudsClipEventArgs& args){
	playClip(args.chosenClip);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::visualSystemBegan(CloudsVisualSystemEventArgs& args)
{
	if(showingVisualSystem){
		ofLogError("CloudsPlaybackController::visualSystemBegan") << "Triggered visual system while still showing one";
	}
	
    if(bQuestionAsked){
        //Don't run the system
        return;
    }
    
	nextVisualSystemPreset = args.preset;
    
	//	cout << "CloudsPlaybackController::showVisualSystem SETTING NEXT SYSTEM TO " << nextVisualSystem.presetName << endl;
	if(nextVisualSystemPreset.system == NULL){
		ofLogError("CloudsPlaybackController::showVisualSystem") << "Incoming system is NULL";
	}
	
	if(currentVisualSystem == introSequence ||
	   currentVisualSystem == clusterMap ||
       actJustBegan)
	{
		transitionController.transitionToFirstVisualSystem(1.0);
        actJustBegan = false;
	}
	else if(currentVisualSystem == rgbdVisualSystem) {
		transitionController.transitionToVisualSystem(1.0, 1.0);
	}
	else {
		playNextVisualSystem();
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	if(showingVisualSystem){
		transitionController.transitionToInterview(1.0, 1.0);
	}
//	else {
//		ofLogError("CloudsPlaybackController::visualSystemEnded") << "Hiding visual system while none is showing";
//	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::questionProposed(CloudsQuestionEventArgs& args){
	//don't ask a topic that we've already seen
	if(!ofContains(run.topicHistory, args.topic) &&
       !run.historyContainsClip(args.questionClip))
    {
		rgbdVisualSystem->addQuestion(args.questionClip,
                                      args.topic,
                                      args.question);
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::questionSelected(CloudsQuestionEventArgs& args){
    
}

//--------------------------------------------------------------------
void CloudsPlaybackController::topicChanged(CloudsTopicEventArgs& args){
	currentTopic = args.topic;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::preRollRequested(CloudsPreRollEventArgs& args){
	cout << "PREROLLING CLIP TO " << 1. - args.handleLength << endl;
	prerollClip(args.preRollClip, 1. - args.handleLength);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::prerollClip(CloudsClip* clip, float toTime){

	if(clip == NULL){
		ofLogError("CloudsPlaybackController::prerollClip") << "clip IS NULL ";
		return;
	}

	if(!clip->hasMediaAsset){
		ofLogError("CloudsPlaybackController::prerollClip") << "clip " << clip->getLinkName() << " doesn't have combined video";
		return;
	}
	
    string subtitlesPath = clip->getSubtitlesPath();
    if(GetLanguage() == "ENGLISH" && clip->getLanguage() == "ENGLISH"){
        subtitlesPath = ""; //for now don't show english on english subtitles
    }
    
	bool clipLoadSuccessfullyLoaded = false;
	if(clip->voiceOverAudio){
		clipLoadSuccessfullyLoaded = CloudsVisualSystem::getRGBDVideoPlayer().setupVO(clip->voiceOverAudioPath,
                                                                                      subtitlesPath);
	}
	else{
#ifdef VHX_MEDIA
        cout << "PREROLL LOADING URL " << clip->vhxSourceVideoUrl << endl;
        clipLoadSuccessfullyLoaded = CloudsVisualSystem::getRGBDVideoPlayer().setup(clip->vhxSourceVideoUrl,
                                                                                    clip->combinedCalibrationXMLPath,
                                                                                    subtitlesPath,
                                                                                    1.0, clip->getSpeakerVolume());
#else
		clipLoadSuccessfullyLoaded = CloudsVisualSystem::getRGBDVideoPlayer().setup(clip->combinedVideoPath,
																					clip->combinedCalibrationXMLPath,
																					subtitlesPath,
																					1.0, clip->getSpeakerVolume());
#endif
	}
    
	if(!clipLoadSuccessfullyLoaded){
#ifdef VHX_MEDIA
        ofLogError("CloudsPlaybackController::prerollClip") << "Error loading clip " << clip->getLinkName() << " file url " << clip->vhxSourceVideoUrl;
#else
		ofLogError("CloudsPlaybackController::prerollClip") << "Error loading clip " << clip->getLinkName() << " file path " << clip->combinedVideoPath;
#endif
		return;
	}
    
	prerolledClipID = clip->getID();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::playClip(CloudsClip* clip){

	numClipsPlayed++;
	
	cout << "**** CLIP BEGAN" << endl;

	if(clip->getID() != prerolledClipID){
		cout << "**** HAD TO MANUALLY PREROLL IN CLIP BEGAN" << endl;
		prerollClip(clip, 1);
	}
	
	rgbdVisualSystem->setupSpeaker( 
		clip->getSpeakerFirstName(),
		clip->getSpeakerLastName(), 
		clip->name);
	
	prerolledClipID = "";
	currentClip = clip;
	currentClipName = clip->getID();
    
    //TODO: disable for VO?
    hud.setSeeMoreName( CloudsSpeaker::speakers[clip->person].firstName + " " + CloudsSpeaker::speakers[clip->person].lastName );
    
	cout << "**** SWAPPING IN FROM CLIP BEGAN" << endl;
	rgbdVisualSystem->getRGBDVideoPlayer().swapAndPlay();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showClusterMap(){
    if(showingClusterMapNavigation){
        #ifdef CLOUDS_SCREENING
		rgbdVisualSystem->clearQuestionQueue();
		storyEngine.populateScreeningQuestionsPart2();
		clusterMap->setQuestions(storyEngine.screeningQuestionClips);
		showedClusterMapNavigation = true;
		clusterMap->loadPresetGUISFromName("NavigationInterlude_Screen");
        #else
        //SHOW QUESTIONS FROM CURRENT ACT
        showingClusterMapNavigation = false; //TEMP HACK UNTIL WE GET THIS WORKING ON NON SCREENING MODE
        #endif
    }
    
    if(!showingClusterMapNavigation){
        #ifdef OCULUS_RIFT
        switch (getVisualLevel()) {
            case FAST:
                clusterMap->loadPresetGUISFromName("FollowTraverse_OculusHD_fast");
                break;
            case PRETTY:
                clusterMap->loadPresetGUISFromName("FollowTraverse_OculusHD_pretty");
                break;
        }
        
        #else
        
        switch (getVisualLevel()) {
            case FAST:
                clusterMap->loadPresetGUISFromName("FollowTraverse_Screen_fast");
                break;
            case PRETTY:
                clusterMap->loadPresetGUISFromName("FollowTraverse_Screen_pretty");
                break;
                
        }
        #endif
    }
    
    clusterMap->playSystem();
    clusterMap->clearTraversal();

    if(!showingClusterMapNavigation){
        clusterMap->autoTraversePoints = true;
        
        clusterMap->traverse();
        clusterMap->traverse();
    }
    else{
        clusterMap->autoTraversePoints = false;
    }
    
    currentVisualSystem = clusterMap;

    showingVisualSystem = true;
    showingClusterMap = true;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showInterlude(){
    
	resetInterludeVariables();
	
    CloudsVisualSystemPreset interludePreset;
    
    #ifdef CLOUDS_SCREENING
    cout << "HAS QUESTIONS REMAINING??? " << (rgbdVisualSystem->hasQuestionsRemaining() ? "YES" : "NO") << endl;
	if(!rgbdVisualSystem->hasQuestionsRemaining() && showedClusterMapNavigation){
		forceCredits = true;
	}
	#endif
    
    if(showingVisualLoop){
    
        vector<int> presetIndeces = visualSystems.getFilteredPresetIndeces(true, false, false, exploreVisualsSelectedSystem);
        if(presetIndeces.size() == 0){
            ofLogError("CloudsPlaybackController::showInterlude") << "No visual systems presets found for system " << exploreVisualsSelectedSystem;
            return;
        }

        interludePreset = visualSystems.getPresets()[ presetIndeces[ofRandom(presetIndeces.size())] ];
    }
    else{
    
        if(!storyEngine.getPresetIDForInterlude(run, interludePreset, forceCredits)){
            ofLogError("CloudsPlaybackController::showInterlude") << "Defaulting to cluster map because we found no topics from the last act";
            if(!storyEngine.getRandomInterlude(run, interludePreset)){
                returnToIntro = true;
                return;
            }
        }
    }
    
    interludeSystem = CloudsVisualSystemManager::InstantiateSystem(interludePreset.systemName);
    
	if(interludeSystem == NULL){
		returnToIntro = true;
        ofLogError("CloudsPlaybackController::showInterlude") << "Interlude system null, returning to intro";
		return;
	}

	interludeSystem->setDrawToScreen( false );
	interludeSystem->setup();
	interludeSystem->loadPresetGUISFromName( interludePreset.presetName );
	interludeSystem->playSystem();
	interludeSystem->isInterlude = true;
    
    hud.clearQuestion();
    
	currentVisualSystem = interludeSystem;
        
	showingInterlude = true;

}

//--------------------------------------------------------------------
void CloudsPlaybackController::cleanupInterlude(){

    showingVisualSystem = false;
	clearRenderTarget();

    if(currentVisualSystem == clusterMap) {
        clusterMap->stopSystem();
    }
    else if(currentVisualSystem != NULL && currentVisualSystem == interludeSystem){
        interludeSystem->stopSystem();
        interludeSystem->exit();
		
        visualSystems.DeallocateSystems();
        
        showingInterlude = false;
		interludeSystem = NULL;
		currentVisualSystem = NULL;
        
		hud.clearQuestion();
        hud.animateOff(CLOUDS_HUD_NEXT);
        
        //exitedInterlude = true;
    }
    else {
        ofLogError("CloudsPlaybackController::cleanupInterlude") << " Ended interulde while not showing ClusterMap or Interlude System";
    }
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showExploreMap(){

    hud.animateOn(CLOUDS_RESEARCH);
    if(canReturnToAct){
        hud.animateOn(CLOUDS_RESEARCH_RESUME);
    }
    
    if(showingResearchMode){ //from research mode--
        researchModeTopic  = true;
        researchModePerson = false;
        researchModeVisual = false;
    }

    //TODO fix the preset for fast
    clusterMap->loadPresetGUISFromName("TopicResearch_pretty");

    clusterMap->playSystem();
    clusterMap->clearTraversal();
    
    clusterMap->autoTraversePoints = false;
    
    currentVisualSystem = clusterMap;
    
    showingVisualSystem = true;
    showingExploreMap = true;

}

//--------------------------------------------------------------------
void CloudsPlaybackController::showExplorePeople(){
    hud.animateOn(CLOUDS_RESEARCH);
    if(canReturnToAct){
        hud.animateOn(CLOUDS_RESEARCH_RESUME);
    }

    if(showingResearchMode){
        researchModeTopic  = false;
        researchModePerson = true;
        researchModeVisual = false;
    }
    
    //TODO: make fast version
    peopleMap->loadPresetGUISFromName("ResearchPeople_pretty");
    
    peopleMap->playSystem();

    currentVisualSystem = peopleMap;
    
    showingVisualSystem = true;
    showingExplorePeople = true;
    
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showExploreVisuals(){
    hud.animateOn(CLOUDS_RESEARCH);
    if(canReturnToAct){
        hud.animateOn(CLOUDS_RESEARCH_RESUME);
    }

    if(showingResearchMode){
        researchModeTopic  = false;
        researchModePerson = false;
        researchModeVisual = true;
    }
    
    //TODO: make fast version
    visualsMap->loadPresetGUISFromName("VisualPicker_pretty");
    
    visualsMap->playSystem();
    
    currentVisualSystem = visualsMap;
    
    showingVisualSystem = true;
    showingExploreVisuals = true;
    
}

//--------------------------------------------------------------------
void CloudsPlaybackController::hideVisualSystem() {
	if(showingVisualSystem){
		currentVisualSystem->stopSystem();
		showingVisualSystem = false;
	}
    if(resumingAct){
        hud.pause();
    }
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showRGBDVisualSystem(){

	rgbdVisualSystem->loadPresetGUISFromName(basePreset);
    if(numActsCreated < 3){
		rgbdVisualSystem->loadBackgroundGUISFromName(backgroundPresets[numActsCreated]);
		rgbdVisualSystem->loadPointcloudGUISFromName(pointcloudPresets[numActsCreated]);
    }
	else{
		rgbdVisualSystem->loadBackgroundGUISFromName(backgroundPresets[ ofRandom(backgroundPresets.size()) ]);
		rgbdVisualSystem->loadPointcloudGUISFromName(pointcloudPresets[ ofRandom(pointcloudPresets.size()) ]);
	}
    
    if(currentVisualSystem == NULL){
        rgbdVisualSystem->startTransitionIn( CloudsVisualSystemRGBD::FLY_THROUGH );
    }
    else{
        if(cachedTransition){
            rgbdVisualSystem->startTransitionIn( cachedTransitionType );
            cachedTransition = false;
        }
        else {
            rgbdVisualSystem->startTransitionIn( currentVisualSystem->getTransitionType() );
        }
    }

	rgbdVisualSystem->playSystem();
	currentVisualSystem = rgbdVisualSystem;
    resumingAct = false;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::playNextVisualSystem()
{
	if(nextVisualSystemPreset.system == NULL){
		ofLogError("CloudsPlaybackController::playNextVisualSystem") << "nextVisualSystemPreset == NULL";
        return;
	}
 
    if(rgbdVisualSystem->isPlaying()){
        rgbdVisualSystem->stopSystem();
    }
    
    nextVisualSystemPreset.system->setDrawToScreen( false );
    if(!canReturnToAct){
        nextVisualSystemPreset.system->loadPresetGUISFromName( nextVisualSystemPreset.presetName );
    }
    nextVisualSystemPreset.system->playSystem();

    currentVisualSystemPreset = nextVisualSystemPreset;
    currentVisualSystem = nextVisualSystemPreset.system;
    
    cachedTransition = false;
    //in case this was a resume
    resumingAct = false;
    showingVisualSystem = true;
}

#pragma mark - Visual System Event Callbacks

//--------------------------------------------------------------------
void CloudsPlaybackController::portalHoverBegan(CloudsPortalEventArgs &args){
    hud.questionHoverOn(args.question);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::portalHoverEnded(CloudsPortalEventArgs &args){
//	hud.questionHoverOff();
//	if(args.question == "RESET"){
		//returnToIntro = true;
		//CloudsVisualSystem::getRGBDVideoPlayer().stop();
		//currentAct->getTimeline().stop();		
//	}
}