
#include "CloudsPlaybackController.h"

//if( e.name == fadeOutRGBD )
//{
//	if(e.message == "started")
//	{
//		
//		ofLog(OF_LOG_VERBOSE) << "RGBD fading out: " << ofGetElapsedTimef();
//		//JG TEMP HACKING OUT TRANSITIONS
//		//			rgbdVisualSystem->transitionOut( currentVisualSystem->getTransitionType(), e.span );
//		rgbdVisualSystem->selfUpdate();
//	}
//	if(e.message == "updated"){
//		crossfadeValue = e.value;
//		rgbdVisualSystem->visualSystemFadeValue = crossfadeValue;
//	}
//	if(e.message == "ended")
//	{
//		ofLog(OF_LOG_VERBOSE) << "RGBD faded out: " << ofGetElapsedTimef();
//		
//		//play our ext visual system & stop the rgbd
//		playNextVisualSystem();
//		
//		//fade in nextVisual system
//		float fadeInDuration = 1;
//		
//		//fade in the next system
//		addControllerTween( fadeInVisualSystem, ofGetElapsedTimef(), fadeInDuration, 0, 1, NULL );
//	}
//}
//
//if( e.name == fadeInRGBD )
//{
//	if(e.message == "started")
//	{
//		ofLog(OF_LOG_VERBOSE) << "RGBD FADING IN: " << ofGetElapsedTimef() ;
//		
//		// play & transition in the RGBD so that we see as we fade in
//		//JG TEMP HACKING OUT TRANSITIONS
//		//			rgbdVisualSystem->transitionIn( currentVisualSystem->getTransitionType(), e.span );
//		rgbdVisualSystem->playSystem();
//		rgbdVisualSystem->selfUpdate();
//	}
//	if(e.message == "updated"){
//		crossfadeValue = e.value;
//		rgbdVisualSystem->visualSystemFadeValue = crossfadeValue;
//	}
//	if(e.message == "ended")
//	{
//		ofLog(OF_LOG_VERBOSE) << "RGBD FADED IN: " << ofGetElapsedTimef() ;
//		
//	}
//}
//
//if( e.name == fadeInVisualSystem){
//	if(e.message == "started")
//	{
//		ofLog(OF_LOG_VERBOSE) << "fading in next visual system: " << ofGetElapsedTimef();
//	}
//	if(e.message == "updated"){
//		crossfadeValue = e.value;
//	}
//	if(e.message == "ended")
//	{
//		ofLog(OF_LOG_VERBOSE) << "faded in next visual system: " << ofGetElapsedTimef();
//	}
//}
//
//
//if( e.name == fadeOutVisualSystem){
//	if(e.message == "started")
//	{
//		ofLog(OF_LOG_VERBOSE) << "fading out next visual system" << ofGetElapsedTimef();
//	}
//	if(e.message == "updated"){
//		crossfadeValue = e.value;
//	}
//	if(e.message == "ended")
//	{
//		ofLog(OF_LOG_VERBOSE) << "faded out next visual system" << ofGetElapsedTimef();
//		
//		//hide the visual system
//		hideVisualSystem();
//		
//	}
//}

CloudsPlaybackController::CloudsPlaybackController(){
	
	storyEngine = NULL;
	eventsRegistered = false;
//	fadingIntro = false;
	
	revertToIntroAfter1Act = false;
	actFinished = false;
	
	showingVisualSystem = false;
	currentAct = NULL;
	showingClusterMap = false;
	showingCursor = false;
//	targetScratchVolume = currentVolume = 1.0;
}

//--------------------------------------------------------------------
CloudsPlaybackController::~CloudsPlaybackController(){
}

