
#include "CloudsPlaybackController.h"


CloudsPlaybackController::CloudsPlaybackController(){
	storyEngine = NULL;
	eventsRegistered = false;
	currentVisualSystem = NULL;
	showingVisualSystem = false;
	currentAct = NULL;
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
//	nextRenderTarget.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
//	nextRenderTarget.begin();
//	ofClear(0,0,0,0);
//	nextRenderTarget.end();
	
	if(!eventsRegistered){
		
		eventsRegistered = true;
		
		ofRemoveListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		ofRemoveListener(ofEvents().update, this, &CloudsPlaybackController::update);

		ofAddListener(ofEvents().update, this, &CloudsPlaybackController::update);
		ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
		
		//pointing to our our sharedRenderTarget
		//rgbdVisualSystem.sharedRenderTarget = &sharedRenderTarget;
		
		//rgbdVisualSystem.setRenderer(combinedRenderer);
		rgbdVisualSystem.setup();
		rgbdVisualSystem.setDrawToScreen( false );
		currentVisualSystem = &rgbdVisualSystem;
		
		introSequence.setup();

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
//	srand( ofGetSeconds()*1000 );
//	CloudsClip& clip = parser.getRandomClip(true,true);
	introSequence.setStartQuestions(possibleStartQuestions);
	introSequence.loadPresetGUISFromName("TunnelWarp");
	introSequence.playSystem();
	showingIntro = true;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::playAct(CloudsAct* act){

	//TODO this shouldn't happen when runs are set up
	if(currentAct != NULL){
		currentAct->unregisterEvents(this);
		delete currentAct;
	}
	
	//TODO: show loading screen while we initialize all the visual systems
	vector<CloudsVisualSystem*> systems = act->getAllVisualSystems();
	for(int i = 0; i < systems.size(); i++){
		if(systems[i] != NULL){
			cout << "CloudsPlaybackController::playAct -- Setting up:: " << systems[i]->getSystemName() << endl;
			systems[i]->setup();
		}
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
				storyEngine->buildAct(clip, q->topic );

			}
			//Transition out of the act into the loading screen.

		}
	}
	else {
		updateVisualSystemFade();
	}
	
	//TODO: add camera animations to RGBDVisSys
	
	//TODO: offsetTargets for turning away
	
}


void CloudsPlaybackController::updateVisualSystemFade(){
	//handle fadin/out
	if( fadingIn || fadingOut){
		float currentTime = ofGetElapsedTimef();
		
		crossfadeValue = ofxTween::map( currentTime, fadeStartTime, fadeEndTime, fadeStartVal, fadeTargetVal, true, fadeEase,  ofxTween::easeInOut );
		
		//end fading in
		if( fadingIn && currentTime > fadeEndTime ){
			cout  << "FADED IN " << ofGetElapsedTimef() << endl;
			bIsFading = false;

			//end fade
			fadingIn = false;
		}
		
		//end fading out
		else if( fadingOut && currentTime > fadeEndTime ){
			
			cout  << "FADED OUT " << ofGetElapsedTimef() << endl;
			
			//fade in the new visual system
			if(nextSystem != NULL){
				//non RGBD visual system
				currentVisualSystem = nextSystem;
				nextSystem = NULL;
				
				fadeInVisualSystem(1);
			}
			else{
				//back to the RGBD visual system
				hideVisualSystem();
				fadeInVisualSystem(1);
			}
			
			//end fade
			fadingOut = false;
		}
		
		//otherwise we're fading and we need to mix our cameras
		else{
			//shout out to the faded
			if(!bIsFading && currentTime >= fadeStartTime){
				bIsFading = true;
			}
		}
		
	}
}

void CloudsPlaybackController::mixCameras(ofCamera* targetCam,
										  ofCamera* c0,
										  ofCamera*  c1,
										  float x )
{
	
	//get inverse val
	float mx = 1. - x;
	
	//projection stuff
	targetCam->setupPerspective(false,												//bool vFlip
								c0->getFov()*x			+	c1->getFov()*mx,			//float fov
								c0->getNearClip()*x	+	c1->getNearClip()*mx,			//float nearDist
								c0->getFarClip()*x		+	c1->getFarClip()*mx,		//float farDist
								c0->getLensOffset()*x	+	c1->getLensOffset()*mx );	//const ofVec2f & lensOffset
	
	//position, rotation, are we missing something else here?
	targetCam->setPosition( c0->getPosition()*x + c1->getPosition()*mx );
	ofQuaternion rot;
	rot.slerp( mx, c0->getOrientationQuat(), c1->getOrientationQuat() );
	targetCam->setOrientation( rot );
}

