
#include "CloudsPlaybackController.h"


CloudsPlaybackController::CloudsPlaybackController(){
	
	eventsRegistered = false;
	
//	revertToIntroAfter1Act = false;
//	actFinished = false;
	
	showingVisualSystem = false;
	currentAct = NULL;
	showingClusterMap = false;
	showingCursor = false;
	shouldPlayAct = false;
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
	
	}
	clearAct();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::setup(){
	
    parser.loadFromFiles();
	
	if(!ofFile::doesFileExist(GetCloudsDataPath() + "CloudsMovieDirectory.txt")){
		ofSystemAlertDialog("Could not find movie file path. \
							Create a file called CloudsMovieDirectory.txt \
							that contains one line, the path to your movies folder");
	}
	
	parser.setCombinedVideoDirectory(ofBufferFromFile(GetCloudsDataPath() + "CloudsMovieDirectory.txt").getText());
	
	visualSystems.loadPresets();
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	storyEngine.printDecisions = false;
	storyEngine.combinedClipsOnly = true;
	storyEngine.setup();
	
	ofAddListener(storyEngine.getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
	
	///SOUND
	mixer.setup();
	sound.setup(storyEngine);

	////COMMUNICATION
	oscSender.setup();
		
	if(!eventsRegistered){
		
		eventsRegistered = true;
		
		ofRemoveListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		ofRemoveListener(ofEvents().update, this, &CloudsPlaybackController::update);

		ofAddListener(ofEvents().update, this, &CloudsPlaybackController::update);
		ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
		
		
		rgbdVisualSystem = ofPtr<CloudsVisualSystemRGBD>( new CloudsVisualSystemRGBD() );
		rgbdVisualSystem->setup();
		rgbdVisualSystem->setDrawToScreen( false );


		clusterMap = ofPtr<CloudsVisualSystemClusterMap>( new CloudsVisualSystemClusterMap() );
		clusterMap->buildEntireCluster(parser);
		clusterMap->setRun(run);
		clusterMap->setup();

		
		introSequence = ofPtr<CloudsIntroSequence>( new CloudsIntroSequence() );
		introSequence->setup();
		introSequence->setDrawToScreen(false);
		currentVisualSystem = introSequence;
		
		hud.setup();
		
	}
	
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
			cout << " Adding Clip " << startingNodes[i].getID() << " with question " << startingNodes[i].getQuestions()[0] << endl;
		}
	}
	
	cout << "Starting with " << startingNodes.size() << endl;
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
	
	sound.enterTunnel();
	
	showingVisualSystem = true;
	showingIntro = true;	
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
	
	if(args.key == 'R'){
//		combinedRenderer.reloadShader();
		CloudsQuestion::reloadShader();
	}
		
	if(args.key == 'Q'){
		for(int i = 0; i < fakeQuestions.size(); i++){
			rgbdVisualSystem->addQuestion(fakeQuestions[i],
										  fakeQuestions[i].getTopicsWithQuestions()[0],
										  fakeQuestions[i].getQuestions()[0]);
		}
	}

	if(args.key == 'C'){
		if(showingCursor){
			ofHideCursor();
		}
		else{
			ofShowCursor();
		}
		showingCursor = !showingCursor;
	}
	
	if(args.key == '\\'){
		if(currentVisualSystem == introSequence){
			introSequence->autoSelectQuestion();
		}
		else {
			
		}
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::keyReleased(ofKeyEventArgs & args){
	
}

void CloudsPlaybackController::mouseDragged(ofMouseEventArgs & args){
	
}

void CloudsPlaybackController::mouseMoved(ofMouseEventArgs & args){
	cursorMovedTime = ofGetElapsedTimef();
}

void CloudsPlaybackController::mousePressed(ofMouseEventArgs & args){

}

void CloudsPlaybackController::mouseReleased(ofMouseEventArgs & args){
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::update(ofEventArgs & args){
		
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
		if(!clusterMap->getTimeline()->getIsPlaying()){
			
			transitionController.transitionFromClusterMap(1.0);
			
			cout << "TRANSITIONING FROM CLUSTER MAP IN UPDATE" << endl;
   
		}
	}

	////////////////////
	// RGBD SYSTEM
	if(rgbdVisualSystem->isQuestionSelectedAndClipDone()){
		CloudsQuestion* q = rgbdVisualSystem->getSelectedQuestion();
		CloudsClip clip = q->clip;
		string topic = q->topic;
		
		rgbdVisualSystem->clearQuestions();
		
		//cout << " *** SELECTED QUESTION Clip : "<<clip.name<<" Staring point for new act. Question: "<< q->question << " topic " << q->topic << endl;
		
		rgbdVisualSystem->stopSystem();
		run.questionTopicHistory.insert(topic);
		storyEngine.buildAct(run, clip, topic);
	}
	
	if(!showingIntro && !showingClusterMap){
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
	
	crossfadeValue = transitionController.getFadeValue();
	rgbdVisualSystem->visualSystemFadeValue = crossfadeValue;
	
	if(transitionController.getCurrentState() != TRANSITION_IDLE){
		cout << "CURRENT STATE IS " << transitionController.getCurrentStateDescription() << " CROSSFADE IS " << crossfadeValue << endl;
	}
	
	if(transitionController.transitioning){
		rgbdVisualSystem->updateTransition( transitionController.getInterviewTransitionPoint() );
	}

	if(transitionController.isStateNew()){
        
        switch (transitionController.getCurrentState()) {
            
            case TRANSITION_INTERVIEW_OUT:
                
                rgbdVisualSystem->startTransitionOut( currentVisualSystem->getTransitionType() );
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
                
//			rgbdVisualSystem->playSystem();
//			rgbdVisualSystem->startTransitionIn( currentVisualSystem->getTransitionType() );
                break;
            
            case TRANSITION_CLUSTERMAP_OUT:
				
				/// wait for it to fade out...
				showingClusterMap = false;
				
                break;

            case TRANSITION_CLUSTERMAP_IN:
                
                if(transitionController.getPreviousState() == TRANSITION_INTERVIEW_OUT){
                    rgbdVisualSystem->transtionFinished();
                    rgbdVisualSystem->stopSystem();
                }
                else if(transitionController.getPreviousState() == TRANSITION_VISUALSYSTEM_OUT){
                    hideVisualSystem();
                }
				
                clusterMap->traverse();
                clusterMap->loadPresetGUISFromName("DefaultCluster");
                clusterMap->playSystem();

				currentVisualSystem = clusterMap;

                showingClusterMap = true;
				
                break;
                
            case TRANSITION_IDLE:
                
                if(transitionController.getPreviousState() == TRANSITION_INTRO_OUT){
                    CloudsQuestion* q = introSequence->getSelectedQuestion();
                    CloudsClip& clip = q->clip;
					
					run.questionTopicHistory.insert(q->topic);
					
                    showingVisualSystem = false;
                    introSequence->stopSystem();
					
                    storyEngine.buildAct(run, clip, q->topic, true);
				
					sound.exitTunnel();
                }
                else if(transitionController.getPreviousState() == TRANSITION_CLUSTERMAP_OUT){

					showingVisualSystem = false;
					clusterMap->stopSystem();
					
					storyEngine.buildAct(run, currentClip, currentTopic, false);
					
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
		/////*****///////
/*
		if(transitionController.getCurrentState() == TRANSITION_INTERVIEW_OUT){
			
			rgbdVisualSystem->startTransitionOut( currentVisualSystem->getTransitionType() );
//			rgbdVisualSystem->selfUpdate(); //still necessary?

		}
		else if(transitionController.getCurrentState() == TRANSITION_INTRO_OUT){
			//intro just started fading out
			showingIntro = false;
			
		}
		else if(transitionController.getCurrentState() == TRANSITION_VISUALSYSTEM_IN){
			if(transitionController.getPreviousState() == TRANSITION_INTERVIEW_OUT){
				rgbdVisualSystem->transtionFinished();
			}
			playNextVisualSystem();
		}
		else if(transitionController.getCurrentState() == TRANSITION_VISUALSYSTEM_OUT){
			// no need to do anything special, the crossfade value will take care of this
		}
		else if(transitionController.getCurrentState() == TRANSITION_INTERVIEW_IN){
			
			hideVisualSystem();
			showRGBDVisualSystem();
			
//			rgbdVisualSystem->playSystem();
//			rgbdVisualSystem->startTransitionIn( currentVisualSystem->getTransitionType() );
		}
		else if(transitionController.getCurrentState() == TRANSITION_IDLE){
			//we just finished fading out of the intro
			
			if(transitionController.getPreviousState() == TRANSITION_INTRO_OUT){
				CloudsPortal* q = introSequence->getSelectedQuestion();
				CloudsClip& clip = q->clip;
				
				showingVisualSystem = false;
				introSequence->stopSystem();
								
				storyEngine->buildAct(introSequence->getSelectedRun(), clip, q->topic );
			}
            else if(transitionController.getPreviousState() == TRANSITION_CLUSTERMAP_OUT){
                //				rgbdVisualSystem->transtionFinished();
                cout<<"IDLE POST TRANSITION CLUSTERMAP OUT"<<endl;
			}
			//we just finished fading out of the interview
			else if(transitionController.getPreviousState() == TRANSITION_INTERVIEW_IN){
				rgbdVisualSystem->transtionFinished();
			}
		}
*/
		/////*****///////
	}		
}

//--------------------------------------------------------------------
void CloudsPlaybackController::draw(ofEventArgs & args){

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable( GL_DEPTH_TEST );

	ofPushStyle();
	//ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	float mixVal = ofClamp( crossfadeValue * 255, 0, 255);
	
	//cout << "crosffade value is " << crossfadeValue << " showing intro? " << showingIntro << endl;
	
	ofSetColor( 255, 255, 255, mixVal );
	if(!showingClusterMap && currentVisualSystem != NULL){
		currentVisualSystem->selfPostDraw();
		if(numClipsPlayed > 1){
			hud.draw();
		}
	}
    ofPopStyle();
    glEnable( GL_DEPTH_TEST );
	
	if(currentAct != NULL){
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
			//cout << "DRAWING DEBUGG STRING " << debugString << endl;
			
			ofDrawBitmapString(debugString, 0,0);
			
			ofSetColor(0,0,0,255);
			ofRect(-5,-5,300, 50);
			
			ofPopStyle();
			ofPopMatrix();
		}
	}
	
	glPopAttrib();
}

#pragma story engine events
//--------------------------------------------------------------------
void CloudsPlaybackController::actCreated(CloudsActEventArgs& args){
	
	numClipsPlayed = 0;
	
	bool destroyAct = currentAct != args.act;
	clearAct(destroyAct);
	
	shouldPlayAct = true;
	currentAct = args.act;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::actBegan(CloudsActEventArgs& args){

}

//--------------------------------------------------------------------
void CloudsPlaybackController::actEnded(CloudsActEventArgs& args){
	
	cout << "ACT ENDED TRIGGERED" << endl;
	transitionController.transitionToClusterMap(1.0,1.0);
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
	
	rgbdVisualSystem->clearQuestions();
	
	nextVisualSystemPreset = args.preset;
	
	//store the preset name for loading later in playNextVisualSystem()
//	nextPresetName = nextVisualSystem.presetName;
//	nextSystem = nextVisualSystem.system;
	
	//	cout << "CloudsPlaybackController::showVisualSystem SETTING NEXT SYSTEM TO " << nextVisualSystem.presetName << endl;
	if(nextVisualSystemPreset.system == NULL){
		ofLogError("CloudsPlaybackController::showVisualSystem") << "Incoming system is NULL";
	}
	
	//most of the time we will be looking at the RGBDVisualSystem
//	if(currentVisualSystem != rgbdVisualSystem){
//		playNextVisualSystem();
//	}
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
void CloudsPlaybackController::visualSystemEnded(CloudsVisualSystemEventArgs& args)
{
	if(showingVisualSystem){
		float fadeDuration = 1; 
		transitionController.transitionToInterview(fadeDuration, 1.0);
	}
	else {
		ofLogError("CloudsPlaybackController::visualSystemEnded") << "Hiding visual system while none is showing";
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::questionAsked(CloudsQuestionEventArgs& args){
	if(!showingVisualSystem){
		//don't ask a topic that we've already seen
		if(run.questionTopicHistory.find(args.topic) == run.questionTopicHistory.end()){
			rgbdVisualSystem->addQuestion(args.questionClip, args.topic, args.question);
		}
	}
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
		clipLoadSuccessfullyLoaded = rgbdVisualSystem->getRGBDVideoPlayer().setupVO(clip.voiceOverAudioPath);
	}
	else{
		clipLoadSuccessfullyLoaded = rgbdVisualSystem->getRGBDVideoPlayer().setup( clip.combinedVideoPath, clip.combinedCalibrationXMLPath, toTime,clip.getSpeakerVolume());
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
	
	rgbdVisualSystem->clearQuestions();
	if(clip.getID() != prerolledClipID){
		prerollClip(clip,1);
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
	rgbdVisualSystem->loadPresetGUISFromName("RGBDMain");
#endif
	rgbdVisualSystem->startTransitionIn( currentVisualSystem->getTransitionType() );
	rgbdVisualSystem->playSystem();
	
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
