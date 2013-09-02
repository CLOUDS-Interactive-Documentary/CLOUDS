
#include "CloudsPlaybackController.h"

//TODO: rename crossfadeValue
ofEvent <CloudsPlaybackControllerEvent> CloudsPlaybackControllerEvent::events;

void CloudsPlaybackController::CloudsPlaybackControllerEventHandler( CloudsPlaybackControllerEvent &e )
{
	
	if( e.name == fadeOutRGBD )
	{
		//cout << e.name << " : " << e.message << endl;
		if(e.message == "started")
		{
			cout << endl << "RGBD fading out" << endl << endl;
			rgbdVisualSystem.transitionOut( currentVisualSystem->getTransitionType(), e.span );
			rgbdVisualSystem.selfUpdate();
		}
		if(e.message == "ended")
		{
			cout << endl << "RGBD faded out" << endl << endl;
			
			//stop the rgbd system
			rgbdVisualSystem.stopSystem();
			playNextVisualSystem();
			
			//fade in nextVisual system
			float duration = 1;
			
			//fade in the next system
			addControllerTween( fadeInVisualSystem, ofGetElapsedTimef(), duration, 0, 1, &crossfadeValue );
		}
	}
	
	if( e.name == fadeInRGBD )
	{
		//cout << e.name << " : " << e.message << endl;
		if(e.message == "started")
		{
			cout << endl << "RGBD FADING IN" << endl << endl;
			

			rgbdVisualSystem.transitionIn( currentVisualSystem->getTransitionType(), e.span );
			rgbdVisualSystem.playSystem();
			
			rgbdVisualSystem.selfUpdate();//:) updating fixes the 1 frame lag for the camera update
		}
		
		if(e.message == "ended")
		{
			cout << endl << "RGBD FADED IN" << endl << endl;
//			currentVisualSystem = &rgbdVisualSystem;
		}
	}
	
	if( e.name == fadeInVisualSystem){
		if(e.message == "started")
		{
			cout << endl << "fading in next visual system" << endl << endl;
		}
		
		if(e.message == "ended")
		{
			cout << endl << "faded in next visual system" << endl << endl;
		}
	}
	
	
	if( e.name == fadeOutVisualSystem){
		if(e.message == "started")
		{
			cout << endl << "fading out next visual system" << endl << endl;
		}
		
		if(e.message == "ended")
		{
			cout << endl << "faded out next visual system" << endl << endl;
			
			hideVisualSystem();
			
			float duration = 1;
			addControllerTween( fadeInRGBD, ofGetElapsedTimef(), duration, 0, 1, &crossfadeValue );
			
		}
	}
}


void CloudsPlaybackController::addControllerTween( string name, float startTime, float span, float startVal, float endVal, float *value )
{
	
	CloudsPlaybackControllerTween t;
	t.setup( name, startTime, span, startVal, endVal, value );
	
	controllerTweens.push_back( t );
}

CloudsPlaybackController::CloudsPlaybackController(){
	storyEngine = NULL;
	eventsRegistered = false;
	currentVisualSystem = NULL;
	showingVisualSystem = false;
	currentAct = NULL;
	showingClusterMap = false;
	
	//JG cut out transition hack
	TEMP_SYSTEM_HACK = NULL;
}

//--------------------------------------------------------------------
CloudsPlaybackController::~CloudsPlaybackController(){
}

