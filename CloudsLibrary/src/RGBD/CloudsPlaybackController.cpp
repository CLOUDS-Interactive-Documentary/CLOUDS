
#include "CloudsPlaybackController.h"


CloudsPlaybackController::CloudsPlaybackController(){
	eventsRegistered = false;
	currentVisualSystem = NULL;
	showingVisualSystem = false;
	simplePlaybackMode = false;
	
}

CloudsPlaybackController::~CloudsPlaybackController(){
}

//--------------------------------------------------------------------
void CloudsPlaybackController::exit(ofEventArgs & args){
	if(eventsRegistered){
		eventsRegistered = false;
		
		ofRemoveListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		ofRemoveListener(ofEvents().update, this, &CloudsPlaybackController::update);
		
		ofRemoveListener(storyEngine->getEvents().storyBegan, this, &CloudsPlaybackController::storyBegan);
		ofRemoveListener(storyEngine->getEvents().clipBegan, this, &CloudsPlaybackController::clipBegan);
		ofRemoveListener(storyEngine->getEvents().visualSystemBegan, this, &CloudsPlaybackController::visualSystemBegan);
		ofRemoveListener(storyEngine->getEvents().visualSystemEnded, this, &CloudsPlaybackController::visualSystemEnded);
		
		ofRemoveListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
		
		ofUnregisterMouseEvents(this);
		ofUnregisterKeyEvents(this);
		
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::setup(CloudsStoryEngine& storyEngine){
	if(!eventsRegistered){
		
		this->storyEngine = &storyEngine;
		
		ofAddListener(storyEngine.getEvents().storyBegan, this, &CloudsPlaybackController::storyBegan);
		ofAddListener(storyEngine.getEvents().clipBegan, this, &CloudsPlaybackController::clipBegan);
		ofAddListener(storyEngine.getEvents().visualSystemBegan, this, &CloudsPlaybackController::visualSystemBegan);
		ofAddListener(storyEngine.getEvents().visualSystemEnded, this, &CloudsPlaybackController::visualSystemEnded);
	
		ofAddListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
		
		ofAddListener(ofEvents().update, this, &CloudsPlaybackController::update);
		ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
		
		eventsRegistered = true;

		if(!simplePlaybackMode){
			rgbdVisualSystem.setRenderer(combinedRenderer);
			rgbdVisualSystem.setup();
			combinedRenderer.setShaderPath( CloudsVisualSystem::getDataPath() + "shaders/rgbdcombined");
		}

	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::keyPressed(ofKeyEventArgs & args){
	
	if(args.key == 'R'){
		combinedRenderer.reloadShader();
	}
	
	if(args.key == OF_KEY_RIGHT){
		storyEngine->playNextClip();
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
	
	//SIMPLE MODE
	if(simplePlaybackMode){
		if(!storyEngine->isWaiting() &&
		   combinedRenderer.getPlayer().isLoaded() &&
		   combinedRenderer.getPlayer().isPlaying() &&
		   combinedRenderer.getPlayer().getCurrentFrame() >= currentClip.endFrame)
		{
			storyEngine->clipEnded();
			combinedRenderer.getPlayer().stop();
		}
		return;
	}
	
	//END SIMPLE MODE
	if(combinedRenderer.isDone() && !storyEngine->isWaiting()){
		rgbdVisualSystem.speakerEnded();
		storyEngine->clipEnded();
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::draw(ofEventArgs & args){
	if(simplePlaybackMode){
		storyEngine->drawStoryEngineDebug();
		combinedRenderer.getPlayer().draw(0, 0, 960, 540);
	}
}

#pragma story engine
//--------------------------------------------------------------------
void CloudsPlaybackController::storyBegan(CloudsStoryEventArgs& args){
	
	if(!simplePlaybackMode){
		rgbdVisualSystem.playSystem();
	}
	
	playClip(args.chosenClip);	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::clipBegan(CloudsStoryEventArgs& args){
	playClip(args.chosenClip);
	
	//this has to draw last
	ofRemoveListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
	ofAddListener(ofEvents().draw, this, &CloudsPlaybackController::draw);
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	if(!showingVisualSystem){
		cout << "Received show visual system" << endl;
		//showVisualSystem();
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
void CloudsPlaybackController::playClip(CloudsClip& clip){
		
	//SIMPLE PLAYBACK
	if(simplePlaybackMode){
		if(!combinedRenderer.getPlayer().loadMovie( clip.getRelinkedVideoFilePath() )){
			ofLogError() << "CloudsPlaybackController::playClip -- simple mode -- couldn't play back " << clip.getRelinkedVideoFilePath();
		}
		combinedRenderer.getPlayer().setFrame(clip.startFrame);
		combinedRenderer.getPlayer().play();
		currentClip = clip;
		return;
	}
	//END SIMPLE PLAYBACK
	
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
	
	if(simplePlaybackMode) return;
	
	if(showingVisualSystem){
		hideVisualSystem();
	}
	
	cout << "showing " << nextVisualSystem.system->getSystemName() << " Preset: " << nextVisualSystem.presetName << endl;
	
	rgbdVisualSystem.stopSystem();
	
	nextVisualSystem.system->setCurrentTopic( storyEngine->getCurrentTopic() );
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
