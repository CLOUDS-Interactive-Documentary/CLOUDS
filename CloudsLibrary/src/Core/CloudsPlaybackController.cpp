
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
    //justOpened = true;
    
//	badIdle = false;
//	badIdleStartTime = false;

	showingIntro = false;
	showingVisualSystem = false;
	showingClusterMap = false;
    showingInterlude = false;
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
    bVHXRentalExpired = false;
    
	loading = false;
	showingResearchMode = false;
    
    bShowingAct = false;
    bBufferingVideo = false;

    cachedTransition = false;
    showedClusterMapNavigation = false;

    canReturnToAct = false;
    resumingAct = false;
    runningLatestVerion = true; //assume yes
    
    keyedToNext = false;
    
	resetInterludeVariables();
	lastTimeRecheckedPurchase = 0;

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
    CloudsVisualSystem::getRGBDVideoPlayer().maxVolume = 1.0;
    
	vector<CloudsVisualSystemPreset>& currentPresets = currentAct->getAllVisualSystemPresets();
    if( CloudsVisualSystemManager::HasSystemRegistered(currentVisualSystem) ){
        currentVisualSystem = NULL;
    }
    
    hideVisualSystem();
    
//    if(showingVisualSystem){
//        ofLogError("CloudsPlaybackController::clearAct") << "Clearing act with visual system still showing";
//    }
    
    CloudsVisualSystemManager::DeallocateSystems();
    
	currentAct->unregisterEvents(this);
	currentAct->unregisterEvents(&run);
	currentAct->unregisterEvents(&hud);
	#ifdef CLOUDS_INSTALLATION
	currentAct->unregisterEvents(&oscSender);
	#endif
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
	
    run.save();
    
	if(eventsRegistered){
		
		eventsRegistered = false;
		
		ofUnregisterMouseEvents(this);
		ofUnregisterKeyEvents(this);
		
		ofRemoveListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
		ofRemoveListener(storyEngine.getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
        
		ofRemoveListener(ofEvents().update, this, &CloudsPlaybackController::update);
        ofRemoveListener(ofEvents().draw, this, &CloudsPlaybackController::preDraw, OF_EVENT_ORDER_BEFORE_APP);
		ofRemoveListener(ofEvents().draw, this, &CloudsPlaybackController::draw, OF_EVENT_ORDER_AFTER_APP);
        
        ofRemoveListener(CloudsIntroSequence::events.portalHoverBegan, this, &CloudsPlaybackController::portalHoverBegan);
        ofRemoveListener(CloudsIntroSequence::events.portalHoverEnded, this, &CloudsPlaybackController::portalHoverEnded);
        
        ofRemoveListener(CloudsVisualSystemRGBD::events.portalHoverBegan, this, &CloudsPlaybackController::portalHoverBegan);
        ofRemoveListener(CloudsVisualSystemRGBD::events.portalHoverEnded, this, &CloudsPlaybackController::portalHoverEnded);
        ofRemoveListener(ofURLResponseEvent(), this, &CloudsPlaybackController::updateCheckCompleted);
        
#ifdef VHX_MEDIA
        ofRemoveListener(ofxReachability::connectedEvent, this, &CloudsPlaybackController::networkConnected);
        ofRemoveListener(ofxReachability::disconnectedEvent, this, &CloudsPlaybackController::networkDisconnected);
        
        ofRemoveListener(vhxAuth.requestTokenComplete, this, &CloudsPlaybackController::requestTokenComplete);
        ofRemoveListener(vhxAuth.refreshTokenComplete, this, &CloudsPlaybackController::refreshTokenComplete);
        ofRemoveListener(vhxAuth.requestCodeComplete, this, &CloudsPlaybackController::requestCodeComplete);
        ofRemoveListener(vhxAuth.linkCodeComplete, this, &CloudsPlaybackController::linkCodeComplete);
        ofRemoveListener(vhxAuth.verifyPackageComplete, this, &CloudsPlaybackController::verifyPackageComplete);
        ofRemoveListener(vhxAuth.codeExpired, this, &CloudsPlaybackController::codeExpired);
        ofRemoveListener(vhxAuth.packageExpired, this, &CloudsPlaybackController::packageExpired);
#endif
        
	}
    
#ifdef VHX_MEDIA
    reachability.exit();
#endif
}

