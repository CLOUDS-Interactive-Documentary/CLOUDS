
#include "CloudsPlaybackController.h"


CloudsPlaybackController::CloudsPlaybackController(){
	eventsRegistered = false;
	currentVisualSystem = NULL;
	showingVisualSystem = false;
}

CloudsPlaybackController::~CloudsPlaybackController(){
}

//--------------------------------------------------------------------
void CloudsPlaybackController::exit(ofEventArgs & args){
	if(eventsRegistered){
		eventsRegistered = false;
		ofRemoveListener(storyEngine->getEvents().storyBegan, this, &CloudsPlaybackController::storyBegan);
		ofRemoveListener(storyEngine->getEvents().clipBegan, this, &CloudsPlaybackController::clipBegan);
		ofRemoveListener(storyEngine->getEvents().visualSystemBegan, this, &CloudsPlaybackController::visualSystemBegan);
		ofRemoveListener(storyEngine->getEvents().visualSystemEnded, this, &CloudsPlaybackController::visualSystemEnded);
		
		ofRemoveListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
		
		ofUnregisterMouseEvents(this);
		ofUnregisterKeyEvents(this);
		
//		delete visualSystemControls;
//		delete visualSystemDD;
//		delete keyThemesDD;
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::setup(CloudsStoryEngine& storyEngine){
	if(!eventsRegistered){
//		camera.setup();
//		camera.autosavePosition = true;
//		camera.loadCameraPosition();
		

		this->storyEngine = &storyEngine;
		
		ofAddListener(storyEngine.getEvents().storyBegan, this, &CloudsPlaybackController::storyBegan);
		ofAddListener(storyEngine.getEvents().clipBegan, this, &CloudsPlaybackController::clipBegan);
		ofAddListener(storyEngine.getEvents().visualSystemBegan, this, &CloudsPlaybackController::visualSystemBegan);
		ofAddListener(storyEngine.getEvents().visualSystemEnded, this, &CloudsPlaybackController::visualSystemEnded);
	
		ofAddListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
		
//		populateVisualSystems();

		rgbdVisualSystem.setRenderer(combinedRenderer);
		rgbdVisualSystem.setup();
		
		eventsRegistered = true;

		//combinedRenderer.setShaderPath("../../../CloudsData/shaders/rgbdcombined");
		combinedRenderer.setShaderPath( CloudsVisualSystem::getDataPath() + "shaders/rgbdcombined");
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
void CloudsPlaybackController::update(){

	
	combinedRenderer.update();
	
	if(combinedRenderer.isDone() && !storyEngine->isWaiting()){
		rgbdVisualSystem.speakerEnded();
		storyEngine->clipEnded();
	}
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::draw(){
	
	storyEngine->drawStoryEngineDebug();
	
//	if(!showingVisualSystem && combinedRenderer.isPlaying()){
//		camera.begin();
//		combinedRenderer.drawPointCloud();
//		camera.end();
//	}
	
}

#pragma story engine
//--------------------------------------------------------------------
void CloudsPlaybackController::storyBegan(CloudsStoryEventArgs& args){
	
	rgbdVisualSystem.playSystem();
	
	playClip(args.chosenClip);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::clipBegan(CloudsStoryEventArgs& args){
	playClip(args.chosenClip);
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
	
	if(clip.hasCombinedVideo){
		if(combinedRenderer.setup( clip.combinedVideoPath, clip.combinedCalibrationXMLPath) ){
			combinedRenderer.getPlayer().play();
			rgbdVisualSystem.setupSpeaker(clip.person, "", clip.name);
			currentClip = clip;
		}
		else{
			ofLogError() << "RGBD LOAD : folder " << clip.combinedVideoPath << " is not valid" << endl;
		}
	}
	else {
		ofLogError() << "RGBD LOAD : clip " << clip.getLinkName() << " doesn't have combined video" << endl;
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showVisualSystem(){

	/*
	//get the visual system that is closest to this tag
	string keyTheme = storyEngine->network->getKeyThemeForTag(storyEngine->getCurrentTopic());

	for(int i = 0; i < visualSystems.size(); i++){
		if(visualSystems[i]->isReleventToKeyword(keyTheme)){
			showVisualSystem(visualSystems[i], keyTheme);
			cout << "selected visual system " << currentVisualSystem->getSystemName() << " for topic " << storyEngine->getCurrentTopic() << " and key theme " << keyTheme << endl;
			return;
		}
	}
	
	ofLogError() << "No visual systems found for topic: " << storyEngine->getCurrentTopic() << " picking random"<<endl;

	//pick a random one
	showVisualSystem(visualSystems[ int(ofRandom(visualSystems.size())) ], storyEngine->getCurrentTopic());
	*/
	
	//For now show a random system!
//	showVisualSystem(visualSystems[ int(ofRandom(visualSystems.size())) ], storyEngine->getCurrentTopic());
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showVisualSystem(CloudsVisualSystemPreset& nextVisualSystem){
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
		
//		visualSystemControls->enable();
//		if(keyThemesPanel != NULL) keyThemesPanel->enable();
		
		rgbdVisualSystem.playSystem();
	}
}

//--------------------------------------------------------------------
//CloudsVisualSystem* CloudsPlaybackController::visualSystemWithName(string systemName){
//	if(nameToVisualSystem.find(systemName) != nameToVisualSystem.end()){
//		return nameToVisualSystem[systemName];
//	}
//	ofLogError() << "Could not find Visual System with name " << systemName;
//	return NULL;
//}

//--------------------------------------------------------------------
//void CloudsPlaybackController::guiEvent(ofxUIEventArgs &e)
//{
//    string name = e.widget->getName();
//    
////    cout << "WIDGET NAME: " << name << endl;
//    
//    if(e.widget->getParent() == visualSystemRadio){
//		if(visualSystemRadio->getActive() != NULL){
//						
//			CloudsVisualSystem* system = visualSystemWithName( name );
//			if(system != NULL){
//				if(keyThemesPanel != NULL){
//					delete keyThemesPanel;
//				}
//				keyThemesPanel = new ofxUICanvas(0, visualSystemControls->getRect()->getMaxY(), 0, 0);
//				vector<string>& relevantKeys = system->getRelevantKeywords();
//				if(relevantKeys.size() == 0){
//					relevantKeys.push_back("no-key");
//				}
//				keyThemesRadio = keyThemesPanel->addRadio("KEY THEMES", relevantKeys, OFX_UI_ORIENTATION_VERTICAL, 16, 16);
//				
//				keyThemesPanel->setTheme(OFX_UI_THEME_COOLCLAY);
//				playButton = keyThemesPanel->addButton("Test System", &triggerVisualSystem);
//			
//				keyThemesPanel->autoSizeToFitWidgets();
//				ofAddListener(keyThemesPanel->newGUIEvent, this, &CloudsPlaybackController::guiEvent);
//			}
//		}
//    }
//	else if(e.widget == playButton && playButton->getValue() && !showingVisualSystem){
//		
//		cout << "Showing visual system!" << endl;
//		
//		showVisualSystem(visualSystemWithName(visualSystemRadio->getActive()->getName()),
//						 keyThemesRadio->getActive()->getName());
//	}
//}
