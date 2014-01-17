
#include "CloudsPlaybackController.h"
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
    
    returnToIntro = false;
    
    cachedTransition = false;

    lastMouseMoveMillis = 0;
}

//--------------------------------------------------------------------
CloudsPlaybackController::~CloudsPlaybackController(){
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
	clearAct();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::setup(){
	
	///START THREADED
    parser.loadFromFiles();
	
	if(!ofFile::doesFileExist(GetCloudsDataPath() + "CloudsMovieDirectory.txt")){
		ofSystemAlertDialog("Could not find movie file path. \
							Create a file called CloudsMovieDirectory.txt \
							that contains one line, the path to your movies folder");
	}
	
	parser.setCombinedVideoDirectory(ofBufferFromFile(GetCloudsDataPath() + "CloudsMovieDirectory.txt").getText());
	
	visualSystems.loadPresets();
    visualSystems.loadCachedDataForSystems();

	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	storyEngine.printDecisions = false;
	storyEngine.combinedClipsOnly = true;
	storyEngine.setup();
	
//    vector< pair<int,string> > topicCountPairs;
//    for(int i = 0; i < parser.getAllKeywords().size(); i++){
//        topicCountPairs.push_back(make_pair(parser.getClipsWithKeyword( parser.getAllKeywords()[i]).size(),
//                                            parser.getAllKeywords()[i] ) );
//    }
//
//    sort(topicCountPairs.begin(), topicCountPairs.end(), listsort);
//    ofBuffer b;
//    
//    for(int i = 0; i < topicCountPairs.size(); i++){
//        b.append( ofToString(topicCountPairs[i].first) + "\t" + topicCountPairs[i].second + "\n" );
//    }
//    ofBufferToFile(GetCloudsDataPath() + "logs/topics.txt", b);
    
	///SOUND
	mixer.setup();
	sound.setup(storyEngine);
    

#ifndef  OCULUS_RIFT
	////COMMUNICATION
	oscSender.setup();
#endif
    
	//END THREADED
	
	rgbdVisualSystem = new CloudsVisualSystemRGBD();
	rgbdVisualSystem->setup();
	rgbdVisualSystem->setDrawToScreen(false);
	
	clusterMap = new CloudsVisualSystemClusterMap();
	clusterMap->setRun(run);
	clusterMap->setup();
	clusterMap->buildEntireCluster(parser);
	clusterMap->setDrawToScreen(false);
	
	introSequence = new CloudsIntroSequence();
	introSequence->setup();
	introSequence->setDrawToScreen(false);
	
	hud.setup();
#ifdef OCULUS_RIFT
    rgbdVisualSystem->hud = &hud;
    rgbdVisualSystem->setupHUDGui();

    introSequence->hud = &hud;
    introSequence->setupHUDGui();
#endif
    
    if(!eventsRegistered){
		
		eventsRegistered = true;
		
        ofAddListener(storyEngine.getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
        
		ofAddListener(ofEvents().update, this, &CloudsPlaybackController::update);
		ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
        
        ofAddListener(CloudsIntroSequence::events.portalHoverBegan, this, &CloudsPlaybackController::portalHoverBegan);
        ofAddListener(CloudsIntroSequence::events.portalHoverEnded, this, &CloudsPlaybackController::portalHoverEnded);
        
        ofAddListener(CloudsVisualSystemRGBD::events.portalHoverBegan, this, &CloudsPlaybackController::portalHoverBegan);
        ofAddListener(CloudsVisualSystemRGBD::events.portalHoverEnded, this, &CloudsPlaybackController::portalHoverEnded);
        
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
	}
	
    
	//////////////SHOW INTRO
    startingNodes = parser.getClipsWithKeyword("#start");
	//safe guard delete any starters that don't have questions
	for(int i = startingNodes.size()-1; i >= 0; i--){
		if(!startingNodes[i].hasQuestion() ) {
			ofLogError() << "Clip " << startingNodes[i].getID() << " is labeled as #start but has no question, removing.";
			startingNodes.erase(startingNodes.begin() + i);
		}
		else if(!startingNodes[i].hasMediaAsset){
			ofLogError() << "Clip " << startingNodes[i].getID() << " has no media asset, removing.";
			startingNodes.erase(startingNodes.begin() + i);
		}
#ifdef OCULUS_RIFT
        else if(!startingNodes[i].hasSpecialKeyword("#oculus")){
			ofLogError() << "Clip " << startingNodes[i].getID() << " is not tagged for the oculus.";
			startingNodes.erase(startingNodes.begin() + i);
        }
#endif
//        else if(ofToLower( startingNodes[i].getQuestions()[0]) != "what does music look like?"){
//			startingNodes.erase(startingNodes.begin() + i);            
//        }
		else{
            //			cout << " Adding Clip " << startingNodes[i].getID() << " with question " << startingNodes[i].getQuestions()[0] << endl;
		}
	}
    
	//////////////SHOW INTRO
	cout << "Found " << startingNodes.size() << " questions" << endl;
	showIntro( startingNodes );
}

//--------------------------------------------------------------------
CloudsRGBDVideoPlayer& CloudsPlaybackController::getSharedVideoPlayer(){
	return CloudsVisualSystem::getRGBDVideoPlayer();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showIntro(vector<CloudsClip>& possibleStartQuestions){
	introSequence->setStartQuestions(possibleStartQuestions);
	showIntro();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showIntro(){
#ifdef OCULUS_RIFT
	introSequence->loadPresetGUISFromName("Oculus");
#else
	introSequence->loadPresetGUISFromName("TunnelWarp");
#endif
	introSequence->playSystem();
	
	currentVisualSystem = introSequence;
	
	sound.enterTunnel();
	numActsCreated = 0;
    
	showingVisualSystem = true;
	showingIntro = true;
	hud.setHomeEnabled(false);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::playAct(CloudsAct* act){
    
//	currentAct = act;
    
	//TODO: show loading screen while we initialize all the visual systems
	vector<CloudsVisualSystemPreset>& presets = currentAct->getAllVisualSystemPresets();
	vector< CloudsVisualSystem* > systems = CloudsVisualSystemManager::InstantiateSystems(presets);
	for(int i = 0; i < presets.size(); i++){
		if(presets[i].system != NULL){
            //			cout << "CloudsPlaybackController::playAct -- Setting up:: " << presets[i].systemName << endl;
			presets[i].system->setup();
		}
		else{
			ofLogError("CloudsPlaybackController::playAct") << presets[i].systemName << " NULL right after instantiaton. correlating system null? " << (systems[i] == NULL ? "YES" : "NO");
		}
	}
	
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
            
            //cancel the act
            //currentAct->getTimeline().setCurrentTimeSeconds( currentAct->getTimeline().getDurationInSeconds()-3 );
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
	sound.startMusicFX(0, cue.duration);
	sound.schedulePreset(interludePreset, cue.startTime, cue.duration, cue.mixLevel);

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
//#ifdef OCULUS_RIFT
    // EZ: Override CloudsInputSystem just to get the thing started
    // since we can't click with Oculus input.
//    if (introSequence) {
//        introSequence->selfMousePressed(args);
//    }
//#endif
}

void CloudsPlaybackController::mouseReleased(ofMouseEventArgs & args){
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::update(ofEventArgs & args){
    ////////////////////
	//OS CURSOR
#ifdef CLOUDS_RELEASE
    ofHideCursor();
#else
    currMousePos.set(ofGetMouseX(), ofGetMouseY());
    if (currMousePos != lastMousePos) {
        lastMouseMoveMillis = ofGetElapsedTimeMillis();
        ofShowCursor();
    }
    else if ((ofGetElapsedTimeMillis() - lastMouseMoveMillis) > 1000) {
        ofHideCursor();
    }
    lastMousePos = currMousePos;
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
        bool stopInterlude = false;
        bool goToNextAct = false;
        if(GetSelectedInterludePortalContinue() || !interludeSystem->getTimeline()->getIsPlaying()){
            stopInterlude = true;
            goToNextAct = true;
        }
        else if(GetSelectedInterludePortalResetClouds()){
            stopInterlude = true;
            goToNextAct = false;
        }
		else if(ofGetElapsedTimef() - interludeStartTime > 2*60){
            stopInterlude = true;
            goToNextAct = false;
		}
        
#ifdef OCULUS_RIFT
        goToNextAct = false;
#endif
        if(stopInterlude){
         
            sound.stopMusic();
            if(goToNextAct){
                transitionController.transitionFromInterlude(1.0);
            }
            else{
                transitionController.transitionToIntro(1.0);
            }
            ShowInterludePortals(false);
            showingInterlude = false;
        }
    }


	////////////////////
	// RGBD SYSTEM
    //	if(rgbdVisualSystem->isQuestionSelectedAndClipDone()){
    if(currentVisualSystem == rgbdVisualSystem){
        if(!bQuestionAsked && rgbdVisualSystem->isQuestionSelected()){
            
            bQuestionAsked = true;

            transitionController.transitionWithQuestion(2.0, 0.1);
            sound.questionSelected(2.0);
        }
    }
    
    
	if(!showingClusterMap && !showingInterlude){
		hud.update();
	}
	
	if(shouldPlayAct){
		playAct(currentAct);
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
                
                if(introSequence != NULL){
                    delete introSequence;
                }
                
                run.clear();
                
                introSequence = new CloudsIntroSequence();
                introSequence->setup();
#ifdef OCULUS_RIFT
                introSequence->hud = &hud;
                introSequence->setupHUDGui();
#endif
                introSequence->setDrawToScreen(false);
                
                // TODO: Look into using Intro events for setting bDrawHud, so it works like everything else.
                hud.setHudEnabled(true);

                showIntro(startingNodes);
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
				
				/// wait for it to fade out...
//				showingClusterMap = false;
//                showingInterlude = false;
                
                break;

            case TRANSITION_INTERLUDE_IN:
                
				interludeStartTime = ofGetElapsedTimef();
				
                CloudsVisualSystem::getRGBDVideoPlayer().getPlayer().stop();
                
                if(transitionController.getPreviousState() == TRANSITION_INTERVIEW_OUT){
                    rgbdVisualSystem->transtionFinished();
                    rgbdVisualSystem->stopSystem();
                    //Call stop on the clip?
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
                
//			case TRANSITION_QUESTION_IN:
//				
//                currentAct->getTimeline().stop();
//                
//				// show question transition over this period
//				rgbdVisualSystem->transtionFinished();
//				rgbdVisualSystem->stopSystem();
//                
//                break;
                
    //////////////////
    ////////IDLE CASES
    ///////////////////
            case TRANSITION_IDLE:
                
                if(transitionController.getPreviousState() == TRANSITION_INTRO_OUT){
					
					q = introSequence->getSelectedQuestion();
                    clip = q->clip;
                    
                    showingVisualSystem = false;
                    
                    introSequence->stopSystem();
					introSequence->exit();
                    
                    storyEngine.buildAct(run, clip, q->topic, true);
                    
					sound.exitTunnel();
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

//This is where everything in clouds is drawn
//--------------------------------------------------------------------
void CloudsPlaybackController::draw(ofEventArgs & args){
    
    ofBackground(0);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable( GL_DEPTH_TEST );
	
	if(currentVisualSystem != NULL){
		ofPushStyle();
        
		//cout << "crosffade value is " << crossfadeValue << " showing intro? " << showingIntro << endl;
		ofSetColor(255, crossfadeValue*255 );
		
		currentVisualSystem->selfPostDraw();
        
#ifdef SHOW_SUBTITLES
        CloudsVisualSystem::getRGBDVideoPlayer().drawSubtitles(CloudsVisualSystem::getStaticRenderTarget().getWidth()/2,
                                                               (float)CloudsVisualSystem::getStaticRenderTarget().getHeight()*0.8);
#endif
        
#ifndef OCULUS_RIFT
		hud.draw();
#endif
        
		ofPopStyle();
	}
    else{
        ofBackground(0, 0, 255);
    }
	
	drawDebugOverlay();
	
	glPopAttrib();
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
	
	if(showingVisualSystem && ofGetKeyPressed('k')){
		ofPushMatrix();
		ofPushStyle();
		ofEnableAlphaBlending();
		ofTranslate(ofGetWidth()*.5, ofGetHeight()*.5);
		ofScale(7,7);
		ofSetColor(255);
		string debugString = "";
//		currentVisualSystemPreset.presetName + " was associated with keyword " + currentVisualSystemPreset.conjureKeyword + "\n" +
//        "Preset's keywords " + ofJoinString(currentVisualSystemPreset.allKeywords, ", ") + "\n" +
//        "current clip's keywords " + ofJoinString(currentClip.getKeywords(), ", ") + "\n" +
//        "Had to default to keyword family? " + (currentVisualSystemPreset.defaultedToFamily ? "YES" : "NO") + "\n" +
//        "Had to pick a random preset? " + (currentVisualSystemPreset.randomlySelected ? "YES" : "NO") + "\n" +
//        "Act #? " + ofToString(run.actCount);
		
		ofDrawBitmapString(debugString, 0,0);
		
		ofSetColor(0,0,0,255);
		ofRect(-5,-5,300, 50);
		
		ofPopStyle();
		ofPopMatrix();
	}
}

#pragma story engine events
//--------------------------------------------------------------------
void CloudsPlaybackController::actCreated(CloudsActEventArgs& args){
	
    if(currentAct != NULL){
        clearAct();
    }
    
	numClipsPlayed = 0;
	shouldPlayAct = true;
	currentAct = args.act;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::actBegan(CloudsActEventArgs& args){

    if(!args.act->startsWithVisualSystem()){
        transitionController.transitionToFirstInterview(1.0);
    }
}

//--------------------------------------------------------------------
void CloudsPlaybackController::actEnded(CloudsActEventArgs& args){
	
    //make sure its' stopped
//    CloudsVisualSystem::getRGBDVideoPlayer().getPlayer().stop();
    shouldClearAct = true;
    
    if(returnToIntro){
        returnToIntro = false;
        transitionController.transitionToIntro(1.0);
    }
    else{
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
	if(!ofContains(run.topicHistory, args.topic)) {
		rgbdVisualSystem->addQuestion(args.questionClip, args.topic, args.question);
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
    
    vector<string> topics;
    CloudsVisualSystemPreset interludePreset;
    
    if(storyEngine.getPresetIDForInterlude(run, interludePreset)){
        
        interludeSystem = CloudsVisualSystemManager::InstantiateSystem(interludePreset.systemName);
        
        interludeSystem->setDrawToScreen( false );
        interludeSystem->setup();
        interludeSystem->loadPresetGUISFromName( interludePreset.presetName );
        interludeSystem->playSystem();
        
        currentVisualSystem = interludeSystem;
        
        showingInterlude = true;
        
        ShowInterludePortals(true);
    }
    else{
        ofLogError("CloudsPlaybackController::showInterlude") << "Defaulting to cluster map because we found no topics from the last act";
        showClusterMap();
    }
}

//--------------------------------------------------------------------
void CloudsPlaybackController::cleanupInterlude(){
    showingVisualSystem = false;
    
    ResetInterludePortals();
    
    if(currentVisualSystem == clusterMap) {
        clusterMap->stopSystem();

    }
    else if(currentVisualSystem == interludeSystem){
        interludeSystem->stopSystem();
        interludeSystem->exit();
        exitedInterlude = true;
    }
    else {
        ofLogError("CloudsPlaybackController::updateTransition") << " Ended interulde while not showing ClusterMap or Interlude System";
    }
//    currentVisualSystem = NULL;
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
        rgbdVisualSystem->loadPresetGUISFromName("RGBD_ACT1_POINTS");
    }
    else if(numActsCreated == 1){
        rgbdVisualSystem->loadPresetGUISFromName("RGBD_ACT2_LINES");
    }
    else{
        rgbdVisualSystem->loadPresetGUISFromName("RGBD_ACT3_MESH");
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
}

