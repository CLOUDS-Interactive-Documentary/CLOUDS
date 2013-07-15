
#include "CloudsPlaybackController.h"


CloudsPlaybackController::CloudsPlaybackController(){
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
		ofRemoveListener(storyEngine->getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
	}
	
	if(currentAct != NULL){
		currentAct->unregisterEvents(this);
		delete currentAct;
	}
}


//--------------------------------------------------------------------
void CloudsPlaybackController::setup(CloudsStoryEngine& storyEngine){
	if(!eventsRegistered){
		
		this->storyEngine = &storyEngine;
		
		ofAddListener(storyEngine.getEvents().actCreated, this, &CloudsPlaybackController::actCreated);
		eventsRegistered = true;
		
		ofRemoveListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		ofRemoveListener(ofEvents().update, this, &CloudsPlaybackController::update);

		ofAddListener(ofEvents().update, this, &CloudsPlaybackController::update);
		ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);

		rgbdVisualSystem.setRenderer(combinedRenderer);
		rgbdVisualSystem.setup();
		
		combinedRenderer.setShaderPath( getDataPath() + "shaders/rgbdcombined");
	}
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
	
	if(args.key == OF_KEY_RIGHT){
		//replace with some call to act
//		storyEngine->playNextClip();
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
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::draw(ofEventArgs & args){

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
		hideVisualSystem();
	}
	else{
		ofLogError() << "Hiding visual system while none is showing";
	}	
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
		hideVisualSystem();
	}
	
	cout << "showing " << nextVisualSystem.system->getSystemName() << " Preset: " << nextVisualSystem.presetName << endl;
	
	rgbdVisualSystem.stopSystem();
	
	//TODO: replace with act current question
	nextVisualSystem.system->setCurrentTopic( currentTopic );
	nextVisualSystem.system->playSystem();
	nextVisualSystem.system->loadPresetGUISFromName( nextVisualSystem.presetName );
	
	currentVisualSystem = nextVisualSystem.system;
	showingVisualSystem = true;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::hideVisualSystem(){
	if(showingVisualSystem && currentVisualSystem != NULL){
		currentVisualSystem->stopSystem();
		showingVisualSystem = false;
		currentVisualSystem = NULL;
				
		rgbdVisualSystem.playSystem();
	}
}