//--------------------------------------------------------------------
void CloudsPlaybackController::setup(){
    
	loading = true;

//    if(ofGetKeyPressed('f')){
//        cout << "HOLDING DOWN ALT AT START UP" << endl;
//    }
    if(!eventsRegistered){
		
		eventsRegistered = true;
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
		
		ofAddListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
        ofAddListener(storyEngine.getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
		
		ofAddListener(ofEvents().update, this, &CloudsPlaybackController::update);
		ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::preDraw, OF_EVENT_ORDER_BEFORE_APP);
		ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw, OF_EVENT_ORDER_AFTER_APP);
        
        ofAddListener(CloudsIntroSequence::events.portalHoverBegan, this, &CloudsPlaybackController::portalHoverBegan);
        ofAddListener(CloudsIntroSequence::events.portalHoverEnded, this, &CloudsPlaybackController::portalHoverEnded);
        
        ofAddListener(CloudsVisualSystemRGBD::events.portalHoverBegan, this, &CloudsPlaybackController::portalHoverBegan);
        ofAddListener(CloudsVisualSystemRGBD::events.portalHoverEnded, this, &CloudsPlaybackController::portalHoverEnded);
        
        ofAddListener(ofURLResponseEvent(), this, &CloudsPlaybackController::updateCheckCompleted);
        #ifdef VHX_MEDIA
        ofAddListener(ofxReachability::connectedEvent, this, &CloudsPlaybackController::networkConnected);
        ofAddListener(ofxReachability::disconnectedEvent, this, &CloudsPlaybackController::networkDisconnected);
        
        ofAddListener(vhxAuth.requestTokenComplete, this, &CloudsPlaybackController::requestTokenComplete);
        ofAddListener(vhxAuth.refreshTokenComplete, this, &CloudsPlaybackController::refreshTokenComplete);
        ofAddListener(vhxAuth.requestCodeComplete, this, &CloudsPlaybackController::requestCodeComplete);
        ofAddListener(vhxAuth.linkCodeComplete, this, &CloudsPlaybackController::linkCodeComplete);
        ofAddListener(vhxAuth.verifyPackageComplete, this, &CloudsPlaybackController::verifyPackageComplete);
        ofAddListener(vhxAuth.codeExpired, this, &CloudsPlaybackController::codeExpired);
        ofAddListener(vhxAuth.packageExpired, this, &CloudsPlaybackController::packageExpired);
        #endif
        
	}
    interludeInterfaceFont.loadFont(GetMediumFontBuffer(), 14);

    GetGraphicsQualityLevel();
    
	cout << "*****LOAD STEP*** STARTING INTRO" << endl;
	introSequence = new CloudsIntroSequence();
	introSequence->setup();
	introSequence->setDrawToScreen(false);
    if(!MediaPathFound()){
        showIntro();
        loading = false;
        introSequence->loadingFinished();
        introSequence->alertNoMedia();
        return;
    }
    
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

//#ifdef OCULUS_RIFT
//    // Link the HUD.
//    rgbdVisualSystem->hud = &hud;
//    rgbdVisualSystem->setupHUDGui();
//    
//    introSequence->hud = &hud;
//    introSequence->setupHUDGui();
//#endif
    
#ifdef VHX_MEDIA
    reachability.setup();
