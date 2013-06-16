
#include "CloudsPlaybackController.h"

#include "CloudsVisualSystemRezanator.h"
#include "CloudsVisualSystemComputationTicker.h"
#include "CloudsVisualSystemLSystems.h"
#include "CloudsVisualSystemVoro.h"
#include "CloudsVisualSystemVerletForm.h"
#include "CloudsVisualSystemCities.h"
#include "CloudsVisualSystemAmber.h"
#include "CloudsVisualSystemCollaboration1.h"

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
		camera.setup();
		camera.autosavePosition = true;
		camera.loadCameraPosition();
		
		this->storyEngine = &storyEngine;
		
		ofAddListener(storyEngine.getEvents().storyBegan, this, &CloudsPlaybackController::storyBegan);
		ofAddListener(storyEngine.getEvents().clipBegan, this, &CloudsPlaybackController::clipBegan);
		ofAddListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
		
		populateVisualSystems();

		rgbdVisualSystem.setRenderer(combinedRenderer);
		rgbdVisualSystem.setup();
		
		eventsRegistered = true;

		combinedRenderer.setShaderPath("../../../CloudsData/shaders/rgbdcombined");

	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::keyPressed(ofKeyEventArgs & args){
	
	//TEMPORARY SWITCH!!
	if(args.key == 'v'){
		if(showingVisualSystem){
			hideVisualSystem();
		}
		else{
			showVisualSystem();
		}
	}
	
	if(args.key == 'R'){
		combinedRenderer.reloadShader();
	}
	
	if(args.key == OF_KEY_RIGHT){
		storyEngine->selectNewClip();
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

#pragma visual systems
//--------------------------------------------------------------------
void CloudsPlaybackController::populateVisualSystems(){
	
	ofLogVerbose() << "Populating visual systems";
	
	//registerVisualSystem( new CloudsVisualSystemRezanator() );
	registerVisualSystem( new CloudsVisualSystemComputationTicker() );
	registerVisualSystem( new CloudsVisualSystemLSystems() );
	registerVisualSystem( new CloudsVisualSystemVoro() );
	
	set<string> keyThemes;
	for(int i = 0; i < visualSystems.size(); i++){
		vector<string>& keys = visualSystems[i]->getRelevantKeywords();
		for(int k = 0; k < keys.size(); k++){
			keyThemes.insert( keys[k] );
		}
	}
	
//	storyEngine->network->populateKeyThemes(keyThemes);
	
	//create the drop down
    vector<string> names;
	for(int i = 0; i < visualSystems.size(); i++){
		names.push_back(visualSystems[i]->getSystemName());
	}
	
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
	float length = 320;
		
	visualSystemControls = new ofxUISuperCanvas("VISUAL SYSTEMS");
	visualSystemControls->addWidgetDown(new ofxUILabel("VISUAL SYSTEMS:", OFX_UI_FONT_MEDIUM));
	visualSystemRadio = visualSystemControls->addRadio("VISUAL SYSTEM", names, OFX_UI_ORIENTATION_VERTICAL, 16, 16);
	visualSystemControls->addSlider("Test Duration (S)", 2, 60*5, &timeToTest);
    visualSystemControls->setTheme(OFX_UI_THEME_COOLCLAY);
    visualSystemControls->autoSizeToFitWidgets();
	
    ofAddListener(visualSystemControls->newGUIEvent, this, &CloudsPlaybackController::guiEvent);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::registerVisualSystem(CloudsVisualSystem* system){
	
	ofLogVerbose() << "Registering system " << system->getSystemName();
	
	system->setup();
	visualSystems.push_back( system );
	nameToVisualSystem[system->getSystemName()] = system;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showVisualSystem(){

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
}

//--------------------------------------------------------------------
void CloudsPlaybackController::showVisualSystem(CloudsVisualSystem* nextVisualSystem, string keyTheme){
	if(showingVisualSystem){
		hideVisualSystem();
	}

	rgbdVisualSystem.stopSystem();
	
	nextVisualSystem->setCurrentTopic(storyEngine->getCurrentTopic());
	nextVisualSystem->setCurrentKeyword(keyTheme);
	nextVisualSystem->playSystem();

	currentVisualSystem = nextVisualSystem;
	showingVisualSystem = true;
	
	visualSystemControls->disable();
	if(keyThemesPanel != NULL) keyThemesPanel->disable();

}

//--------------------------------------------------------------------
void CloudsPlaybackController::hideVisualSystem(){
	if(showingVisualSystem && currentVisualSystem != NULL){
		currentVisualSystem->stopSystem();
		showingVisualSystem = false;
		currentVisualSystem = NULL;
		
		visualSystemControls->enable();
		if(keyThemesPanel != NULL) keyThemesPanel->enable();
		
		rgbdVisualSystem.playSystem();
	}
}

//--------------------------------------------------------------------
CloudsVisualSystem* CloudsPlaybackController::visualSystemWithName(string systemName){
	if(nameToVisualSystem.find(systemName) != nameToVisualSystem.end()){
		return nameToVisualSystem[systemName];
	}
	ofLogError() << "Could not find Visual System with name " << systemName;
	return NULL;
}

//--------------------------------------------------------------------
void CloudsPlaybackController::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    
//    cout << "WIDGET NAME: " << name << endl;
    
    if(e.widget->getParent() == visualSystemRadio){
		if(visualSystemRadio->getActive() != NULL){
						
			CloudsVisualSystem* system = visualSystemWithName( name );
			if(system != NULL){
				if(keyThemesPanel != NULL){
					delete keyThemesPanel;
				}
				keyThemesPanel = new ofxUICanvas(0, visualSystemControls->getRect()->getMaxY(), 0, 0);
				vector<string>& relevantKeys = system->getRelevantKeywords();
				if(relevantKeys.size() == 0){
					relevantKeys.push_back("no-key");
				}
				keyThemesRadio = keyThemesPanel->addRadio("KEY THEMES", relevantKeys, OFX_UI_ORIENTATION_VERTICAL, 16, 16);
				
				keyThemesPanel->setTheme(OFX_UI_THEME_COOLCLAY);
				playButton = keyThemesPanel->addButton("Test System", &triggerVisualSystem);
			
				keyThemesPanel->autoSizeToFitWidgets();
				ofAddListener(keyThemesPanel->newGUIEvent, this, &CloudsPlaybackController::guiEvent);
			}
		}
    }
	else if(e.widget == playButton && playButton->getValue() && !showingVisualSystem){
		
		cout << "Showing visual system!" << endl;
		
		showVisualSystem(visualSystemWithName(visualSystemRadio->getActive()->getName()),
						 keyThemesRadio->getActive()->getName());
	}
}