//--------------------------------------------------------------------
void CloudsPlaybackController::exit(ofEventArgs & args){
	if(eventsRegistered){
		eventsRegistered = false;
		
		ofUnregisterMouseEvents(this);
		ofUnregisterKeyEvents(this);
		
		ofRemoveListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
		
	}
	
	if(currentAct != NULL){
		currentAct->unregisterEvents(this);
		delete currentAct;
	}
	
	if(storyEngine != NULL){
		ofRemoveListener(storyEngine->getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
	}
	
	
	ofRemoveListener( CloudsPlaybackControllerEvent::events, this, &CloudsPlaybackController::CloudsPlaybackControllerEventHandler );
}

//--------------------------------------------------------------------
void CloudsPlaybackController::setup(){
	//LB
	//	create a shared fbo. We'll pass a pointer to each visual system as the are played
//	sharedRenderTarget.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
//	sharedRenderTarget.begin();
//	ofClear(0,0,0,0);
//	sharedRenderTarget.end();
//
	//JG pretty sure we don't need the FBO anymore
	nextRenderTarget.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
	nextRenderTarget.begin();
	ofClear(0,0,0,0);
	nextRenderTarget.end();
	
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
		
		rgbdVisualSystem.setup();
		rgbdVisualSystem.setDrawToScreen( false );
		currentVisualSystem = &rgbdVisualSystem;
		
		introSequence.setup();
		//introSequence.setDrawToScreen(false);
		
		clusterMapVisualSystem.setup();
		//clusterMapVisualSystem.setDrawToScreen( false );
		
		//start an initila fade... and set our fade variables
		fadeDuration = 1;
		fadeStartTime = ofGetElapsedTimef();
		fadeEndTime = fadeStartTime + fadeDuration;
		fadeStartVal = 0;
		fadeTargetVal = 1.;
		
		fadingOut = fadingIn = false;
		crossfadeValue = 1.;
		bIsFading = false;
		
	}
	
	CloudsPlaybackControllerTween t;
	t.setup("testTween", ofGetElapsedTimef(), 5, 0, 100);
	controllerTweens.push_back( t );
	
	ofAddListener( CloudsPlaybackControllerEvent::events, this, &CloudsPlaybackController::CloudsPlaybackControllerEventHandler );
	
	fadeOutRGBD = "fadeOutRGBD";
	fadeInRGBD = "fadeInRGBD";
	fadeOutVisualSystem = "fadeOutVisualSystem";
	fadeInVisualSystem = "fadeInVisualSystem";
}

