#include "testApp.h"
#include "CloudsGlobal.h"
#include "CloudsVisualSystem.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);

	systemManager.loadPresets();
    systemManager.loadCachedDataForSystems();

	currentSystem = NULL;
	currentSystemIndex = 0;

	lastSystemChangedTime = 0;

	presetIndices = systemManager.getFilteredPresetIndeces(true);
	for(int i = 0; i < presetIndices.size(); i++){
		cout << systemManager.getPresets()[ presetIndices[i] ].systemName << " " << systemManager.getPresets()[ presetIndices[i] ].presetName << endl;
	}


}

//--------------------------------------------------------------
void testApp::update(){
	if(ofGetElapsedTimef() - lastSystemChangedTime > 10){
		if(currentSystem != NULL){
			currentSystem->stopSystem();
			currentSystem->exit();
			CloudsVisualSystemManager::DeallocateSystems();
		}
		CloudsVisualSystemPreset& preset = systemManager.getPresets()[presetIndices[currentSystemIndex]];
		currentSystem = CloudsVisualSystemManager::InstantiateSystem(preset.systemName);
		if(currentSystem->getSystemName() == "Empty"){
			ofLogError() << "System " << preset.systemName << " isn't registered";
		}
		currentSystem->setup();
		currentSystem->loadPresetGUISFromName(preset.presetName);
		currentSystem->setDrawToScreen(false);
		currentSystem->playSystem();
		lastSystemChangedTime = ofGetElapsedTimef();
		currentSystemIndex = (currentSystemIndex + 1) % presetIndices.size();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if(currentSystem != NULL){
		currentSystem->selfPostDraw();
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}