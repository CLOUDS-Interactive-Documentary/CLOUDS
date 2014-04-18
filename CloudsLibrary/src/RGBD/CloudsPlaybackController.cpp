
#include "CloudsPlaybackController.h"

#ifdef KINECT_INPUT
#include "CloudsInputKinectOSC.h"
#endif

bool listsort(pair<int,string> a, pair<int,string> b){
    return a.first > b.first;
}

//--------------------------------------------------------------------
CloudsPlaybackController::CloudsPlaybackController(){
	
	eventsRegistered = false;
	showingVisualSystem = false;
	currentAct = NULL;
    interludeSystem = NULL;
	showingClusterMap = false;
	shouldPlayAct = false;
    shouldClearAct = false;
	bQuestionAsked = false;
    showingInterlude = false;
    exitedInterlude = false;
	interludeStartTime = 0;
    numActsCreated = 0;
    crossfadeValue = 0;
    loadingAct = false;
	shouldLoadAct = false;
	
	loading = false;
	loadPercent = 0.0;
	
    returnToIntro = false;
    cachedTransition = false;
    
	resetInterludeVariables();
	
    interludeInterfaceFont.loadFont(GetCloudsDataPath()+"font/Blender-BOOK.ttf", 15);
    
#ifdef KINECT_INPUT
    kinectFeedbackAlpha = 0;
    kinectFeedbackFont.loadFont(GetCloudsDataPath()+"font/Blender-BOOK.ttf", 15);
#endif

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
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::clearAct(){
	
	if(currentAct == NULL){
		return;
	}
	
	vector<CloudsVisualSystemPreset>& currentPresets = currentAct->getAllVisualSystemPresets();
    if( CloudsVisualSystemManager::HasSystemRegistered(currentVisualSystem) ){
        currentVisualSystem = NULL;
    }
    showingVisualSystem = false;
    
    visualSystems.DeallocateSystems();
    
	currentAct->unregisterEvents(this);
	currentAct->unregisterEvents(&run);
	currentAct->unregisterEvents(&hud);
	currentAct->unregisterEvents(&oscSender);
	
    delete currentAct;
    currentAct = NULL;
    
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
	
//	clearAct();
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
		
	introSequence = new CloudsIntroSequence();
	introSequence->setup();
	introSequence->setDrawToScreen(false);
	
	rgbdVisualSystem = new CloudsVisualSystemRGBD();
	rgbdVisualSystem->setup();
	rgbdVisualSystem->setDrawToScreen(false);

	clusterMap = new CloudsVisualSystemClusterMap();
	clusterMap->setup();
	clusterMap->setDrawToScreen(false);
	
	hud.setup();
	
	showIntro();

	startThread();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::threadedFunction(){
	
	loadPercent = 0.0;
	
	///START THREADED
	parser.loadFromFiles();
	if(!isThreadRunning()) return;
	
	if(!ofFile::doesFileExist(GetCloudsDataPath() + "CloudsMovieDirectory.txt")){
		ofSystemAlertDialog("Could not find movie file path. \
							Create a file called CloudsMovieDirectory.txt \
							that contains one line, the path to your movies folder");
	}
	
	parser.setCombinedVideoDirectory(ofBufferFromFile(GetCloudsDataPath() + "CloudsMovieDirectory.txt").getText());
	
	if(!isThreadRunning()) return;
	
	visualSystems.loadPresets();
	visualSystems.loadCachedDataForSystems();
	
	if(!isThreadRunning()) return;
	
	///SOUND
	mixer.setup();
	sound.setup(storyEngine);
	sound.enterTunnel();
	
	if(!isThreadRunning()) return;
	
#ifndef  OCULUS_RIFT
	////COMMUNICATION
	oscSender.setup();
#endif
	
	clusterMap->setRun(run);
	clusterMap->buildEntireCluster(parser);
	
	//END THREADED
	loading = false;
	loadFinished = true;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::finishSetup(){
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	storyEngine.printDecisions = false;
	storyEngine.combinedClipsOnly = true;
	storyEngine.setup();
	
	startingNodes = storyEngine.getStartingQuestions();
	introSequence->setStartQuestions(startingNodes);

#ifdef OCULUS_RIFT
    rgbdVisualSystem->hud = &hud;
    rgbdVisualSystem->setupHUDGui();
    
    introSequence->hud = &hud;
    introSequence->setupHUDGui();
#endif

}

//--------------------------------------------------------------------
CloudsRGBDVideoPlayer& CloudsPlaybackController::getSharedVideoPlayer(){
	return CloudsVisualSystem::getRGBDVideoPlayer();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showIntro(){
	
	resetInterludeVariables();
	
#ifdef OCULUS_RIFT
	introSequence->loadPresetGUISFromName("Oculus");
#else
	introSequence->loadPresetGUISFromName("TunnelWarp");
#endif
	introSequence->playSystem();
	
	currentVisualSystem = introSequence;
	
	//HACCCK!
//	currentVisualSystem->isInterlude = true;
	
	numActsCreated = 0;
    
	showingVisualSystem = true;
	showingIntro = true;
	hud.setHomeEnabled(false);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::loadCurrentAct(){
	
	currentPresetIndex = 0;
	loadingAct = true;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::updateLoadingAct(){

	CloudsVisualSystemPreset& preset = currentAct->getAllVisualSystemPresets()[currentPresetIndex];
	
	preset.system = CloudsVisualSystemManager::InstantiateSystem(preset.systemName);
	if(preset.system != NULL){
		preset.system->setup();
	}
	else{
		ofLogError("CloudsPlaybackController::updateLoadingAct") << preset.systemName << " NULL right after instantiaton.";
	}
	
	currentPresetIndex++;
	if(currentPresetIndex == currentAct->getAllVisualSystemPresets().size()){
		loadingAct = false;
		shouldPlayAct = true;
	}
	
//	for(int i = 0; i < presets.size(); i++){
//		if(presetsToLoad[i].system != NULL){
//            //			cout << "CloudsPlaybackController::playAct -- Setting up:: " << presets[i].systemName << endl;
//			presetsToLoad[i].system->setup();
//		}
//		else{
//			ofLogError("CloudsPlaybackController::playAct") << presetsToLoad[i].systemName << " NULL right after instantiaton.");
//		}
//	}
	
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
	
	if(args.key == 'Q'){
		for(int i = 0; i < fakeQuestions.size(); i++){
			rgbdVisualSystem->addQuestion(fakeQuestions[i],
										  fakeQuestions[i].getTopicsWithQuestions()[0],
										  fakeQuestions[i].getQuestions()[0]);
		}
	}
    
    
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
                CloudsClip& clip = currentAct->getAllClips()[i];
                for(int i = 0; i < clip.getKeywords().size(); i++){
                    run.accumuluatedTopics[clip.getKeywords()[i]]++;
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
    
    if(args.key == 'R'){
        oscSender.reset();
    }
	
	if(args.key == 'B'){
		GetCloudsAudioEvents()->respawn = true;
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
    if(LUKEDEBUG) cout << "===============" << endl;
    if(LUKEDEBUG) cout << "INTERLUDE MUSIC!!!" << endl;
    if(LUKEDEBUG) cout << "===============" << endl;

    if(LUKEDEBUG) cout << "TOTAL DURATION: " << cue.duration+5.0 << endl;
    else cout << "SOUND: MUSIC STARTED." << endl;
	
#ifdef RTCMIX
	sound.startMusicFX(0, cue.duration+5.0);
#endif
	
    if(LUKEDEBUG) cout << "   preset: " << interludePreset.slotnumber << endl;
	sound.schedulePreset(interludePreset, cue.startTime, cue.duration, cue.mixLevel, 0);

    if(LUKEDEBUG) cout << "====================" << endl;
    if(LUKEDEBUG) cout << "DONE INTERLUDE MUSIC!!!" << endl;
    if(LUKEDEBUG) cout << "====================" << endl;

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

}

void CloudsPlaybackController::mouseReleased(ofMouseEventArgs & args){
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::update(ofEventArgs & args){

	///TESTING HACK
//	updateInterludeInterface();
	/////

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
		if(introSequence->isStartQuestionSelected()){
			
			CloudsPortal* q = introSequence->getSelectedQuestion();
			CloudsClip& clip = q->clip;
			
			map<string,string> questionsAndTopics = clip.getAllQuestionTopicPairs();
			if(questionsAndTopics.size() > 0){
				transitionController.transitionFromIntro(1.0);
			}
			else{
				ofLogError("CloudsPlaybackController::update") << "Somehow selected an intro question with no topics " << clip.getLinkName();
			}
		}
	}
	
	////////////////////
	//CLUSTER MAP
	else if(showingClusterMap){
		
		//TODO add questions or something to the cluster map
		//right now we can just have a canned animation and stop it when we are done
        
        //MIKE
        //if(HUD->continue: go to next act
        //else if hud reset: go to next intro
		if(!clusterMap->getTimeline()->getIsPlaying()){
            sound.stopMusic();
#ifdef OCULUS_RIFT
            transitionController.transitionToIntro(1.0);
#else
            transitionController.transitionFromInterlude(1.0);
#endif
			cout << "TRANSITIONING FROM CLUSTER MAP IN UPDATE" << endl;
            showingClusterMap = false;
		}
	}
    
    ////////////////////
	//INTERLUDE
	else if(showingInterlude){

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
    }
    
	////////////////////
	// RGBD SYSTEM
    if(currentVisualSystem == rgbdVisualSystem){
        if(!transitionController.isTransitioning() && !bQuestionAsked && rgbdVisualSystem->isQuestionSelected()){
            
            bQuestionAsked = true;
            
            transitionController.transitionWithQuestion(2.0, 0.1);

        }
		
		if(returnToIntro){
			returnToIntro = false;
			transitionController.transitionToIntro(1.0);
		}
    }
    
	if(!showingClusterMap && !showingInterlude){
		hud.update();
	}
	
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
void CloudsPlaybackController::updateTransition(){
	
	transitionController.update();
    
    //if(transitionController.getCurrentState() != TRANSITION_IDLE){
    //    cout << "CURRENT STATE IS " << transitionController.getCurrentStateDescription() << " PREVIOUS STATE IS " << transitionController.getPreviousStateDescription() <<  " CROSSFADE IS " << crossfadeValue << endl;
    //	}
    
	CloudsPortal* q;
	CloudsClip clip;
	string topic;
	if(transitionController.isStateNew()){
        
        if(transitionController.fadedOut() && shouldClearAct){
            shouldClearAct = false;
            clearAct();
        }
        
        switch (transitionController.getCurrentState()) {
                
            case TRANSITION_INTERVIEW_OUT:
                if(bQuestionAsked){
                    currentAct->getTimeline().stop();
					rgbdVisualSystem->startTransitionOut( CloudsVisualSystem::QUESTION );
				}
				else{
                    rgbdVisualSystem->startTransitionOut( currentVisualSystem->getTransitionType() );
				}
                
                hud.animateOff(CLOUDS_HUD_FULL);
                
                break;
                
            case TRANSITION_INTRO_OUT:
                
                showingIntro = false;
                break;
                
			case TRANSITION_INTRO_IN:
                
                if(currentVisualSystem == interludeSystem){
                    cleanupInterlude();
                }
                else if(currentVisualSystem == rgbdVisualSystem){
					rgbdVisualSystem->transtionFinished();
                    rgbdVisualSystem->stopSystem();
				}
				
                if(introSequence != NULL){
                    delete introSequence;
                }
                
                run.clear();
                
                introSequence = new CloudsIntroSequence();
                introSequence->setup();
				introSequence->setStartQuestions(startingNodes);
#ifdef OCULUS_RIFT
                introSequence->hud = &hud;
                introSequence->setupHUDGui();
#endif
                introSequence->setDrawToScreen(false);
                
                // TODO: Look into using Intro events for setting bDrawHud, so it works like everything else.
                hud.setHudEnabled(true);
                
                showIntro();
                break;
                
            case TRANSITION_VISUALSYSTEM_IN:
                
                if(transitionController.getPreviousState() == TRANSITION_INTERVIEW_OUT){
                    rgbdVisualSystem->transtionFinished();
                }
                
                playNextVisualSystem();
                break;
                
            case TRANSITION_VISUALSYSTEM_OUT:
                
                // no need to do anything special, the crossfade value will take care of this
                
                //                hud.animateOff(CLOUDS_HUD_FULL);
                
                break;
                
            case TRANSITION_INTERVIEW_IN:
                
                hideVisualSystem();
                showRGBDVisualSystem();
                
                if(transitionController.getPreviousState() == TRANSITION_VISUALSYSTEM_OUT){
                    hud.playCued();
                }
                
                break;
                
            case TRANSITION_INTERLUDE_OUT:
				
                updateCompletedInterlude();
                break;
                
            case TRANSITION_INTERLUDE_IN:
                
				interludeStartTime = ofGetElapsedTimef();
				
                CloudsVisualSystem::getRGBDVideoPlayer().getPlayer().stop();
                
                if(transitionController.getPreviousState() == TRANSITION_INTERVIEW_OUT){
                    rgbdVisualSystem->transtionFinished();
                    rgbdVisualSystem->stopSystem();
                }
                else if(transitionController.getPreviousState() == TRANSITION_VISUALSYSTEM_OUT){
                    hideVisualSystem();
                }
				
				hud.setHomeEnabled(false);
                

                
#ifdef OCULUS_RIFT
                showInterlude();
#else
                //                if(run.actCount == 1){
                //                    showClusterMap();
                //                }
                //                else {
                showInterlude();
                //                }
#endif
				createInterludeSoundQueue();
                
                break;
                
    //////////////////
    ////////IDLE CASES
    ///////////////////
            case TRANSITION_IDLE:

                if(transitionController.getPreviousState() == TRANSITION_INTRO_IN){
                    
                    sound.enterTunnel();
                    
                }
                if(transitionController.getPreviousState() == TRANSITION_INTRO_OUT){
					
					q = introSequence->getSelectedQuestion();
                    clip = q->clip;
                    
                    showingVisualSystem = false;
                    
                    introSequence->stopSystem();
					introSequence->exit();
                    
                    storyEngine.buildAct(run, clip, q->topic, true);
                    
//					sound.exitTunnel();
                }
                else if(transitionController.getPreviousState() == TRANSITION_INTERLUDE_OUT){
                    
                    
                    cachedTransition = true;
                    cachedTransitionType = interludeSystem->getTransitionType();

                    cleanupInterlude();
                    
                    //build the next clip based on the history
					storyEngine.buildAct(run);
					
                    cout<<"IDLE POST TRANSITION INTERLUDE OUT"<<endl;
                }
				else if(transitionController.getPreviousState() == TRANSITION_INTERVIEW_OUT){

					if(bQuestionAsked){
                        
                        q = rgbdVisualSystem->getSelectedQuestion();
                        clip = q->clip;
                        topic = q->topic;
                        
                        rgbdVisualSystem->transtionFinished();
                        rgbdVisualSystem->clearQuestions();
                        rgbdVisualSystem->stopSystem();
                        
                        CloudsVisualSystem::getRGBDVideoPlayer().stop();
                        CloudsVisualSystem::getRGBDVideoPlayer().maxVolume = 1.0;
                        
                        crossfadeValue = 0;
                        
                        storyEngine.buildAct(run, clip, topic);
                        
                        bQuestionAsked = false;
                    }
					
				}
                
                //we just finished fading out of the interview
                else if(transitionController.getPreviousState() == TRANSITION_INTERVIEW_IN){
                    rgbdVisualSystem->transtionFinished();
                }
                
                break;
				
            default:
                break;
        }
	}
    
//    if(abs(crossfadeValue - transitionController.getFadeValue()) > .5){
//        ofLogError("TRANSITION JUMP");
//    }
    
    crossfadeValue = transitionController.getFadeValue();

    //if(bQuestionAsked || shouldLoadAct || loadingAct){
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
bool CloudsPlaybackController::updateInterludeInterface(){
	//hack
//	if(interludeSystem == NULL){
//		interludeSystem = currentVisualSystem;
//	}
	//hack
	
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
	return false;
#else
	
	if(GetCloudsInputX() > interludeSystem->getCanvasWidth() - interludeExitBarWidth){
		if(!interludeHoveringContinue){
			interludeHoveringContinue = true;
			interludeBarHoverStartTime = ofGetElapsedTimef();
			//TODO PLAY SOUND
		}
	}
	else{
		interludeHoveringContinue = false;
	}
	
	if(GetCloudsInputX() < interludeExitBarWidth){
		if(!interludeHoveringReset){
			interludeHoveringReset = true;
			interludeBarHoverStartTime = ofGetElapsedTimef();
			//TODO PLAY SOUND
		}
	}
	else{
		interludeHoveringReset = false;
	}
	
	if(interludeHoveringContinue || interludeHoveringReset){
		interludeBarHoverPercentComplete = ofMap(ofGetElapsedTimef(),
												 interludeBarHoverStartTime,interludeBarHoverStartTime+interludeBarHoverHoldTime,
												 0.0, 1.0, true);
		
		if(interludeBarHoverPercentComplete == 1.0){
			//TODO PLAY SOUND
			
			///one of these will be true
			interludeContinueSelected = interludeHoveringContinue;
			interludeResetSelected = interludeHoveringReset;
			//return false; //hack
			return true;
		}
	}
	else {
		//slowly attenuate it back down
		interludeBarHoverPercentComplete *= 0.995;
		interludeBarHoverPercentComplete = MAX(0.0,interludeBarHoverPercentComplete-.001);
	}
	return false;
#endif
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::updateCompletedInterlude(){
	
	return;
	
	interludeExitBarWidth *= .95;
	interludeExitBarWidth -= 0.01;
	interludeExitBarWidth = MAX(0,interludeExitBarWidth);
	
	interludeArcRadius *= 1.01;
	interludeArcBaseAlpha *= .95;
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::drawInterludeInterface(){
    

	ofRectangle hoverRect;
	bool hovering = false;
	string promptType;
	int tracking;
	if(interludeHoveringContinue){
		hoverRect = ofRectangle(interludeSystem->getCanvasWidth(), 0,
								-interludeExitBarWidth, interludeSystem->getCanvasHeight());
		hovering = true;
		promptType = "CONTINUE";
		tracking = 6;
	}
	else if(interludeHoveringReset){
		hoverRect = ofRectangle(0, 0, interludeExitBarWidth, interludeSystem->getCanvasHeight());
		hovering = true;
		promptType = "RESET";
		tracking = 11;
	}
	
		
	ofPushStyle();
	ofEnableAlphaBlending();

	ofFill();
	float alpha      = ofxTween::map(interludeBarHoverPercentComplete, 0.0, .3, 0.2, 1.0, true, ofxEasingQuad(), ofxTween::easeOut);
	hoverRect.width *= ofxTween::map(interludeBarHoverPercentComplete, 0.0, .2, 0.1, 1.0, true, ofxEasingQuad(), ofxTween::easeOut);
	ofFloatColor arcColor = ofFloatColor(1.0, alpha*powf(crossfadeValue,2.0f) *  .3);
	ofSetColor(arcColor);
	ofRect(hoverRect);
	
	if(hovering){
		
		ofNoFill();
		ofSetColor(255, 100*crossfadeValue);
		ofCircle(GetCloudsInputPosition(), interludeArcRadius*crossfadeValue);
		
		if(interludeBarHoverPercentComplete > 0.0){
			float arcPercent = ofxTween::map(interludeBarHoverPercentComplete, 0.0, 1.0, 0.0, 1.0, true, ofxEasingQuad(), ofxTween::easeOut);
			ofPath arc;
			arc.setFilled(false);
			arc.setStrokeWidth(3);
			arc.setStrokeColor(arcColor);
			float expandedArcRadius = interludeArcRadius + powf(1.0-crossfadeValue,2.0f) * 40; //expand it beyond when it's finished
			
			arc.arc( GetCloudsInputPosition(), expandedArcRadius, expandedArcRadius, -90, 360*arcPercent-90, true);
			arc.draw();
		}
		
		interludeInterfaceFont.setTracking(tracking);
		float typeWidth  = interludeInterfaceFont.stringWidth(promptType);
		float typeHeight = interludeInterfaceFont.stringHeight(promptType);
		
		ofSetColor(255*alpha*crossfadeValue);
		interludeInterfaceFont.drawString(promptType, hoverRect.getCenter().x - typeWidth/2 - 10, GetCloudsInputY() + interludeArcRadius + typeHeight + 10);
	}
	ofPopStyle();
	
}

#ifdef KINECT_INPUT
//--------------------------------------------------------------------
void CloudsPlaybackController::drawKinectFeedback(){
    
    ofPushStyle();
    
    string promptText = "";
    ofPtr<CloudsInputKinectOSC> kinectInput = dynamic_pointer_cast<CloudsInputKinectOSC>(GetCloudsInput());
    if ((kinectInput->viewerState == k4w::ViewerState_OutOfRange) ||
        (kinectInput->viewerState == k4w::ViewerState_PresentIdle && kinectInput->viewerIdleTime >= 5000)) {
        kinectFeedbackAlpha = ofLerp(kinectFeedbackAlpha, 255, 0.3f);
        
        if (kinectInput->viewerState == k4w::ViewerState_OutOfRange) {
            promptText = "MOVE CLOSER TO THE DISPLAY";
        }
        else {
            promptText = "EXTEND YOUR HAND TO BEGIN";
        }
    }
    else {
        kinectFeedbackAlpha = ofLerp(kinectFeedbackAlpha, 0, 0.5f);
    }
    kinectInput->draw(60, ofGetHeight() - 285 - 60, 380, 285, kinectFeedbackAlpha);

    ofSetColor(255, kinectFeedbackAlpha);
    float textWidth  = kinectFeedbackFont.stringWidth(promptText);
    float textHeight = kinectFeedbackFont.stringHeight(promptText);
    kinectFeedbackFont.drawString(promptText, (ofGetWidth() - textWidth) / 2, (ofGetHeight() - textHeight) / 2);
    
    ofPopStyle();
    
}
#endif

//This is where everything in clouds is drawn
//--------------------------------------------------------------------
void CloudsPlaybackController::draw(ofEventArgs & args){
    
    ofBackground(0);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_DEPTH_TEST);
	
    drawRenderTarget();
    
    drawInterludeInterface();
    
#ifdef KINECT_INPUT
    drawKinectFeedback();
#endif
    
	drawDebugOverlay();
	
	if(loadingAct){
		float progressWidth = ofGetWidth() * .66;
		//		loadingFont.drawString("Generating a response to your query.", ofGetWidth()*.33, ofGetHeight()*.5 - 5);
		ofRect(ofGetWidth()*.33 / 2, ofGetHeight()*.5, progressWidth * currentPresetIndex / currentAct->getAllVisualSystemPresets().size(), 50. );
	}
	
	glPopAttrib();
}

void CloudsPlaybackController::drawRenderTarget(){
	if(currentVisualSystem != NULL){
		
        ofPushStyle();
        ofEnableAlphaBlending();
        
		//cout << "crosffade value is " << crossfadeValue << " showing intro? " << showingIntro << endl;
		if(loading){
			crossfadeValue = 1.0;
		}
		ofSetColor(255, crossfadeValue*255 );
		
		currentVisualSystem->selfPostDraw();
        
		//TODO Make parametric based on HUD
        CloudsVisualSystem::getRGBDVideoPlayer().drawSubtitles(CloudsVisualSystem::getStaticRenderTarget().getWidth()/2.0,
                                                               CloudsVisualSystem::getStaticRenderTarget().getHeight()*0.8);
        
#ifndef OCULUS_RIFT
		hud.draw();
#endif
		ofPopStyle();
	}
    else{
#ifdef CLOUDS_APP
		ofBackground(0, 0, 0);
#else
        ofBackground(0, 0, 255);
#endif
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
    
	numClipsPlayed = 0;
	shouldLoadAct = true;
	shouldPlayAct = false;
	currentAct = args.act;
	
	cout << "***** ORDER OF OPERATIONS: ACT CRATED CONTROLLER " << args.act << endl;

}

//--------------------------------------------------------------------
void CloudsPlaybackController::actBegan(CloudsActEventArgs& args){
    cout << "***** ORDER OF OPERATIONS: ACT BEGAN CONTROLLER " << args.act << endl;
	
	resetInterludeVariables();
	
    if(!args.act->startsWithVisualSystem()){
        transitionController.transitionToFirstInterview(1.0);
    }
}

//--------------------------------------------------------------------
void CloudsPlaybackController::actEnded(CloudsActEventArgs& args){
	
	if(!returnToIntro){
		shouldClearAct = true;
		
		cout << "ACT ENDED TRIGGERED" << endl;
		if(!bQuestionAsked){
			cout << "Transitioning to interlude/clustermap" << endl;
			transitionController.transitionToInterlude(1.0,1.0);
		}
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
    
	//rgbdVisualSystem->clearQuestions();
    
	nextVisualSystemPreset = args.preset;
    
	//	cout << "CloudsPlaybackController::showVisualSystem SETTING NEXT SYSTEM TO " << nextVisualSystem.presetName << endl;
	if(nextVisualSystemPreset.system == NULL){
		ofLogError("CloudsPlaybackController::showVisualSystem") << "Incoming system is NULL";
	}
	
	if(currentVisualSystem == introSequence ||
	   currentVisualSystem == clusterMap ||
       exitedInterlude)
	{
		transitionController.transitionToFirstVisualSystem(1.0);
        exitedInterlude = false;
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
		float fadeDuration = 1;
		transitionController.transitionToInterview(fadeDuration, 1.0);
	}
	else {
		ofLogError("CloudsPlaybackController::visualSystemEnded") << "Hiding visual system while none is showing";
	}
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
void CloudsPlaybackController::prerollClip(CloudsClip& clip, float toTime){
	if(!clip.hasMediaAsset){
		ofLogError("CloudsPlaybackController::prerollClip") << "clip " << clip.getLinkName() << " doesn't have combined video";
		return;
	}
	
	bool clipLoadSuccessfullyLoaded = false;
	if(clip.voiceOverAudio){
		clipLoadSuccessfullyLoaded = CloudsVisualSystem::getRGBDVideoPlayer().setupVO(clip.voiceOverAudioPath);
	}
	else{
		clipLoadSuccessfullyLoaded = CloudsVisualSystem::getRGBDVideoPlayer().setup(clip.combinedVideoPath,
																					clip.combinedCalibrationXMLPath,
                                                                                    clip.combinedSRTPath,
																					toTime,clip.getSpeakerVolume());
	}
    
	if(!clipLoadSuccessfullyLoaded){
		ofLogError("CloudsPlaybackController::prerollClip") << "Error loading clip " << clip.getLinkName() << " file path " << clip.combinedVideoPath;
		return;
	}
    
	prerolledClipID = clip.getID();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::playClip(CloudsClip clip){
    
	numClipsPlayed++;
	
    //	rgbdVisualSystem->clearQuestions();
	if(clip.getID() != prerolledClipID){
		prerollClip(clip, 1);
	}
	
	rgbdVisualSystem->setupSpeaker( clip.getSpeakerFirstName(),clip.getSpeakerLastName(), clip.name);
	
	prerolledClipID = "";
	currentClip = clip;
	currentClipName = clip.getID();
    
	rgbdVisualSystem->getRGBDVideoPlayer().swapAndPlay();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showClusterMap(){
    
    clusterMap->traverse();
    clusterMap->loadPresetGUISFromName("JG_SIMPLECAM");
    clusterMap->playSystem();
    
    currentVisualSystem = clusterMap;
    
    showingClusterMap = true;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showInterlude(){
    
	resetInterludeVariables();
	
    vector<string> topics;
    CloudsVisualSystemPreset interludePreset;
    if(storyEngine.getPresetIDForInterlude(run, interludePreset)){
        
        interludeSystem = CloudsVisualSystemManager::InstantiateSystem(interludePreset.systemName);
        
        interludeSystem->setDrawToScreen( false );
        interludeSystem->setup();
        interludeSystem->loadPresetGUISFromName( interludePreset.presetName );
        interludeSystem->playSystem();
        interludeSystem->isInterlude = true;
		
        currentVisualSystem = interludeSystem;
        
        showingInterlude = true;
    }
    else{
        ofLogError("CloudsPlaybackController::showInterlude") << "Defaulting to cluster map because we found no topics from the last act";
        showClusterMap();
    }
}

//--------------------------------------------------------------------
void CloudsPlaybackController::cleanupInterlude(){
    showingVisualSystem = false;
    
    if(currentVisualSystem == clusterMap) {
        clusterMap->stopSystem();
    }
    else if(currentVisualSystem != NULL && currentVisualSystem == interludeSystem){
        interludeSystem->stopSystem();
        interludeSystem->exit();
        exitedInterlude = true;
    }
    else {
        ofLogError("CloudsPlaybackController::updateTransition") << " Ended interulde while not showing ClusterMap or Interlude System";
    }
}

//--------------------------------------------------------------------
void CloudsPlaybackController::hideVisualSystem() {
	if(showingVisualSystem){
		currentVisualSystem->stopSystem();
		showingVisualSystem = false;
	}
}

void CloudsPlaybackController::showRGBDVisualSystem(){
#ifdef OCULUS_RIFT
    //	rgbdVisualSystem->loadPresetGUISFromName("RGBDOC");
    //    if(run.actCount == 1){
    rgbdVisualSystem->loadPresetGUISFromName("RGBD_OC_POINTS");
    //    }
    //    else{
    //        rgbdVisualSystem->loadPresetGUISFromName("RGBD_OC_LINES");
    //    }
#else
    if(numActsCreated == 0){
        rgbdVisualSystem->loadPresetGUISFromName("RGBD_NewAct1");
    }
    else if(numActsCreated == 1){
        rgbdVisualSystem->loadPresetGUISFromName("RGBD_ACT2_LINES_NEW");
    }
    else{
        rgbdVisualSystem->loadPresetGUISFromName("RGBD_ACT3_MESH_NEW");
    }
#endif
    
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
	
	hud.setHomeEnabled(true);
	
	currentVisualSystem = rgbdVisualSystem;
}

void CloudsPlaybackController::playNextVisualSystem()
{
	if(nextVisualSystemPreset.system != NULL){
		
		if(rgbdVisualSystem->isPlaying()){
			rgbdVisualSystem->stopSystem();
		}
		
		nextVisualSystemPreset.system->setDrawToScreen( false );
		nextVisualSystemPreset.system->loadPresetGUISFromName( nextVisualSystemPreset.presetName );
		nextVisualSystemPreset.system->playSystem();
		
		currentVisualSystemPreset = nextVisualSystemPreset;
		currentVisualSystem = nextVisualSystemPreset.system;
		
        cachedTransition = false;
        
		showingVisualSystem = true;
	}
	else{
		ofLogError("CloudsPlaybackController::playNextVisualSystem") << "nextVisualSystemPreset == NULL";
	}
}

#pragma mark - Visual System Event Callbacks

//--------------------------------------------------------------------
void CloudsPlaybackController::portalHoverBegan(CloudsPortalEventArgs &args){
    hud.questionHoverOn(args.question);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::portalHoverEnded(CloudsPortalEventArgs &args){
	hud.questionHoverOff();
	if(args.question == "RESET"){
		returnToIntro = true;
		CloudsVisualSystem::getRGBDVideoPlayer().stop();
		currentAct->getTimeline().stop();		
	}
}

//void CloudsPlaybackController::clearRestButtonParams(){
//    bResetSelected = false;
//    prevResetValue = false;
////    resetSelectedPercentComplete = 0;
//    bResetTransitionComplete = false;
//}
//