#endif
	
	cout << "*****LOAD STEP*** SHOWING INTRO" << endl;
	showIntro();

	cout << "*****LOAD STEP*** STARTING THREAD" << endl;
	startThread();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::threadedFunction(){
    introSequence->percentLoaded = 0.0;

    checkForUpdates();
    
    #ifdef VHX_MEDIA
    // If tokens are saved to disk, they will be loaded automatically and the package
    // will be verified. When that's done, the verifyPackageComplete() callback will
    // get triggered, make sure args.result is either "purchase" or "rental" to proceed.
    //
    // If no tokens are found, you'll need to get a code and verify it in a browser by
    // calling requestCode() on CloudsVHXAuth. Once you get the code, call linkCode()
    // to open a browser window and link it to your account.
    
    if (!vhxAuth.setup()) {
        vhxAuth.requestCode();
    }
    
    #endif
    
    
	cout << "*****LOAD STEP PARSER" << endl;

	///START THREADED
	parser.loadFromFiles();
	if(!isThreadRunning()) return;
    introSequence->percentLoaded = 0.2;
	
	cout << "*****LOAD STEP MEDIA" << endl;

    parser.loadMediaAssets();
    
    cout << "******* LOAD STEP RUN" << endl;
    run.load(&parser);
    if(run.clipHistory.size() > 0){
        introSequence->firstPlay = false;
        hud.setHasHistory(true);
    }
    else{
        introSequence->firstPlay = true;
        hud.setHasHistory(false);
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
//	mixer.setup();
#endif
	sound.setup(storyEngine);
    parser.parseSounds(sound.renders);
	sound.enterTunnel();
    
#ifdef VHX_MEDIA
    //videos for visual systems
    CloudsVisualSystem::mapVHXMediaIds();
#endif
    
	if(!isThreadRunning()) return;
    introSequence->percentLoaded = 0.6;
	
#ifdef CLOUDS_INSTALLATION
	oscSender.setup();
#endif
	
	clusterMap->buildEntireCluster(parser);
    introSequence->percentLoaded = 0.7;
    
    populateResearch();

	populateRGBDPresets();
    
    introSequence->percentLoaded = .9;
	
	//END THREADED
	loading = false;
	loadFinished = true;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::finishSetup(){
	
	clusterMap->allocateFlickerTexture();
    visualsMap->pushTextures();
    
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	storyEngine.printDecisions = false;
	storyEngine.combinedClipsOnly = true;
	storyEngine.setup();
	
	startingNodes = storyEngine.getStartingQuestions();
	introSequence->setStartQuestions(startingNodes);
    introSequence->loadingFinished();
}

//--------------------------------------------------------------
void CloudsPlaybackController::checkForUpdates(){
    //ofLoadURLAsync("http://clouds-documentary.s3-website-us-west-2.amazonaws.com/updatecheck.txt");
    ofLoadURLAsync("https://s3-us-west-2.amazonaws.com/clouds-documentary/updatecheck_test.txt");
}

//--------------------------------------------------------------
void CloudsPlaybackController::updateCheckCompleted(ofHttpResponse& response){
    if(response.status <= 200){
        // ...
    }
    
    //version format
    //version 1.0.1
    //mac.vhx http://www.cloudsdocumentary.com/uploads/CLOUDS_1_0_1_vhx.dmg
    //mac.usb http://www.cloudsdocumentary.com/uploads/CLOUDS_1_0_1_usb.dmg
    //win.vhx http://www.cloudsdocumentary.com/uploads/CLOUDS_1_0_1_vhx.msi
    //win.usb http://www.cloudsdocumentary.com/uploads/CLOUDS_1_0_1_usb.msi

    bool latest = true;
    while(!response.data.isLastLine()){
        vector<string> line = ofSplitString(response.data.getNextLine(), " ");
        if(line.size() != 2){
            ofLogError("CloudsPlaybackController::updateCheckCompleted") << "Incorrectly formatted line";
            continue;
        }
        if(line[0] == "version"){
            latest = line[1] == CLOUDS_VERSION;
        }
        else{
            //store the url
            versionMaps[line[0]] = line[1];
        }
    }
    if(!latest){
        //cout << "UH OH NEW VERSION. Download at: " << versionMaps[CLOUDS_VERSION_KEY] << endl;
        newVersionDownloadURL = versionMaps[CLOUDS_VERSION_KEY];
        runningLatestVerion = false;
    }
    
    cout << "RESPONSE FINISHED " << response.data << " " << response.status << " " << response.error << endl;
}

#ifdef VHX_MEDIA
//--------------------------------------------------------------
void CloudsPlaybackController::networkConnected(){
    ofLogNotice("CloudsPlaybackController::networkConnected");
//    hud.hideNetworkDisconnected();
    if(bShowingAct){
        currentAct->unpause();
        getSharedVideoPlayer().unpause();
 
    }
}

//--------------------------------------------------------------
void CloudsPlaybackController::networkDisconnected(){
    ofLogNotice("CloudsPlaybackController::networkDisconnected");
    
    if(bShowingAct){
        currentAct->pause();
        getSharedVideoPlayer().pause();
        
    }

}

//--------------------------------------------------------------
void CloudsPlaybackController::requestTokenComplete(CloudsVHXEventArgs& args){
    ofLogNotice("CloudsPlaybackController::requestTokenComplete") << "Success? " << args.success << ", Token " << args.result;
    // TODO: You probably won't need to do anything here, these are automatically saved to disk.
    if(!args.success){
        introSequence->vhxError();
        return;
    }
    
}

//--------------------------------------------------------------
void CloudsPlaybackController::refreshTokenComplete(CloudsVHXEventArgs& args){
    ofLogNotice("CloudsPlaybackController::refreshTokenComplete") << "Success? " << args.success << ", Token " << args.result;
    // TODO: You probably won't need to do anything here, these are automatically saved to disk.
    if(!args.success){
        introSequence->vhxError();
        return;
    }
    if (vhxAuth.state == CloudsVHXAuth::INACTIVE) {
        // Try to verify the package with a fresh token.
        vhxAuth.verifyPackage();
    }
}

//--------------------------------------------------------------
void CloudsPlaybackController::requestCodeComplete(CloudsVHXEventArgs& args){
    ofLogNotice("CloudsPlaybackController::requestCodeComplete") << "Success? " << args.success << ", Code " << args.result;
    // TODO: Call linkCode() to open a browser window and link it to your account.
    if(!args.success){
        introSequence->vhxError();
        return;
    }
    introSequence->vhxSetAuthCode(args.result);
    vhxAuth.linkCode();

}

//--------------------------------------------------------------
void CloudsPlaybackController::linkCodeComplete(CloudsVHXEventArgs& args){
    ofLogNotice("CloudsPlaybackController::linkCodeComplete") << "Success? " << args.success << ", Token " << args.result;
    if(!args.success){
        introSequence->vhxError();
        return;
    }

    vhxAuth.verifyPackage();
}

//--------------------------------------------------------------
void CloudsPlaybackController::verifyPackageComplete(CloudsVHXEventArgs& args){
    ofLogNotice("CloudsPlaybackController::verifyPackageComplete") << "Success? " << args.success << ", State " << args.result;
    // TODO: Make sure args.result is either "purchase" or "rental" to proceed.
    if(!args.success){
        introSequence->vhxError();
        return;
    }
    if(args.result == "purchase" || args.result == "rental"){
        bVHXRentalExpired = false;
        introSequence->vhxAuthenticated();
    }
    else if(args.result == "expired"){
        introSequence->vhxRentalExpired();
    }
    else if(args.result == "inactive"){
        introSequence->vhxNotPurchase();
    }
}

//--------------------------------------------------------------
void CloudsPlaybackController::codeExpired(CloudsVHXEventArgs& args){
    ofLogNotice("CloudsPlaybackController::codeExpired");
    // You probably won't need to do anything here, but you can pre-emptively request a new code if you want.
    //vhxAuth.requestCode();
}

//--------------------------------------------------------------
void CloudsPlaybackController::packageExpired(CloudsVHXEventArgs& args){
    ofLogNotice("CloudsPlaybackController::packageExpired");
    // Stop playing CLOUDS! We need more money to continue.
    returnToIntro = true;
    bVHXRentalExpired = true;
}
#endif


//--------------------------------------------------------------------
void CloudsPlaybackController::populateRGBDPresets(){
#ifdef OCULUS_RIFT
    
    switch (GetGraphicsQualityLevel()) {
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
    
    switch (GetGraphicsQualityLevel()) {
            
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

void CloudsPlaybackController::populateResearch(){
    
    hud.setTopics(parser.getMasterTopics());
    hud.populateSpeakers();
    hud.setVisuals(visualSystems.getVisualSystemCredits());
    
    map<string,string> twitterHandleToPerson;
    map<string, CloudsSpeaker>::iterator it;
    for(it = CloudsSpeaker::speakers.begin(); it != CloudsSpeaker::speakers.end(); it++){
        if(!it->second.voiceOverOnly){
            twitterHandleToPerson[ofToLower(it->second.twitterHandle)] = it->second.firstName + " " + it->second.lastName;
        }
    }
    peopleMap->setRealNames(twitterHandleToPerson);
    visualsMap->setVisuals(visualSystems.getVisualSystemCredits());
 
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
	
    loadIntroPreset();
    
	introSequence->playSystem();
	
	currentVisualSystem = introSequence;
		
	numActsCreated = 0;
    
	showingVisualSystem = true;
	showingIntro = true;
#ifdef CLOUDS_INSTALLATION
	oscSender.reset();
#endif
    hud.clearQuestion();
	hud.animateOff();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::loadIntroPreset(){
#ifdef OCULUS_RIFT
    switch (GetGraphicsQualityLevel()) {
        case FAST:
            introSequence->loadPresetGUISFromName("Oculus_fast");
            break;
            
        case PRETTY:
            introSequence->loadPresetGUISFromName("Oculus_pretty");
            break;
    }
    
#else
    switch (GetGraphicsQualityLevel()) {
        case FAST:
            introSequence->loadPresetGUISFromName("TunnelWarp_fast");
            break;
        case PRETTY:
            introSequence->loadPresetGUISFromName("TunnelWarp_pretty");
            break;
    }
#endif
}
//--------------------------------------------------------------------
void CloudsPlaybackController::loadCurrentAct(){
	
	currentPresetIndex = 0;
	loadingAct = true;

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
#ifdef CLOUDS_INSTALLATION
	currentAct->registerEvents(&oscSender);
#endif
	currentAct->play();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::keyPressed(ofKeyEventArgs & args){
#ifndef CLOUDS_RELEASE
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
    if(!transitionController.isTransitioning()){
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
    }
    #endif
	
    #ifdef CLOUDS_SCREENING
	if(args.key == 'Q'){
		forceCredits = true;
	}
    #endif
	
    #ifdef CLOUDS_INSTALLATION
	if(args.key == 'R'){
        oscSender.reset();
    }
	#endif
    
	if(args.key == 'B'){
		GetCloudsAudioEvents()->respawn = true;
	}
    
	if(args.key == 'E'){
		SetLanguage("ENGLISH");
	}

	if(args.key == 'J'){
		SetLanguage("JAPANESE");
	}
#endif //release
    
    
    if( (args.key == OF_KEY_RETURN || args.key == ' ') && bShowingAct && currentVisualSystem != clusterMap){
        hud.togglePause();
    }
    if(args.key == OF_KEY_RIGHT && bShowingAct && currentVisualSystem != clusterMap){
        keyedToNext = true;
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
}

void CloudsPlaybackController::mousePressed(ofMouseEventArgs & args){
}

void CloudsPlaybackController::mouseReleased(ofMouseEventArgs & args){
}

void CloudsPlaybackController::mouseScrolled(ofMouseEventArgs & args){    
}

//--------------------------------------------------------------------
void CloudsPlaybackController::update(ofEventArgs & args){

//    cout << "UPDATE " << ofGetElapsedTimef() << endl;
    if(!MediaPathFound()){
        return;
    }
    
    GetCloudsInput()->bUserBegan = !showingIntro || (showingIntro && introSequence->userHasBegun());

    #ifdef VHX_MEDIA
    if(introSequence->shouldClearVHXToken){
        introSequence->shouldClearVHXToken = false;
        vhxAuth.clearTokens();
        vhxAuth.requestCode();
        introSequence->vhxWaitingForCode();
    }
    
    if(introSequence->accountNotPurchased() && ofGetElapsedTimef() - lastTimeRecheckedPurchase > 3 ){
        lastTimeRecheckedPurchase = ofGetElapsedTimef();
        vhxAuth.verifyPackage();
    }
    #endif
    
	if(loading){
        return;
	}
    
	if(loadFinished){
		finishSetup();
		loadFinished = false;
	}
	
    if(!runningLatestVerion){
        introSequence->alertNewVersion(newVersionDownloadURL);
        runningLatestVerion = true;
    }
    ////////////////////
	//OS CURSOR
#ifdef CLOUDS_RELEASE
    ofHideCursor();
#endif
    
	////////////////////
	//INTRO
	if(showingIntro){
    
        if(introSequence->isAboutScreenSelected()){
            hud.showAbout();
        }
        if(hud.aboutClosed()){
            introSequence->aboutClosed();
        }
        
        if(hud.visualLevelDidChange()){
            if(hud.setToPretty){
                SetGraphicsQualityLevel(PRETTY);
            }
            else{
                SetGraphicsQualityLevel(FAST);
            }
            loadIntroPreset();
        }
        
        if(hud.requestClearHistory() && run.clipHistory.size() > 0){
            run.archive();
            run.clear();
            run.save();
            introSequence->firstPlay = true;
        }
        
        if(introSequence->isResearchModeSelected()){
            
            //starts the transition to research
            hud.preselectMap();
            
            //populate current selections based on history
            if(run.topicHistory.size() > 0){
                hud.selectTopic("");
                clusterMap->skipNextCameraSweep();
                clusterMap->moveToTopic(parser.getMasterKeyword(run.topicHistory.back()) );
            }
            if(run.clipHistory.size() > 0){
                hud.selectPerson("");
                peopleMap->skipNextCameraSweep();
                peopleMap->moveToPerson( CloudsSpeaker::speakers[run.clipHistory.back()->person].twitterHandle);
            }
            if(run.presetHistory.size() > 0){
                visualsMap->skipNextCameraSweep();
                hud.selectVisual( visualSystems.getPresetWithID( run.presetHistory.back()).systemName );
            }
            
            transitionController.transitionToExploreMap(1.0, 3.0);
            showingResearchMode = true;
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
        if( ( showingClusterMapNavigation && clusterMap->isQuestionSelected() ) ||
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
            bQuestionAsked = true;
            run.questionsAsked++;
            currentAct->pause();
            currentAct->terminateAct();
            transitionController.transitionWithQuestion(2.0, 0.1);
        }
        rgbdVisualSystem->paused = hud.isPaused() || (transitionController.isTransitioning() && !bQuestionAsked && !rgbdVisualSystem->isResetSelected());
    }

	getSharedVideoPlayer().showingLowerThirds = currentVisualSystem == rgbdVisualSystem;

    if(bShowingAct){
        if(rgbdVisualSystem->getRGBDVideoPlayer().clipJustFinished()){
            hud.clipEnded();
        }
        
        if(!bBufferingVideo){
            if(getSharedVideoPlayer().isCurrentClipStillLoading()){
                bBufferingVideo = true;
                currentAct->pause();
                getSharedVideoPlayer().pause();
                videoBufferingStatus += "VIDEO BUFFERING: NOT LOADED\n";
                //TODO:
                //hud.showBuffering();
            }
            if(getSharedVideoPlayer().isPlaying() && !getSharedVideoPlayer().isBufferLikelyToKeepUp() ){
                bBufferingVideo = true;
                currentAct->pause();
                getSharedVideoPlayer().pause();
                videoBufferingStatus += "VIDEO BUFFERING: WONT KEEP UP\n";
                //TODO:
                //hud.showBuffering();
            }
        }
        
        if(bBufferingVideo && getSharedVideoPlayer().isBufferLikelyToKeepUp()){
            //resume
            currentAct->unpause();
            bBufferingVideo = false;
            if(getSharedVideoPlayer().isPaused()){
                getSharedVideoPlayer().unpause();
            }
            videoBufferingStatus += "VIDEO BUFFERING: PLAYBACK RESUMED\n";
            
            //TODO:
            //hud.hideBuffering();
        }
    }
    
    ////////// HUD UPDATE AND PAUSE
    hud.update();
    if(!transitionController.isTransitioning()){
        updateHUD();
    }
    
    if(returnToIntro){
        bShowingAct = false;
        if(currentAct != NULL){
            currentAct->pause();
            currentAct->terminateAct();
        }
        returnToIntro = false;
//        if(!rgbdVisualSystem->isResetSelected()){ //alrady transitioning
            transitionController.transitionToIntro(1.0);
//        }
    }
    
    /////////////// RESEARCH MODE
    if(showingExploreMap){
        
        if(clusterMap->selectionChanged()){
            hud.selectTopic(clusterMap->getSelectedKeyword());
        }

        if(hud.didItemSelectionChange()){
            clusterMap->moveToTopic(hud.getSelectedItem());
        }

        
        if(hud.isItemConfirmed()){
            exploreMapSelectedTopic = hud.getSelectedItem();
            showingExploreMap = false;
            
            hud.animateOff();
            hud.questionHoverOn(ofToUpper(exploreMapSelectedTopic));
            
            //Transition into new act based on topic
            transitionController.transitionFromExploreMap(1.0);
        }
    }
    
    if(showingExplorePeople){
        
        if(peopleMap->selectionChanged()){
            string selectedTwitterID = peopleMap->getSelectedPerson();
            hud.selectPerson(CloudsSpeaker::twitterHandleToSpeaker[selectedTwitterID].fcpID);
        }
        
        if(hud.didItemSelectionChange()){
            
            peopleMap->moveToPerson( CloudsSpeaker::speakers[hud.getSelectedItem()].twitterHandle);
        }
        
        if(hud.isItemConfirmed()){
            explorePeopleSelectedSpeakerID = hud.getSelectedItem();
            showingExplorePeople = false;
            hud.animateOff();
            string displayName = ofToUpper(CloudsSpeaker::speakers[explorePeopleSelectedSpeakerID].firstName + " " +
                                           CloudsSpeaker::speakers[explorePeopleSelectedSpeakerID].lastName);
            hud.questionHoverOn("WATCHING " + ofToUpper(displayName));
            
            //Transition into new act based on topic
            transitionController.transitionFromExplorePeople(1.0);
        }

    }
    
    if(showingExploreVisuals){
        
        if(visualsMap->selectionChanged()){
            hud.selectVisual(visualsMap->getSelectedSystem());
        }
        
        string selectedVisualSystem = hud.getSelectedItem();
        if(selectedVisualSystem != ""){

            visualsMap->selectSystem(selectedVisualSystem);
            
            if(visualsMap->selectionConfirmed()){
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

#ifdef VHX_MEDIA
    if(shouldPlayAct && currentAct->clipURLsLoaded() ){
        playCurrentAct();
        shouldPlayAct = false;
    }
#else
    if(shouldPlayAct){
        playCurrentAct();
        shouldPlayAct = false;
    }
#endif
	
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
    if(hud.isNextHit() || keyedToNext){
        keyedToNext = false;
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
        prepareForReturnableResearchTransition();
        transitionController.transitionToExploreMap(1.0, 2.0);
    }
    
    if(hud.isSeeMorePersonHit()){
        prepareForReturnableResearchTransition();
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
        hud.animateOn(CLOUDS_HUD_QUESTION);
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
//        transitionController.transitionWithQuestion(2.0, 0.1);
#else
        CloudsVisualSystem::getRGBDVideoPlayer().stop();
#endif
        
    }
}

//--------------------------------------------------------------------
void CloudsPlaybackController::prepareForReturnableResearchTransition(){
    
    canReturnToAct = true;
    bShowingAct = false;
    resumeState = currentVisualSystem == rgbdVisualSystem ? TRANSITION_INTERVIEW_IN : TRANSITION_VISUALSYSTEM_IN;
    
    populateResearchWithLatest();
    
    hud.animateOff();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::populateResearchWithLatest(){
    if(currentTopic != ""){
        string newTopic = parser.getMasterKeyword(currentTopic) ;
        if(newTopic != clusterMap->getSelectedKeyword()){
            hud.selectTopic("");
            clusterMap->skipNextCameraSweep();
        }
        clusterMap->moveToTopic(newTopic);
    }

    if(currentClip != NULL){
        if(currentClip->person != CloudsSpeaker::twitterHandleToSpeaker[peopleMap->getSelectedPerson()].fcpID){
            hud.selectPerson("");
            peopleMap->skipNextCameraSweep();
        }
        peopleMap->moveToPerson(CloudsSpeaker::speakers[currentClip->person].twitterHandle);
    }

    if(nextVisualSystemPreset.systemName != ""){
        if(nextVisualSystemPreset.systemName != visualsMap->getSelectedSystem() ){
            visualsMap->skipNextCameraSweep();
        }
        hud.selectVisual(nextVisualSystemPreset.systemName);
    }
}

//--------------------------------------------------------------------
void CloudsPlaybackController::updateTransition(){
	
	transitionController.update();
    
	string topic;

	if(transitionController.isStateNew()){
        
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
                rgbdVisualSystem->transtionFinished();

                break;
                
            ///LEAVING
            case TRANSITION_INTERVIEW_OUT:
                
                rgbdVisualSystem->transtionFinished();
                
                if(bQuestionAsked){
                    
                    if(shouldClearAct){
                        shouldClearAct = false;
                        clearAct();
                    }

                    selectedQuestion = rgbdVisualSystem->getSelectedQuestion();
                    selectedQuestionClip = selectedQuestion->clip;
                    topic = selectedQuestion->topic;
                    
                    rgbdVisualSystem->clearQuestions();
                    rgbdVisualSystem->stopSystem();
                    
                    CloudsVisualSystem::getRGBDVideoPlayer().stop();
                    CloudsVisualSystem::getRGBDVideoPlayer().maxVolume = 1.0;
                    
                    crossfadeValue = 0;
                    
                    shouldPlayClusterMap = run.questionsAsked % 3 == 2;
                    
                    storyEngine.buildAct(run, selectedQuestionClip, topic);
                    
                    bQuestionAsked = false;
                }
                else{
                    rgbdVisualSystem->stopSystem();
                }
                
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
//#ifdef CLOUDS_SCREENING
//                if(run.questionsAsked > 2 && !showedClusterMapNavigation){
//                    createInterludeSoundQueue();
//                    shouldPlayClusterMap = true;
//                    showingClusterMapNavigation = true;
//                }
//                else{
//                    storyEngine.buildAct(run);
//                }
//#else
                //if we are not going back to the intro
                if(newState != TRANSITION_INTRO_IN){
                    storyEngine.buildAct(run);
                }
//#endif
                break;
                
                ///LEAVING
            case TRANSITION_VISUALSYSTEM_OUT:

                hideVisualSystem();

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
                hud.researchTransitionFinished();
                
                if(exploreMapSelectedTopic != ""){
                    hud.clearSelection();
                    storyEngine.buildActWithTopic(run, exploreMapSelectedTopic);
                    exploreMapSelectedTopic = "";
                }
                break;
                
                ///LEAVING
            case TRANSITION_EXPLORE_PEOPLE_OUT:
                    
                showingExplorePeople = false;
                hideVisualSystem();
                hud.researchTransitionFinished();
                
                if(explorePeopleSelectedSpeakerID != ""){
                    hud.clearSelection();
                    storyEngine.buildActWithPerson(run, explorePeopleSelectedSpeakerID);
                    explorePeopleSelectedSpeakerID = "";
                }
                break;
                
                ///LEAVING
            case TRANSITION_EXPLORE_VISUALS_OUT:

                showingExploreVisuals = false;
                hideVisualSystem();
                hud.researchTransitionFinished();
                break;
                
                ///LEAVING
            case TRANSITION_VISUAL_LOOP_OUT:
                

                showingVisualLoop = false;
                
                if(interludeSystem == NULL){
                    ofLogError("CloudsPlaybackController::updateTransition") << "Transitioning out of NULL visual loop";
                }

                cleanupInterlude();
                hud.exitingVisuals();
                
            default:
                break;
        }

        
        //clean up acts in between these states
        if(transitionController.fadedOut()){
            clearRenderTarget();
            if(shouldClearAct){
                shouldClearAct = false;
                clearAct();
            }
        }
        
        ///////////////////////
        /////////////NEW STATE
        //////////////////////
        switch (newState) {
                
                //starting
            case TRANSITION_INTERVIEW_OUT:
				if(bQuestionAsked || rgbdVisualSystem->isResetSelected()){
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
                introSequence->setStartQuestions(startingNodes);
                introSequence->firstPlay = false;
                introSequence->shouldArchiveAct = false;
                introSequence->loadingFinished();
                if(bVHXRentalExpired){
                    introSequence->vhxRentalExpired();
                }
                
                hud.setHudEnabled(true);
                hud.setHasHistory(true);
                
                showIntro();
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
                hud.researchTransitionFinished();
                showExploreMap();
                break;
                
                //starting
            case TRANSITION_EXPLORE_PEOPLE_IN:
                hud.researchTransitionFinished();
                showExplorePeople();
                break;
                
                //starting
            case TRANSITION_EXPLORE_VISUALS_IN:
                hud.researchTransitionFinished();
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
                
                hud.enteringVisuals();

                break;
                
            default:
                break;
        }
        
        //if we came back from an act, force the HUD to display and move everything along.
        if(transitionController.fadedOut() && resumingAct){
            hud.unpause(true);
            resumingAct = false;
        }

	}
    
    crossfadeValue = transitionController.getFadeValue();
    hud.transitionFade = crossfadeValue;
    
	if(bQuestionAsked || rgbdVisualSystem->isResetSelected()){
        CloudsVisualSystem::getRGBDVideoPlayer().maxVolume = crossfadeValue;
        //don't start fading out right away
        crossfadeValue = ofMap(crossfadeValue, .2, 0.0, 1.0, 0.0, true);
    }
    else{
        CloudsVisualSystem::getRGBDVideoPlayer().maxVolume = 1.0;
    }
	rgbdVisualSystem->visualSystemFadeValue = crossfadeValue;
    
//	#ifdef OCULUS_RIFT
//	CloudsVisualSystem::getOculusRift().setFade(crossfadeValue);
//	#endif
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
#endif
	return false;
	
}

#ifdef KINECT_INPUT
//--------------------------------------------------------------------
void CloudsPlaybackController::drawKinectFeedback(){
    ofPtr<CloudsInputKinectOSC> kinectInput = dynamic_pointer_cast<CloudsInputKinectOSC>(GetCloudsInput());
    kinectInput->draw();
}
#endif

//--------------------------------------------------------------------
void CloudsPlaybackController::preDraw(ofEventArgs& args){
#ifdef OCULUS_RIFT
    
    float hudDistance = CloudsVisualSystem::subtitleHudZ;
    float hudScale = CloudsVisualSystem::subtitleHudScale;
    CloudsVisualSystem::getOculusRift().beginOverlay(hudDistance, hudScale, 1920, 1080);

    if(currentVisualSystem == introSequence){
        introSequence->selfDrawOverlay();
    }
    CloudsVisualSystem::getRGBDVideoPlayer().drawSubtitles();
    hud.draw();
    
    CloudsVisualSystem::getOculusRift().endOverlay();
	
#endif
}

//This is where everything in clouds is drawn
//--------------------------------------------------------------------
void CloudsPlaybackController::draw(ofEventArgs & args){
    
//    cout << "DRAW " << ofGetElapsedTimef() << endl;

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
		if(loading || !MediaPathFound()){
			crossfadeValue = 1.0;
		}
		#ifdef OCULUS_RIFT
		CloudsVisualSystem::getOculusRift().setFade(crossfadeValue);
		#else
		ofSetColor(255, crossfadeValue*255 );
		#endif
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
        drawVideoStatus();
	}
	else{
		currentAct->getTimeline().disableEvents();
	}
	
}

void CloudsPlaybackController::drawVideoStatus(){
    ofPushStyle();
    
    string statusString = "";
    statusString += string("LOADING:  ") + (getSharedVideoPlayer().isCurrentClipStillLoading() ? "YES" : "NO") + "\n";
    statusString += string("LOADED:   ") + (getSharedVideoPlayer().getPlayer().isLoaded() ? "YES" : "NO") + "\n";
    statusString += string("PLAYING:  ") + (getSharedVideoPlayer().getPlayer().isPlaying() ? "YES" : "NO") + "\n";
    statusString += string("WILLPLAY: ") + (getSharedVideoPlayer().getPlayer().isBufferLikelyToKeepUp() ? "YES" : "NO") + "\n";
    statusString += string("PERCENT:  ") + ofToString(getSharedVideoPlayer().getPlayer().getBufferProgress(), 5) + "\n";
    statusString += string("VIDPAUSED:") + (getSharedVideoPlayer().isPaused() ? "YES" : "NO") + "\n";
    statusString += string("PLYPAUSED:") + (getSharedVideoPlayer().getPlayer().isPaused() ? "YES" : "NO") + "\n";
    statusString += string("RATE:     ") + (ofToString(getSharedVideoPlayer().getPlayer().getSpeed())) + "\n";
    statusString += videoBufferingStatus;
    
    ofSetColor(0);
    ofRectangle(20,20, 200, 200);
    ofSetColor(255);
    ofDrawBitmapString(statusString, 30, 30);
    
    ofPopStyle();
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
    
#ifdef VHX_MEDIA
    currentAct->fetchClipVhxUrls();
#endif
    
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
    bShowingAct = true;
    
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
    bBufferingVideo = false;
    
    if(!bQuestionAsked && !returnToIntro) {
        //if we entered this act through research mode, return to the research screen
        if(showingResearchMode){
            transitionBackToResearch();
        }
        //otherwise we are in story mode and we should go to an interlude
        else{
            transitionController.transitionToInterlude(1.0,1.0);
        }
    }
}

void CloudsPlaybackController::transitionBackToResearch(){

    populateResearchWithLatest();

    hud.clearQuestion();
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
#ifdef VHX_MEDIA
		clipLoadSuccessfullyLoaded = CloudsVisualSystem::getRGBDVideoPlayer().setupVO(clip->vhxSourceVideoUrl, subtitlesPath);
#else
        clipLoadSuccessfullyLoaded = CloudsVisualSystem::getRGBDVideoPlayer().setupVO(clip->voiceOverAudioPath, subtitlesPath);
#endif
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
    videoBufferingStatus += "PREROLLED CLIP " + clip->getID() + "\n";
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
    
    if(currentClip->voiceOverAudio){
        hud.setSeeMoreName( "EXPLORE THE NETWORK" );
    }
    else{
        hud.setSeeMoreName( "SEE MORE OF " + ofToUpper(CloudsSpeaker::speakers[clip->person].firstName + " " + CloudsSpeaker::speakers[clip->person].lastName) );
    }
    
	cout << "**** SWAPPING IN FROM CLIP BEGAN" << endl;
	rgbdVisualSystem->getRGBDVideoPlayer().swapAndPlay();
    videoBufferingStatus += "SWAPPED CLIP: " + currentClipName + "\n";
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showClusterMap(){
    if(showingClusterMapNavigation){
//        #ifdef CLOUDS_SCREENING
//		rgbdVisualSystem->clearQuestionQueue();
//		storyEngine.populateScreeningQuestionsPart2();
//		clusterMap->setQuestions(storyEngine.screeningQuestionClips);
//		showedClusterMapNavigation = true;
//		clusterMap->loadPresetGUISFromName("NavigationInterlude_Screen");
//        #else
        //SHOW QUESTIONS FROM CURRENT ACT
        showingClusterMapNavigation = false; //TEMP HACK UNTIL WE GET THIS WORKING ON NON SCREENING MODE
//        #endif
    }
    
    if(!showingClusterMapNavigation){
        #ifdef OCULUS_RIFT
        switch (GetGraphicsQualityLevel()) {
            case FAST:
                clusterMap->loadPresetGUISFromName("FollowTraverse_OculusHD_fast");
                break;
            case PRETTY:
                clusterMap->loadPresetGUISFromName("FollowTraverse_OculusHD_pretty");
                break;
        }
        
        #else
        
        switch (GetGraphicsQualityLevel()) {
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
    
//    #ifdef CLOUDS_SCREENING
//    cout << "HAS QUESTIONS REMAINING??? " << (rgbdVisualSystem->hasQuestionsRemaining() ? "YES" : "NO") << endl;
//	if(!rgbdVisualSystem->hasQuestionsRemaining() && showedClusterMapNavigation){
//		forceCredits = true;
//	}
//	#endif
    
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
    hud.animateOn(CLOUDS_HUD_NEXT);
    
	currentVisualSystem = interludeSystem;
    
    bShowingAct = false;
    
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
		
        if(!canReturnToAct){
            visualSystems.DeallocateSystems();
        }
        
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
    if(hud.isItemSelected() && clusterMap->getSelectedKeyword() == hud.getSelectedItem()){
        hud.animateOn(CLOUDS_RESEARCH_TOPIC);
    }
    if(canReturnToAct){
        hud.animateOn(CLOUDS_RESEARCH_RESUME);
    }
    
    if(showingResearchMode){ //from research mode--
        researchModeTopic  = true;
        researchModePerson = false;
        researchModeVisual = false;
    }

    hud.setVisitedTopics(run.getVisitedTopics());
    
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
    if(hud.isItemSelected() &&
       ofToLower(peopleMap->getSelectedPerson()) == ofToLower(CloudsSpeaker::speakers[hud.getSelectedItem()].twitterHandle))
    {
        hud.animateOn(CLOUDS_RESEARCH_PPL);
    }
    
    if(canReturnToAct){
        hud.animateOn(CLOUDS_RESEARCH_RESUME);
    }

    if(showingResearchMode){
        researchModeTopic  = false;
        researchModePerson = true;
        researchModeVisual = false;
    }
    
    hud.setVisitedPeople(run.getVisitedPeople());

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
    
    hud.setVisitedVisuals(run.getVisitedVisuals());
                          
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
    //resumingAct = false;
    bShowingAct = true;
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
    if(!resumingAct){
        nextVisualSystemPreset.system->loadPresetGUISFromName( nextVisualSystemPreset.presetName );
    }
    nextVisualSystemPreset.system->playSystem();
    
    currentVisualSystemPreset = nextVisualSystemPreset;
    currentVisualSystem = nextVisualSystemPreset.system;
    
    cachedTransition = false;
    //in case this was a resume
    //resumingAct = false;
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