void CloudsPlaybackController::clearAct(bool destroyAct){
	
	if(currentAct != NULL){
		vector<CloudsVisualSystemPreset>& currentPresets = currentAct->getAllVisualSystemPresets();
		for(int i = 0; i < currentPresets.size(); i++){
			//flag them done!
			if(currentPresets[i].system != NULL){
				currentPresets[i].system->exit();
			}
			
		}
		currentAct->unregisterEvents(this);
        currentAct->unregisterEvents(&introSequence->getSelectedRun());
		currentAct->unregisterEvents(&hud);
		if(destroyAct){
			delete currentAct;
		}
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::exit(ofEventArgs & args){
	if(eventsRegistered){
		eventsRegistered = false;
		
		ofUnregisterMouseEvents(this);
		ofUnregisterKeyEvents(this);
		
		ofRemoveListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
	}
	
	clearAct();

	if(storyEngine != NULL){
		ofRemoveListener(storyEngine->getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
	}

//	ofRemoveListener( CloudsPlaybackControllerEvent::events, this, &CloudsPlaybackController::CloudsPlaybackControllerEventHandler );
}

//--------------------------------------------------------------------
void CloudsPlaybackController::setup(){
	
	rgbdVisualSystemFadeInDuration = 3;
	rgbdVisualSystemFadeOutDuration = 3;
	
	if(!eventsRegistered){
		
		eventsRegistered = true;
		
		ofRemoveListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		ofRemoveListener(ofEvents().update, this, &CloudsPlaybackController::update);

		ofAddListener(ofEvents().update, this, &CloudsPlaybackController::update);
		ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
		rgbdVisualSystem = ofPtr<CloudsVisualSystemRGBD>( new CloudsVisualSystemRGBD() );
		introSequence = ofPtr<CloudsIntroSequence>( new CloudsIntroSequence() );
		
		rgbdVisualSystem->setup();
		rgbdVisualSystem->setDrawToScreen( false );
		
		introSequence->setup();
		introSequence->setDrawToScreen(false);
		currentVisualSystem = introSequence;
		
		clusterMapVisualSystem.setup();
		//clusterMapVisualSystem.setDrawToScreen( false );
		
		//start an initila fade... and set our fade variables
//		fadeDuration = 1;
//		fadeStartTime = ofGetElapsedTimef();
//		fadeEndTime = fadeStartTime + fadeDuration;
//		fadeStartVal = 0;
//		fadeTargetVal = 1.;
//		
//		fadingOut = fadingIn = false;
//		crossfadeValue = 1.;
//		bIsFading = false;
		
		hud.setup();
		
		//setup scratch tracks
		
	}
	
	ofHideCursor();
	
//	CloudsPlaybackControllerTween t;
//	t.setup("testTween", ofGetElapsedTimef(), 5, 0, 100);
//	controllerTweens.push_back( t );
//	ofAddListener( CloudsPlaybackControllerEvent::events, this, &CloudsPlaybackController::CloudsPlaybackControllerEventHandler );
//	
//	fadeOutRGBD = "fadeOutRGBD";
//	fadeInRGBD = "fadeInRGBD";
//	fadeOutVisualSystem = "fadeOutVisualSystem";
//	fadeInVisualSystem = "fadeInVisualSystem";
}

//void CloudsPlaybackController::addControllerTween( string name, float startTime, float span, float startVal, float endVal, float *value ){
//	
//	CloudsPlaybackControllerTween t;
//	t.setup( name, startTime, span, startVal, endVal, value );
//	
//	controllerTweens.push_back( t );
//}

//void CloudsPlaybackController::startScratchTracks(){
//	ofDirectory dir(GetCloudsDataPath() + "scratch/");
//	dir.allowExt("aif");
//	dir.allowExt("aiff");
//	dir.allowExt("wav");
//	dir.allowExt("mp3");
//	dir.sort();
//	dir.listDir();
//	for(int i = 0; i < dir.numFiles(); i++){
//		scratchTracks.push_back(dir.getPath(i));
//	}
//	if(scratchTracks.size() > 0){
//		currentScratch = 0;
//		scratchPlayer.loadSound(scratchTracks[0]);
//		scratchPlayer.play();
//	}
//}

//void CloudsPlaybackController::playScratchTrack(string track){
//	for(int i = 0; i < scratchTracks.size(); i++){
//		if(ofFilePath::getFileName(scratchTracks[i]) == track){
//			scratchPlayer.loadSound(scratchTracks[i]);
//			scratchPlayer.play();
//			break;
//		}
//	}
//}

CloudsVisualSystemClusterMap& CloudsPlaybackController::getClusterMap(){
	return clusterMapVisualSystem;
}

CloudsRGBDVideoPlayer& CloudsPlaybackController::getSharedVideoPlayer(){
	return CloudsVisualSystem::getRGBDVideoPlayer();
}


//void CloudsPlaybackController::setUseScratch(bool useScratch){
//	if(useScratch){
//		targetScratchVolume = .9;
//		scratchVolumeAttenuate = 1.0;
//	}
//	else{
//		targetScratchVolume = 0.0;
//	}
//}

//--------------------------------------------------------------------
void CloudsPlaybackController::setStoryEngine(CloudsStoryEngine& storyEngine){
	if(this->storyEngine != NULL){
		ofRemoveListener(this->storyEngine->getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
	}
	ofAddListener(storyEngine.getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
    this->storyEngine = &storyEngine;
}

//we currently use introSequence.run() as a hack
void CloudsPlaybackController::setRun(CloudsRun &run){
//    this->currentRun = &run;
}

void CloudsPlaybackController::showIntro(vector<CloudsClip>& possibleStartQuestions){
	introSequence->setStartQuestions(possibleStartQuestions);
	showIntro();
}

//private internal one
void CloudsPlaybackController::showIntro(){
//	scratchVolumeAttenuate = 1.0;
	
#ifdef OCULUS_RIFT
	introSequence->loadPresetGUISFromName("Oculus");
#else
	introSequence->loadPresetGUISFromName("TunnelWarp");
#endif
	introSequence->playSystem();
	
	showingVisualSystem = true;
	showingIntro = true;
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::playAct(CloudsAct* act){

	bool destroyAct = currentAct != act;
	clearAct(destroyAct);
	
	currentAct = act;

	//TODO: show loading screen while we initialize all the visual systems
	vector<CloudsVisualSystemPreset>& presets = currentAct->getAllVisualSystemPresets();
	vector< ofPtr<CloudsVisualSystem> > systems = CloudsVisualSystemManager::InstantiateSystems(presets);
	for(int i = 0; i < presets.size(); i++){
		if(presets[i].system != NULL){
			cout << "CloudsPlaybackController::playAct -- Setting up:: " << presets[i].systemName << endl;
			presets[i].system->setup();
		}
		else{
			ofLogError("CloudsPlaybackController::playAct") << presets[i].systemName << " NULL right after instantiaton. correlating system null? " << (systems[i] == NULL ? "YES" : "NO");
		}
	}
	
	currentAct->registerEvents(this);
    currentAct->registerEvents(&introSequence->getSelectedRun());
	currentAct->registerEvents(&hud);
	currentAct->play();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::keyPressed(ofKeyEventArgs & args){
	
	if(args.key == 'R'){
//		combinedRenderer.reloadShader();
		CloudsQuestion::reloadShader();
	}
	
//	if(args.key == 'P'){
//		currentAct->getTimeline().togglePlay();
//	}
	
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
	}
	
	//SCRATCH SCRUB
//	if(scratchTracks.size() > 0){
//		if(args.key == OF_KEY_UP){
//			targetScratchVolume = MIN(targetScratchVolume+.1, 1.0);
//			//scratchPlayer.setVolume(currentVolume);
//		}
//		else if(args.key == OF_KEY_DOWN){
//			targetScratchVolume = MAX(targetScratchVolume-.1, 0.0);
//			//scratchPlayer.setVolume(currentVolume);
//		}
//		else if(args.key == OF_KEY_RIGHT){
////			currentScratch = (currentScratch + 1) % scratchTracks.size();
////			scratchPlayer.stop();
////			scratchPlayer.unloadSound();
////			scratchPlayer.loadSound(scratchTracks[currentScratch]);
////			scratchPlayer.play();
//		}
//		else if (args.key == OF_KEY_LEFT){
////			currentScratch = (scratchTracks.size() + currentScratch - 1) % scratchTracks.size();
////			scratchPlayer.stop();
////			scratchPlayer.unloadSound();
////			scratchPlayer.loadSound(scratchTracks[currentScratch]);
////			scratchPlayer.play();
//		}
//	}
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
		
//	currentVolume += (targetScratchVolume - currentVolume) * .05;
//	scratchPlayer.setVolume( currentVolume * scratchVolumeAttenuate );
	
	////////////////////
	//INTRO
	if(showingIntro){
		if(introSequence->isStartQuestionSelected()){
			
			CloudsQuestion* q = introSequence->getSelectedQuestion();
			CloudsClip& clip = q->clip;
			
			map<string,string> questionsAndTopics = clip.getAllQuestionTopicPairs();
			if(questionsAndTopics.size() > 0){
				//addControllerTween( fadeOutVisualSystem, ofGetElapsedTimef(), fadeDuration, 1, 0, NULL );
				//transitionController.transitionToVisualSystem(1.0, 1.0);
				transitionController.transitionFromIntro(1.0, 1.0);
//				storyEngine->buildAct(introSequence->getSelectedRun(), currentClip, currentTopic);
			}
			else{
				ofLogError("CloudsPlaybackController::update") << "Somehow selected an intro question with no topics " << clip.getLinkName();
			}
		}
	}
	
	////////////////////
	//CLUSTER MAP
	else if(showingClusterMap){
		//TODO add questions to cluster map
		//right now we can just have a canned animation and stop it when we are done
		if(!clusterMapVisualSystem.getTimeline()->getIsPlaying()){
			
//			CloudsQuestion* q = clusterMapVisualSystem.getSelectedQuestion();
//			CloudsClip& clip = q->clip;
			
			showingClusterMap = false;
			clusterMapVisualSystem.stopSystem();
			storyEngine->buildAct(introSequence->getSelectedRun(), currentClip, currentTopic);
		}
	}
	
	
	if(!showingIntro && !showingClusterMap){
		hud.update();
	}
	
	////////////////////
	// RGBD SYSTEM
	
	//updating tweens
	//JG TRANSITION HACKING
//	float elapsedTime = ofGetElapsedTimef();
//	for (int i = controllerTweens.size() - 1; i >= 0; i--) {
//		controllerTweens[i].update( elapsedTime );
//		
//		if(controllerTweens[i].bEnded){
//			controllerTweens.erase(controllerTweens.begin() + i );
//		}
//	}
	updateTransition();
	
	if(rgbdVisualSystem->isQuestionSelectedAndClipDone()){
		CloudsQuestion* q = rgbdVisualSystem->getSelectedQuestion();
		CloudsClip clip = q->clip;
		string topic = q->topic;
		
		rgbdVisualSystem->clearQuestions();
		
		//cout << " *** SELECTED QUESTION Clip : "<<clip.name<<" Staring point for new act. Question: "<< q->question << " topic " << q->topic << endl;
		
		rgbdVisualSystem->stopSystem();
		introSequence->getSelectedRun().questionTopicHistory.insert(topic);
		storyEngine->buildAct(introSequence->getSelectedRun(), clip, topic);
		
		//TODO: transition to question selection
	}

}

//--------------------------------------------------------------------
void CloudsPlaybackController::updateTransition(){
	
	transitionController.update();
	
	if(transitionController.isStateNew()){
		
		if(transitionController.getCurrentState() == TRANSITION_INTERVIEW_OUT){
			rgbdVisualSystem->startTransitionOut( currentVisualSystem->getTransitionType() );
//			rgbdVisualSystem->selfUpdate(); //still necessary?
		}
		else if(transitionController.getCurrentState() == TRANSITION_INTRO_OUT){
			showingIntro = false;
		}
		else if(transitionController.getCurrentState() == TRANSITION_VISUALSYSTEM_IN){
			rgbdVisualSystem->transtionFinished();
			playNextVisualSystem();
		}
		else if(transitionController.getCurrentState() == TRANSITION_VISUALSYSTEM_OUT){
			// no need to do anything special, the crossfade value will take care of this
		}
		else if(transitionController.getCurrentState() == TRANSITION_INTERVIEW_IN){
			hideVisualSystem();
			rgbdVisualSystem->playSystem();
			rgbdVisualSystem->startTransitionIn( currentVisualSystem->getTransitionType() );
		}
		else if(transitionController.getCurrentState() == TRANSITION_IDLE){
			//we just finished fading out of the intro
			
			if(transitionController.getPreviousState() == TRANSITION_INTRO_OUT){
				CloudsQuestion* q = introSequence->getSelectedQuestion();
				CloudsClip& clip = q->clip;
				
				showingVisualSystem = false;
				introSequence->stopSystem();
				
//				rgbdVisualSystem->visualSystemFadeValue = 0.0;
//				rgbdVisualSystem->selfUpdate();
				
				storyEngine->buildAct(introSequence->getSelectedRun(), clip, q->topic );
			}
			//we just finished fading out of the intreview
			else{
				rgbdVisualSystem->transtionFinished();
			}
		}

	}
		
	crossfadeValue = transitionController.getFadeValue();
	rgbdVisualSystem->visualSystemFadeValue = crossfadeValue;
	if(transitionController.transitioning){
		rgbdVisualSystem->updateTransition(crossfadeValue);
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
				"Had to pick a random preset? " + (currentVisualSystemPreset.randomlySelected ? "YES" : "NO");
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
	playAct(args.act);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::actBegan(CloudsActEventArgs& args){

}

//--------------------------------------------------------------------
void CloudsPlaybackController::actEnded(CloudsActEventArgs& args){
	
	cout << "ACT ENDED TRIGGERED" << endl;

	//TEMPORARY FOR DEMO
	if(revertToIntroAfter1Act){
		actFinished = true;
	}
	else{
		rgbdVisualSystem->stopSystem();
		
		clusterMapVisualSystem.setRun(introSequence->getSelectedRun());
		clusterMapVisualSystem.traverse();
		
		clusterMapVisualSystem.loadPresetGUISFromName("DefaultCluster");
		clusterMapVisualSystem.playSystem();
		
		showingClusterMap = true;
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::clipBegan(CloudsClipEventArgs& args){
	playClip(args.chosenClip);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::visualSystemBegan(CloudsVisualSystemEventArgs& args)
{
	if(!showingVisualSystem){
		showVisualSystem( args.preset, 1.0	);
	}
	else{
		ofLogError() << "Triggered visual system while still showing one";
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::visualSystemEnded(CloudsVisualSystemEventArgs& args)
{
	if(showingVisualSystem){

		//JG: Timing thing. If the system is indefinite, and has an outro then it most likely was created with
		//a "middle" flag, which would stop the timeline. so when the system is ready to fade out let's play it again to
		//watch the outro		
		if(args.preset.outroDuration > 0 && args.preset.indefinite){
			args.preset.system->getTimeline()->play();
		}
	
		float fadeDuration = 1; //(args.preset.outroDuration != 0)? args.preset.outroDuration : 1;
//		addControllerTween( fadeOutVisualSystem, ofGetElapsedTimef(), fadeDuration, 1, 0, NULL );
		transitionController.transitionToInterview(fadeDuration, 1.0);
		
	}
	else{
		ofLogError() << "Hiding visual system while none is showing";
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::questionAsked(CloudsQuestionEventArgs& args){
	if(!showingVisualSystem){
		//don't ask a topic that we've already seen
		CloudsRun& run = introSequence->getSelectedRun();
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
void CloudsPlaybackController::showVisualSystem(CloudsVisualSystemPreset& nextVisualSystem, float transitionDuration) {
	if(showingVisualSystem){
		ofLogError("CloudsPlaybackController::showVisualSystem") << "Still showing last system";
		hideVisualSystem();
	}
	
	rgbdVisualSystem->clearQuestions();
	//store the preset name for loading later in playNextVisualSystem()
	nextPresetName = nextVisualSystem.presetName;
	nextSystem = nextVisualSystem.system;
//	cout << "CloudsPlaybackController::showVisualSystem SETTING NEXT SYSTEM TO " << nextVisualSystem.presetName << endl;
	if(nextSystem == NULL){
		ofLogError("CloudsPlaybackController::showVisualSystem") << "Incoming system is NULL";
	}
	currentVisualSystemPreset = nextVisualSystem;
	
	//most of the time we will be looking at the RGBDVisualSystem
	if(currentVisualSystem == rgbdVisualSystem){
		//start the rgbd fade out. playNextVisualSystem() will be called once it's faded out
//		addControllerTween(fadeOutRGBD, ofGetElapsedTimef(), fadeDuration, 1, 0, NULL );
		transitionController.transitionToVisualSystem(1.0, 1.0);
	}
	//in the case of the intro we won't be, so just fade this system directly in
	else{
		playNextVisualSystem();

		//fade in nextVisual system
		float fadeInDuration = 1;
		//fade in the next system
//		addControllerTween( fadeInVisualSystem, ofGetElapsedTimef(), fadeInDuration, 0, 1, NULL );
		transitionController.transitionToVisualSystem(1.0, 1.0);
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::hideVisualSystem()
{
	if(showingVisualSystem){
		
		//HACK for demo!!!
		if(revertToIntroAfter1Act && actFinished){
			actFinished = false;
//			playScratchTrack("00 Parallel Stripes.aif");
			rgbdVisualSystem->stopSystem();
			currentVisualSystem = introSequence;
			showIntro();
			float fadeInDuration = 1;
			//fade in the next system
//			addControllerTween( fadeInVisualSystem, ofGetElapsedTimef(), fadeInDuration, 0, 1, NULL );
			transitionController.transitionToVisualSystem(1.0, 1.0);
		}
//		else if(currentVisualSystem == introSequence){
//			CloudsQuestion* q = introSequence->getSelectedQuestion();
//			CloudsClip& clip = q->clip;
//
//			introSequence->stopSystem();
//			rgbdVisualSystem->visualSystemFadeValue = 0.0;
//			rgbdVisualSystem->selfUpdate();
//			
//			storyEngine->buildAct(introSequence->getSelectedRun(), clip, q->topic );
//			
////			fadingIntro = false;
////			scratchPlayer.stop();
////			scratchPlayer.unloadSound();
//		}
		else{
			currentVisualSystem->stopSystem();
#ifdef OCULUS_RIFT
			rgbdVisualSystem->loadPresetGUISFromName("RGBDOC");
#else
			rgbdVisualSystem->loadPresetGUISFromName("RGBDMain");
#endif
			rgbdVisualSystem->playSystem();
			currentVisualSystem = rgbdVisualSystem;
			
			//fade in the RGBD
			float duration = 1;
//			addControllerTween( fadeInRGBD, ofGetElapsedTimef(), duration, 0, 1, NULL );
			transitionController.transitionToInterview(1.0, 1.0);
		}
		
		showingVisualSystem = false;
		
	}
}


void CloudsPlaybackController::playNextVisualSystem()
{
	if(nextSystem != NULL){

		rgbdVisualSystem->stopSystem();
		
		nextSystem->setDrawToScreen( false );
		nextSystem->setCurrentTopic( currentTopic );
		nextSystem->loadPresetGUISFromName( nextPresetName );
		nextSystem->playSystem();
		currentVisualSystem = nextSystem;
		
		showingVisualSystem = true;
	}
	else{
		ofLogError("CloudsPlaybackController::playNextVisualSystem") << "nextSystem == NULL";
	}
}
