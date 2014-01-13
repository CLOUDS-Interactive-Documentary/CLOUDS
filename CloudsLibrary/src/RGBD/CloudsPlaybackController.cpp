
#include "CloudsPlaybackController.h"

//--------------------------------------------------------------------
CloudsPlaybackController::CloudsPlaybackController(){
	eventsRegistered = false;
	showingVisualSystem = false;
	currentAct = NULL;
	showingClusterMap = false;
	shouldPlayAct = false;
	bQuestionAsked = false;
}

//--------------------------------------------------------------------
CloudsPlaybackController::~CloudsPlaybackController(){
}

//--------------------------------------------------------------------
void CloudsPlaybackController::clearAct(bool destroyAct){
	
	if(currentAct == NULL){
		return;
	}
	
	vector<CloudsVisualSystemPreset>& currentPresets = currentAct->getAllVisualSystemPresets();
	for(int i = 0; i < currentPresets.size(); i++){
		//flag them done!
		if(currentPresets[i].system != NULL){
			if(currentPresets[i].system->isPlaying()){
				currentPresets[i].system->stopSystem();
				ofLogError("CloudsPlaybackController::clearAct") << "System " << currentPresets[i].getID() << " Was still playing!";
			}
			currentPresets[i].system->exit();
		}
	}
	
    visualSystems.freeSystemPointers();
    
	currentAct->unregisterEvents(this);
	currentAct->unregisterEvents(&run);
	currentAct->unregisterEvents(&hud);
	currentAct->unregisterEvents(&oscSender);
	
	if(destroyAct){
		delete currentAct;
		currentAct = NULL;
	}
    
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
	
	
	///SOUND
	mixer.setup();
	sound.setup(storyEngine);
    
	////COMMUNICATION
	oscSender.setup();
	//END THREADED
	
	if(!eventsRegistered){
		
		eventsRegistered = true;
		
        ofAddListener(storyEngine.getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
        
		ofAddListener(ofEvents().update, this, &CloudsPlaybackController::update);
		ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
	}
	
	rgbdVisualSystem = ofPtr<CloudsVisualSystemRGBD>( new CloudsVisualSystemRGBD() );
	rgbdVisualSystem->setup();
	rgbdVisualSystem->setDrawToScreen(false);
	
	
	clusterMap = ofPtr<CloudsVisualSystemClusterMap>( new CloudsVisualSystemClusterMap() );
	clusterMap->setRun(run);
	clusterMap->setup();
	clusterMap->buildEntireCluster(parser);
	clusterMap->setDrawToScreen(false);
	
	introSequence = ofPtr<CloudsIntroSequence>( new CloudsIntroSequence() );
	introSequence->setup();
	introSequence->setDrawToScreen(false);
	
	hud.setup();
#ifdef OCULUS_RIFT
    rgbdVisualSystem->hud = &hud;
    rgbdVisualSystem->setupHUDGui();
#endif
	
	//////////////SHOW INTRO
	vector<CloudsClip> startingNodes = parser.getClipsWithKeyword("#start");
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
		else{
            //			cout << " Adding Clip " << startingNodes[i].getID() << " with question " << startingNodes[i].getQuestions()[0] << endl;
		}
	}
	//////////////SHOW INTRO
	
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
	
	showingVisualSystem = true;
	showingIntro = true;
	hud.setHomeEnabled(false);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::playAct(CloudsAct* act){
    
	currentAct = act;
    
	//TODO: show loading screen while we initialize all the visual systems
	vector<CloudsVisualSystemPreset>& presets = currentAct->getAllVisualSystemPresets();
	vector< ofPtr<CloudsVisualSystem> > systems = CloudsVisualSystemManager::InstantiateSystems(presets);
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
		if(currentVisualSystem == introSequence){
			introSequence->autoSelectQuestion();
		}
	}
    
    if(args.key == 'l'){
        transitionController.transitionFromClusterMap(1.0);
    }
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
#ifdef OCULUS_RIFT
    // EZ: Override CloudsInputSystem just to get the thing started
    // since we can't click with Oculus input.
    if (introSequence) {
        introSequence->selfMousePressed(args);
    }
#endif
}

