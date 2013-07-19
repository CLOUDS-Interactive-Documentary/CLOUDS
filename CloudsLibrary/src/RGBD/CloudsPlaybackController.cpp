
#include "CloudsPlaybackController.h"


CloudsPlaybackController::CloudsPlaybackController(){
	storyEngine = NULL;
	eventsRegistered = false;
	currentVisualSystem = NULL;
	nextVisualSystem = NULL;
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
	sharedRenderTarget.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 4);
	sharedRenderTarget.begin();
	ofClear(0,0,0,0);
	sharedRenderTarget.end();
	
	nextRenderTarget.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 4);
	nextRenderTarget.begin();
	ofClear(0,0,0,0);
	nextRenderTarget.end();
	
	
	
	if(!eventsRegistered){
		
		eventsRegistered = true;
		
		ofRemoveListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		ofRemoveListener(ofEvents().update, this, &CloudsPlaybackController::update);

		ofAddListener(ofEvents().update, this, &CloudsPlaybackController::update);
		ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
		
		
		//LB:: pointing to our our sharedRndertarget
		rgbdVisualSystem.sharedRenderTarget = &sharedRenderTarget;

		rgbdVisualSystem.setRenderer(combinedRenderer);
		rgbdVisualSystem.setup();
		rgbdVisualSystem.setDrawToScreen( false );
		
		combinedRenderer.setShaderPath( getDataPath() + "shaders/rgbdcombined");
		
		//just temporary. we'll need a better fade event setup with more triggers
		fadeDuration = 1000;
		fadeStartTime = ofGetElapsedTimeMillis();
		fadeEndTime = fadeStartTime + fadeDuration;
		fadeStartVal = 0;
		fadeTargetVal = 1.;
		
		fadingOut = fadingIn = false;
		crossfadeValue = 1.;
		
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

//--------------------------------------------------------------------
void CloudsPlaybackController::playAct(CloudsAct* act){

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
		combinedRenderer.reloadShader();
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
	
	combinedRenderer.update();
	
	updateVisualSystemCrossFade();
	
}

void CloudsPlaybackController::updateVisualSystemCrossFade(){
	//handle fadin/out
	if( fadingIn || fadingOut){
		int currentTime = ofGetElapsedTimeMillis();
		
		crossfadeValue = ofxTween::map( currentTime, fadeStartTime, fadeEndTime, fadeStartVal, fadeTargetVal, true, fadeEase,  ofxTween::easeInOut );
		
		
		//end fading in
		if( fadingIn && currentTime > fadeEndTime ){
			//end fade and stop the other system
			fadingIn = false;
			rgbdVisualSystem.stopSystem();
			
			//use our currentVisualSystem's camera( no more need for the super camera anymore )
			currentVisualSystem->setCurrentCamera( currentVisualSystem->getCameraRef() );
		}
		
		//end fading out
		else if( fadingOut && currentTime > fadeEndTime ){
			//end fade and stop the other system
			fadingOut = false;
			hideVisualSystem();
			
			//use our currentVisualSystem's camera( no need for the super camera anymore )
			rgbdVisualSystem.setCurrentCamera( rgbdVisualSystem.getCameraRef() );
		}
		
		//otherwise we're fading and we need to mix our cameras
		else{
			
			//get the position offset for the currentVisualSystem
			ofVec3f currentSystemPositionOffsest = currentVisualSystem->getCameraPosition() - nextCameraStartPosition;
			
			//mix the attributes ffrom our vis system cameras to build our superCamera
			mixCameras(&superCamera,						//targetCamera
					   rgbdVisualSystem.getCameraRef(),		//ofCamera0
					   currentVisualSystem->getCameraRef(),	//ofCamera1
					   crossfadeValue,						//mixValue
					   ofVec3f(),							//possOffset0
					   currentSystemPositionOffsest );		//possOffset1
			
			//set the visual systems' current camera to our superCamera
			currentVisualSystem->setCurrentCamera( superCamera );
			rgbdVisualSystem.setCurrentCamera( superCamera );
			
		}
		
	}
}

void CloudsPlaybackController::mixCameras(ofCamera* targetCam,
										  ofCamera* c0,
										  ofCamera*  c1,
										  float x,
										  ofVec3f posOffset0,
										  ofVec3f posOffset1 )
{
	
	//get inverse val
	float mx = 1. - x;
	
	//projection stuff
	targetCam->setupPerspective(false,													//bool vFlip
								c0->getFov()*x			+	c1->getFov()*mx,			//float fov
								c0->getNearClip()*x		+	c1->getNearClip()*mx,		//float nearDist
								c0->getFarClip()*x		+	c1->getFarClip()*mx,		//float farDist
								c0->getLensOffset()*x	+	c1->getLensOffset()*mx );	//const ofVec2f & lensOffset
	
	//position, rotation, are we missing something else here?
	targetCam->setPosition( (c0->getPosition()+posOffset0)*x + (c1->getPosition()+posOffset1)*mx );
	ofQuaternion rot;
	rot.slerp( mx, c0->getOrientationQuat(), c1->getOrientationQuat() );
	targetCam->setOrientation( rot );
	
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::draw(ofEventArgs & args){

//	LB:: we do our mixing here?
	
//	ofClear(0, 0, 0, 0);
	
    
	//turn off depth testing and enable blending
	//TODO:: add blending modes to GUI?
    glDisable( GL_DEPTH_TEST );
	ofEnableBlendMode(	OF_BLENDMODE_ADD );
	
	int mixVal = 255 * crossfadeValue;
	
	ofSetColor( 255, 255, 255, mixVal );
	rgbdVisualSystem.selfPostDraw();
	
	ofSetColor( 255, 255, 255, 255 - mixVal );
	if(currentVisualSystem != NULL)	currentVisualSystem->selfPostDraw();
	
    ofDisableBlendMode();
    glEnable( GL_DEPTH_TEST );
	
	
	if(currentAct != NULL && ofGetKeyPressed('-')){
		currentAct->drawDebug();
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
}

//--------------------------------------------------------------------
void CloudsPlaybackController::actEnded(CloudsActEventArgs& args){
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::clipBegan(CloudsClipEventArgs& args){
	playClip(args.chosenClip);
	
	//this has to draw last
	ofRemoveListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
	ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw);	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	if(!showingVisualSystem){
		cout << "Received show visual system" << endl;
		showVisualSystem(args.preset);
	}
	else{
		ofLogError() << "Triggered visual system while still showing one";
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	if(showingVisualSystem){
		fadeOutVisualSystem();
//		hideVisualSystem(); TODO:: is it ok to swap this with fadeOutVisSys OK?
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
void CloudsPlaybackController::playClip(CloudsClip& clip){

	if(clip.hasCombinedVideo){
		if(combinedRenderer.setup( clip.combinedVideoPath, clip.combinedCalibrationXMLPath) ){
			combinedRenderer.getPlayer().play();
			rgbdVisualSystem.setupSpeaker(clip.person, "", clip.name);
			currentClip = clip;
		}
		else{
			ofLogError() << "CloudsPlaybackController::playClip -- folder " << clip.combinedVideoPath << " is not valid";
		}
	}
	else {
		ofLogError() << "CloudsPlaybackController::playClip -- clip " << clip.getLinkName() << " doesn't have combined video";
	}
}


//--------------------------------------------------------------------
void CloudsPlaybackController::showVisualSystem(CloudsVisualSystemPreset& nextVisualSystem){
	
//	if(simplePlaybackMode) return;
	
	if(showingVisualSystem){
//		hideVisualSystem();
		fadeOutVisualSystem();
	}
	
	cout << "showing " << nextVisualSystem.system->getSystemName() << " Preset: " << nextVisualSystem.presetName << endl << endl<< endl;
	

//	rgbdVisualSystem.stopSystem();
	
	

	nextVisualSystem.system->sharedRenderTarget = &nextRenderTarget;
	
	//we draw to screen in CloudsPlaybackController::draw() so we disable it in the nexVisualSystem
	nextVisualSystem.system->setDrawToScreen( false );
	
	//TODO: replace with act current question
	nextVisualSystem.system->setCurrentTopic( currentTopic );
	nextVisualSystem.system->playSystem();
	nextVisualSystem.system->loadPresetGUISFromName( nextVisualSystem.presetName );
	
	showingVisualSystem = true;
	
	currentVisualSystem = nextVisualSystem.system;
	
	nextCameraStartPosition = currentVisualSystem->getCameraPosition();
	
	fadeInVisualSystem();
}

//--------------------------------------------------------------------
void CloudsPlaybackController::hideVisualSystem(){

	if(showingVisualSystem && currentVisualSystem != NULL){
		currentVisualSystem->stopSystem();
		showingVisualSystem = false;
		
		currentVisualSystem->stopSystem();
		currentVisualSystem = NULL;
				
		//rgbdVisualSystem.playSystem();// fade in instead
	}
}

void CloudsPlaybackController::fadeInVisualSystem(){
	
	fadingIn = true;
	fadingOut = false;
	
	//set crossfade
	fadeDuration = 3000;
	fadeStartTime = ofGetElapsedTimeMillis();
	fadeEndTime = fadeStartTime + fadeDuration;
	fadeStartVal = 1.;
	fadeTargetVal = 0;
	
}

void CloudsPlaybackController::fadeOutVisualSystem(){
	
	fadingIn = false;
	fadingOut = true;
	
	//set crossfade
	fadeDuration = 3000;
	fadeStartTime = ofGetElapsedTimeMillis();
	fadeEndTime = fadeStartTime + fadeDuration;
	fadeStartVal = 0;
	fadeTargetVal = 1.;
	
	rgbdVisualSystem.playSystem();
}