//--------------------------------------------------------------------
void CloudsPlaybackController::draw(ofEventArgs & args){
    
	//turn off depth testing and enable blending
    glDisable( GL_DEPTH_TEST );
	
	ofPushStyle();
	
	//???: rgbdVisualSystem.getBlendMode()
	ofEnableBlendMode(	OF_BLENDMODE_ADD );
	
	float mixVal = crossfadeValue * 255;
	
	ofSetColor( 255, 255, 255, mixVal );
	
	if(currentVisualSystem != NULL)	currentVisualSystem->selfPostDraw();
	
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
	//this has to draw last
//	ofRemoveListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
//	ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::actEnded(CloudsActEventArgs& args){
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::clipBegan(CloudsClipEventArgs& args)
{
	playClip(args.chosenClip);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::visualSystemBegan(CloudsVisualSystemEventArgs& args)
{
	
	if(!showingVisualSystem){
		cout << "Received show visual system" << endl;
		
		float duration = args.preset.introDuration == 0.?  1 : args.preset.introDuration;
		
		//set the nextSystem pointer that we'll switch to when the fade ends
		showVisualSystem( args.preset, duration );
		
		//start the generic fade
		fadeOutVisualSystem( duration );
		
		//if there is a nextSystem transition out the rgbd using the next system's transition type
		if (nextSystem != NULL) {
			
			//transition-in the rgbd visual system
			rgbdVisualSystem.transitionOut( nextSystem->getTransitionType(), duration );
		}
	}
	else{
		ofLogError() << "Triggered visual system while still showing one";
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::visualSystemEnded(CloudsVisualSystemEventArgs& args)
{
	if(showingVisualSystem){
		
		cout << "visualSystemEnded "<< ofGetElapsedTimef() <<endl<<endl<<endl<<endl;

		//JG: Timing thing. If the system is indefinite, and has an outro then it most likely was created with
		//a "middle" flag, which would stop the timeline. so when the system is ready to fade out let's play it again to
		//watch the outro
		
		if(args.preset.outroDuration > 0 && args.preset.indefinite){
			args.preset.system->getTimeline()->play();
		}
		
		fadeOutVisualSystem( args.preset.outroDuration );
		
		//if we have a currentSystem transition the rgbd using it's transition type
		if(currentVisualSystem != NULL){
			
			//???: how long do we transition into the rgbdSystem? it's 2 for now...
			
			//transition-in the rgbd visual system after the system fades out
			rgbdVisualSystem.transitionIn( currentVisualSystem->getTransitionType(), 2, ofGetElapsedTimef() + args.preset.outroDuration );
			
		}
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
		hideVisualSystem();
	}
	
	cout << "showing " << nextVisualSystem.system->getSystemName() << " Preset: " << nextVisualSystem.presetName << endl << endl<< endl;

//	nextVisualSystem.system->sharedRenderTarget = &nextRenderTarget;
	
	//we draw to screen in CloudsPlaybackController::draw() so we disable it in the nexVisualSystem
	nextVisualSystem.system->setDrawToScreen( false );
	
	//TODO: replace with act current question
	nextVisualSystem.system->setCurrentTopic( currentTopic );
	nextVisualSystem.system->loadPresetGUISFromName( nextVisualSystem.presetName );
	nextVisualSystem.system->playSystem();
	
	showingVisualSystem = true;
	
	currentVisualSystem = nextVisualSystem.system;
		
	cameraStartPos = currentVisualSystem->getCameraRef().getPosition();
	
	transitionRgbdSystemOut( 3, 3 );// transitionDuration, transitionDuration );
}

//--------------------------------------------------------------------
void CloudsPlaybackController::hideVisualSystem()
{

	if(showingVisualSystem && currentVisualSystem != NULL){
		cout << "hideVisualSystem "<< ofGetElapsedTimef() << endl<< endl<< endl;
		currentVisualSystem->stopSystem();
		showingVisualSystem = false;
		currentVisualSystem = &rgbdVisualSystem;
		rgbdVisualSystem.playSystem();
	}
}


void CloudsPlaybackController::fadeInVisualSystem( float duration, float start )
{
	cout << "fadeInVisualSystem " << ofGetElapsedTimef() << endl;
	
	fadingIn = true;
	fadingOut = false;
	
	//set crossfade
	fadeDuration = duration;
	fadeStartTime = start;
	fadeEndTime = fadeStartTime + fadeDuration;
	fadeStartVal = 0;
	fadeTargetVal = 1;
	
}

void CloudsPlaybackController::fadeOutVisualSystem( float duration, float start )
{
	cout << "fadeOutVisualSystem " << ofGetElapsedTimef() << endl;
 	
	//handle the fading
	fadingIn = false;
	fadingOut = true;
	
	//set crossfade
	fadeDuration = duration;
	fadeStartTime = start;
	fadeEndTime = fadeStartTime + fadeDuration;
	fadeStartVal = 1;
	fadeTargetVal = 0.;
}


void CloudsPlaybackController::transitionRgbdSystemOut( float transitionDuration, float fadeDuration )
{
	cout << "transitionRgbdSystemOut: " << ofGetElapsedTimef() << endl;
	
	//start our rgbSystem's transition
	rgbdVisualSystem.transitionOut( currentVisualSystem->getTransitionType(), transitionDuration );
	
	//start our fade
	fadeInVisualSystem( transitionDuration, ofGetElapsedTimef() );
}

void CloudsPlaybackController::transitionRgbdSystemIn( float transitionDuration, float fadeDuration )
{
	cout << endl << "transitionRgbdSystemIn: " << ofGetElapsedTimef() << endl<< endl;
	
	//start our rgbSystem's transition
	rgbdVisualSystem.transitionIn( currentVisualSystem->getTransitionType(), transitionDuration );
	
	//start our fade
	fadeOutVisualSystem( transitionDuration, ofGetElapsedTimef() );
}