void CloudsPlaybackController::mouseReleased(ofMouseEventArgs & args){
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::update(ofEventArgs & args){
    
	////////////////////
	//INTRO
	if(showingIntro){
		string questionText = introSequence->getQuestionText();
		if(questionText != ""){
			hud.questionHoverOn(questionText);
		}
		else{
			hud.questionHoverOff();
		}
		
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
			
			transitionController.transitionFromClusterMap(1.0);
			
			cout << "TRANSITIONING FROM CLUSTER MAP IN UPDATE" << endl;
            
		}
	}
    
	////////////////////
	// RGBD SYSTEM
    //	if(rgbdVisualSystem->isQuestionSelectedAndClipDone()){
    if(currentVisualSystem == rgbdVisualSystem){
        string questionText = rgbdVisualSystem->getQuestionText();
        if(questionText != ""){
            hud.questionHoverOn(questionText);
        }
        else{
            hud.questionHoverOff();
        }
        
        if(!bQuestionAsked && rgbdVisualSystem->isQuestionSelected()){
            
            bQuestionAsked = true;
            //currently QUESTION_IN is very short, placeholder for wormhole
            transitionController.transitionWithQuestion(2.0, 0.1);
            sound.questionSelected(2.0);
        }
    }
    
    
	if(!showingClusterMap){
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
	
    //TODO: Stop snapping on crossfade value
	crossfadeValue = transitionController.getFadeValue();
	rgbdVisualSystem->visualSystemFadeValue = crossfadeValue;
	
	if(transitionController.getCurrentState() != TRANSITION_IDLE){
//		cout << "CURRENT STATE IS " << transitionController.getCurrentStateDescription() << " CROSSFADE IS " << crossfadeValue << endl;
	}
	
	if(transitionController.transitioning){
		rgbdVisualSystem->updateTransition( transitionController.getInterviewTransitionPoint() );
	}
	
	CloudsPortal* q;
	CloudsClip clip;
	string topic;
	vector<string> topics;
    vector<CloudsVisualSystemPreset> potentialPresets;
    
	if(transitionController.isStateNew()){
        
        switch (transitionController.getCurrentState()) {
                
            case TRANSITION_INTERVIEW_OUT:
                if(bQuestionAsked){
					rgbdVisualSystem->startTransitionOut( CloudsVisualSystem::QUESTION );
				}
				else{
					rgbdVisualSystem->startTransitionOut( currentVisualSystem->getTransitionType() );
				}
                break;
                
            case TRANSITION_INTRO_OUT:
                
                showingIntro = false;
                break;
                
            case TRANSITION_VISUALSYSTEM_IN:
                
                if(transitionController.getPreviousState() == TRANSITION_INTERVIEW_OUT){
                    rgbdVisualSystem->transtionFinished();
                }
                
                playNextVisualSystem();
                break;
                
            case TRANSITION_VISUALSYSTEM_OUT:
                
                // no need to do anything special, the crossfade value will take care of this
                
                break;
                
            case TRANSITION_INTERVIEW_IN:
                
                hideVisualSystem();
                showRGBDVisualSystem();
                
                break;
                
            case TRANSITION_CLUSTERMAP_OUT:
				
				/// wait for it to fade out...
				showingClusterMap = false;
                
				
                break;

                
            case TRANSITION_CLUSTERMAP_IN:
                
                sound.enterClusterMap();
                
                if(transitionController.getPreviousState() == TRANSITION_INTERVIEW_OUT){
                    rgbdVisualSystem->transtionFinished();
                    rgbdVisualSystem->stopSystem();
                }
                else if(transitionController.getPreviousState() == TRANSITION_VISUALSYSTEM_OUT){
                    hideVisualSystem();
                }
				
				hud.setHomeEnabled(false);
                
                
                clearAct(true);
                
                topics = storyEngine.getValidTopicsForNextAct(run);
                potentialPresets = visualSystems.getPresetsForKeywords(topics,"",true);
                if(potentialPresets.size() == 0 ){
                    cout<<"Couldnt find any presets for the next interlude! Using Cluster Map instead"<<endl;
                    clusterMap->traverse();
                    clusterMap->loadPresetGUISFromName("JG_SIMPLECAM");
                    clusterMap->playSystem();
                    
                    currentVisualSystem = clusterMap;
                    
                    showingClusterMap = true;
                }
                else{
                    CloudsVisualSystemPreset interlude = potentialPresets[ofRandom(potentialPresets.size())];
                    interlude.system = CloudsVisualSystemManager::InstantiateSystem(interlude.systemName);
                    
                    currentVisualSystemPreset = interlude;
                    currentVisualSystemPreset.system->setup();
                    currentVisualSystemPreset.system->setDrawToScreen( false );
                    currentVisualSystemPreset.system->loadPresetGUISFromName( currentVisualSystemPreset.presetName );
                    currentVisualSystemPreset.system->playSystem();
                    
                    currentVisualSystemPreset = currentVisualSystemPreset;
                    currentVisualSystem = currentVisualSystemPreset.system;
                    
                    showingVisualSystem = true;
                    
                }
                
                break;
                
			case TRANSITION_QUESTION_IN:
				
                currentAct->getTimeline().stop();
                
                //clearAct(true);
                
				// show question transition over this period
				rgbdVisualSystem->transtionFinished();
				rgbdVisualSystem->stopSystem();
                break;
				
            case TRANSITION_IDLE:
                
                if(transitionController.getPreviousState() == TRANSITION_INTRO_OUT){
					
					q = introSequence->getSelectedQuestion();
                    clip = q->clip;
                    
                    showingVisualSystem = false;
                    introSequence->stopSystem();
					
                    storyEngine.buildAct(run, clip, q->topic, true);
                    
					sound.exitTunnel();
                }
				else if(transitionController.getPreviousState() == TRANSITION_QUESTION_IN){
					
					q = rgbdVisualSystem->getSelectedQuestion();
					clip = q->clip;
					topic = q->topic;
					
                    rgbdVisualSystem->clearQuestions();
                    
					bQuestionAsked = false;
					
					storyEngine.buildAct(run, clip, topic);
					
				}
                else if(transitionController.getPreviousState() == TRANSITION_CLUSTERMAP_OUT){
                    
					showingVisualSystem = false;
                    if (clusterMap->isPlaying()) {
                        clusterMap->stopSystem();
                    }
                    else if(currentVisualSystem->isPlaying()){
                        currentVisualSystem->stopSystem();
                        currentVisualSystem->exit();
                        currentVisualSystem = ofPtr<CloudsVisualSystem>();
                    }
                    
					
                    //build the next clip based on the history
					storyEngine.buildAct(run);
					
                    cout<<"IDLE POST TRANSITION CLUSTERMAP OUT"<<endl;
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
        
#ifdef OCULUS_RIFT
        //      ofVec2f overlaySize = hud.getSize();
        //      CloudsVisualSystem::getOculusRift().beginOverlay(-230, overlaySize.x,overlaySize.y);
#endif
        
#ifdef SHOW_SUBTITLES
        CloudsVisualSystem::getRGBDVideoPlayer().drawSubtitles(
                                                               CloudsVisualSystem::getStaticRenderTarget().getWidth()/2,
                                                               (float)CloudsVisualSystem::getStaticRenderTarget().getHeight()*0.8);
#endif
        
#ifdef OCULUS_RIFT
        //     hud.drawOverlay(overlaySize);
#else
		hud.draw();
#endif
        
#ifdef OCULUS_RIFT
        //       CloudsVisualSystem::getOculusRift().endOverlay();
#endif
		
		ofPopStyle();
	}
    else{
        ofBackground(0, 0, 255)
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
		string debugString =
		currentVisualSystemPreset.presetName + " was associated with keyword " + currentVisualSystemPreset.conjureKeyword + "\n" +
        "Preset's keywords " + ofJoinString(currentVisualSystemPreset.allKeywords, ", ") + "\n" +
        "current clip's keywords " + ofJoinString(currentClip.getKeywords(), ", ") + "\n" +
        "Had to default to keyword family? " + (currentVisualSystemPreset.defaultedToFamily ? "YES" : "NO") + "\n" +
        "Had to pick a random preset? " + (currentVisualSystemPreset.randomlySelected ? "YES" : "NO") + "\n" +
        "Act #? " + ofToString(run.actCount);
		
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
	
	numClipsPlayed = 0;
	shouldPlayAct = true;
	currentAct = args.act;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::actBegan(CloudsActEventArgs& args){

    if(!args.act->startsWithVisualSystem()){
        transitionController.transitionToFirstInterview(1.0);
    }
    else{
        //we'll get a visual system triggered event;
        bool b = false;
    }
}

//--------------------------------------------------------------------
void CloudsPlaybackController::actEnded(CloudsActEventArgs& args){
	
    //make sure its' stopped
    CloudsVisualSystem::getRGBDVideoPlayer().getPlayer().stop();
    
	cout << "ACT ENDED TRIGGERED" << endl;
    if(!bQuestionAsked){
        cout << "Transitioning to cluster map" << endl;
        transitionController.transitionToClusterMap(1.0,1.0);
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
	   currentVisualSystem == clusterMap)
	{
		transitionController.transitionToFirstVisualSystem(1.0);
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
void CloudsPlaybackController::playClip(CloudsClip& clip){
    
	numClipsPlayed++;
	
    //	rgbdVisualSystem->clearQuestions();
	if(clip.getID() != prerolledClipID){
		prerollClip(clip, 1);
	}
	
	rgbdVisualSystem->setupSpeaker( clip.getSpeakerFirstName(),clip.getSpeakerLastName(), clip.name);
	
	prerolledClipID = "";
	currentClip = clip;
	
	rgbdVisualSystem->getRGBDVideoPlayer().swapAndPlay();
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
	rgbdVisualSystem->loadPresetGUISFromName("RGBDOC");
#else
    if(run.actCount == 1){
        rgbdVisualSystem->loadPresetGUISFromName("RGBDMain");
    }
    else{
        rgbdVisualSystem->loadPresetGUISFromName("RGBDMain2");
    }
#endif
	rgbdVisualSystem->startTransitionIn( currentVisualSystem->getTransitionType() );
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
		
		showingVisualSystem = true;
	}
	else{
		ofLogError("CloudsPlaybackController::playNextVisualSystem") << "nextVisualSystemPreset == NULL";
	}
}