//--------------------------------------------------------------------
void CloudsPlaybackController::setStoryEngine(CloudsStoryEngine& storyEngine){
	if(this->storyEngine != NULL){
		ofRemoveListener(this->storyEngine->getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
	}
	ofAddListener(storyEngine.getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
	this->storyEngine = &storyEngine;
}

void CloudsPlaybackController::showIntro(vector<CloudsClip>& possibleStartQuestions){

	
	//TEMPORARY:: Should be set at the end of the act to all unasked questions from the last act
	clusterMapVisualSystem.setQuestions(possibleStartQuestions);
	
	introSequence.setStartQuestions(possibleStartQuestions);
	introSequence.playSystem();
	introSequence.loadPresetGUISFromName("TunnelWarp");
	showingIntro = true;	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::playAct(CloudsAct* act){

	//TODO: show loading screen while we initialize all the visual systems
	vector<CloudsVisualSystem*> systems = act->getAllVisualSystems();
	for(int i = 0; i < systems.size(); i++){
		if(systems[i] != NULL){
			cout << "CloudsPlaybackController::playAct -- Setting up:: " << systems[i]->getSystemName() << endl;
			systems[i]->setup();
		}
	}
	
	if(currentAct != NULL){
		currentAct->unregisterEvents(this);
		delete currentAct;
	}

	currentAct = act;
	currentAct->registerEvents(this);
	currentAct->play();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::keyPressed(ofKeyEventArgs & args){
	
	if(args.key == 'R'){
//		combinedRenderer.reloadShader();
	}
	
	if(args.key == 'P'){
		currentAct->getTimeline().togglePlay();
	}
	
	if(args.key == 'Q'){
		currentClip.addQuestionTopicPair("topic", "What does it feel like to code?");
		rgbdVisualSystem.addQuestion(currentClip);
	}

}

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

//--------------------------------------------------------------------
void CloudsPlaybackController::update(ofEventArgs & args){
	
	
	if(showingIntro){
		if(introSequence.isStartQuestionSelected()){
			
			CloudsQuestion* q = introSequence.getSelectedQuestion();
			CloudsClip& clip = q->clip;
			
			//ofLogNotice() << clip.getLinkName() << " Started with question " << clip.getStartingQuestion() << endl;
			
			map<string,string> questionsAndTopics = clip.getAllQuestionTopicPairs();
			if(questionsAndTopics.size() > 0){
				showingIntro = false;				
				introSequence.stopSystem();
				storyEngine->buildAct(introSequence.getSelectedRun(), clip, q->topic );
			}
			//TODO: Transition out of the act into the loading screen.
		}
	}
	else if(showingClusterMap){
		//TODO add questions to cluster map
		//right now we can just have a canned animation and stop it when we are done
		if(!clusterMapVisualSystem.getTimeline()->getIsPlaying()){
			CloudsQuestion* q = clusterMapVisualSystem.getSelectedQuestion();
			CloudsClip& clip = q->clip;
			
			showingClusterMap = false;
			clusterMapVisualSystem.stopSystem();
			storyEngine->buildAct(introSequence.getSelectedRun(), clip, q->topic );
		}
	}
	else {
		//updating tweens
		float elapsedTime = ofGetElapsedTimef();
		for (int i = controllerTweens.size()-1; i>=0; i--) {
			controllerTweens[i].update( elapsedTime );
			
			if(controllerTweens[i].bEnded){
				controllerTweens.erase(controllerTweens.begin() + i );
			}
		}
	}
	
	//TODO: add camera animations to RGBDVisSys
	
	//TODO: offsetTargets for turning away
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::draw(ofEventArgs & args){
    
	//turn off depth testing and enable blending
    glDisable( GL_DEPTH_TEST );
	
	ofPushStyle();
	
	//???: rgbdVisualSystem.getBlendMode()
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	float mixVal = ofClamp( crossfadeValue * 255, 0, 255);
	
	ofSetColor( 255, 255, 255, mixVal );
	
	if(!showingIntro && !showingClusterMap){
		if(currentVisualSystem != NULL)	currentVisualSystem->selfPostDraw();
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
	}
}

#pragma story engine events
//--------------------------------------------------------------------
void CloudsPlaybackController::actCreated(CloudsActEventArgs& args){
	playAct(args.act);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::actBegan(CloudsActEventArgs& args){
	
	rgbdVisualSystem.playSystem();
	rgbdVisualSystem.loadPresetGUISFromName("Test_");
}

//--------------------------------------------------------------------
void CloudsPlaybackController::actEnded(CloudsActEventArgs& args){
	
	cout << "ACT ENDED TRIGGERED" << endl;
	
	rgbdVisualSystem.stopSystem();
	
	//TODO: Trigger cluster map with new updates
	clusterMapVisualSystem.playSystem();
	clusterMapVisualSystem.loadPresetGUISFromName("DefaultCluster");
	
	showingClusterMap = true;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::clipBegan(CloudsClipEventArgs& args){
	playClip(args.chosenClip);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::visualSystemBegan(CloudsVisualSystemEventArgs& args)
{
	if(!showingVisualSystem){
		cout << "Received show visual system" << endl;
		
		//****************************************
		//JG TEMP SYSTEM HACK -- LARS PLEASE DELETE THIS WHEN YOU HAVE IT WORKING
//		TEMP_SYSTEM_HACK = args.preset.system;
//		rgbdVisualSystem.stopSystem();
//		args.preset.system->setDrawToScreen( false );
//		args.preset.system->setCurrentTopic( currentTopic );
//		args.preset.system->playSystem();
//		args.preset.system->loadPresetGUISFromName( args.preset.presetName );
//
//		showingVisualSystem = true;
//		return;
		//****************************************END HACK
		
		nextPresetName = args.preset.presetName;
		
		float duration = args.preset.introDuration == 0.?  1 : args.preset.introDuration;
		
		//set the nextSystem pointer that we'll switch to when the fade ends
		showVisualSystem( args.preset, duration );
	}
	else{
		ofLogError() << "Triggered visual system while still showing one";
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::visualSystemEnded(CloudsVisualSystemEventArgs& args)
{
	if(showingVisualSystem){
		
		//****************************************
		//JG TEMP SYSTEM HACK -- LARS PLEASE DELETE THIS WHEN YOU HAVE IT WORKING
//		TEMP_SYSTEM_HACK->stopSystem();
//		TEMP_SYSTEM_HACK = NULL;
//		rgbdVisualSystem.playSystem();
//		rgbdVisualSystem.loadPresetGUISFromName("Test_");
//		showingVisualSystem = false;
//		return;
		//**************************************** END HACK
		
		cout << "visualSystemEnded "<< ofGetElapsedTimef() <<endl<<endl<<endl<<endl;
		
		//JG: Timing thing. If the system is indefinite, and has an outro then it most likely was created with
		//a "middle" flag, which would stop the timeline. so when the system is ready to fade out let's play it again to
		//watch the outro		
		if(args.preset.outroDuration > 0 && args.preset.indefinite){
			args.preset.system->getTimeline()->play();
		}
		
//		fadeOutVisualSystem( args.preset.outroDuration );
		
		float duration = 1;//(args.preset.outroDuration != 0)? args.preset.outroDuration : 1;
		addControllerTween( fadeOutVisualSystem, ofGetElapsedTimef(), duration, 1, 0, &crossfadeValue );

//		//if we have a currentSystem transition the rgbd using it's transition type
//		if(currentVisualSystem != NULL){
//
//			//to change the rgbdVisual system fade out time change: rgbdVisualSystemFadeOutDuration = 2; //fade time
//			//transition-in the rgbd visual system after the system fades out
//			rgbdVisualSystem.transitionIn( currentVisualSystem->getTransitionType(), rgbdVisualSystemFadeOutDuration, ofGetElapsedTimef() + args.preset.outroDuration );
//			
//		}
	}
	else{
		ofLogError() << "Hiding visual system while none is showing";
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::questionAsked(CloudsQuestionEventArgs& args){
	rgbdVisualSystem.addQuestion(args.questionClip);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::topicChanged(string& args){
	currentTopic = args;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::preRollRequested(CloudsPreRollEventArgs& args){
	cout << "PREROLLING CLIP TO " << 1. - args.handleLength << endl;
	prerollClip(args.preRollClip, 1. - args.handleLength);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::prerollClip(CloudsClip& clip, float toTime){
	if(!clip.hasCombinedVideo){
		ofLogError() << "CloudsPlaybackController::prerollClip -- clip " << clip.getLinkName() << " doesn't have combined video";
		return;
	}
	
	if(!rgbdVisualSystem.getRGBDVideoPlayer().setup( clip.combinedVideoPath, clip.combinedCalibrationXMLPath, toTime) ){
		ofLogError() << "CloudsPlaybackController::prerollClip Error prerolling clip " << clip.getLinkName() << " file path " << clip.combinedVideoPath;
		return;
	}
	
	prerolledClipID = clip.getID();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::playClip(CloudsClip& clip)
{
	if(clip.getID() != prerolledClipID){
		prerollClip(clip,1);
	}
	rgbdVisualSystem.setupSpeaker(clip.person, "", clip.name);
	prerolledClipID = "";
	currentClip = clip;
	
	rgbdVisualSystem.getRGBDVideoPlayer().swapAndPlay();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showVisualSystem(CloudsVisualSystemPreset& nextVisualSystem, float transitionDuration)
{
	if(showingVisualSystem){
		ofLogError("CloudsPlaybackController::showVisualSystem") << "Still showing last system";
		hideVisualSystem();
	}
	
	
	cout << "showing " << nextVisualSystem.system->getSystemName() << " Preset: " << nextVisualSystem.presetName << endl;
	
	showingVisualSystem = true;
	
	nextSystem = nextVisualSystem.system;
	
	addControllerTween(fadeOutRGBD, ofGetElapsedTimef(), transitionDuration, 1, 0, &crossfadeValue);
}

void CloudsPlaybackController::playNextVisualSystem()
{	
	nextSystem->loadPresetGUISFromName( nextPresetName );
	nextSystem->setCurrentTopic( currentTopic );
	nextSystem->playSystem();
	currentVisualSystem = nextSystem;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::hideVisualSystem()
{
	if(showingVisualSystem && currentVisualSystem != NULL){
		cout << "hideVisualSystem "<< ofGetElapsedTimef() << endl << endl << endl;
		currentVisualSystem->stopSystem();
		showingVisualSystem = false;
		currentVisualSystem = &rgbdVisualSystem;
		rgbdVisualSystem.playSystem();
	}
}
