//
//  IntroSequence.cpp
//  ComputerTicker
//
//  Created by James George on 5/26/13.
//
//

#include "CloudsIntroSequence.h"
#include "ofxObjLoader.h"

CloudsIntroSequence::CloudsIntroSequence(){
		
}

CloudsIntroSequence::~CloudsIntroSequence(){
	
}

string CloudsIntroSequence::getSystemName(){
	return "IntroSeqence";
}

void CloudsIntroSequence::selfSetup(){
	camera.setup();
	camera.autosavePosition = true;
	camera.loadCameraPosition();

	setCurrentCamera(camera);
	
	ofxObjLoader::load(getDataPath() + "intro/OBJ/ParticleCube_loose.obj", tunnelMesh);
	ofQuaternion rotationFix;
//	rotationFix.makeRotate(90, 1, 0, 0);
	for(int i = 0; i < tunnelMesh.getVertices().size(); i++){
		tunnelMesh.getVertices()[i] = rotationFix * tunnelMesh.getVertices()[i];
	}
}

void CloudsIntroSequence::selfSetupGuis(){
	
	camGui->addSlider("Camera Speed", 0, 5, &camera.speed);
}

void CloudsIntroSequence::selfUpdate(){
	
}

void CloudsIntroSequence::selfDrawBackground(){
	
}

void CloudsIntroSequence::selfDrawDebug(){

}

void CloudsIntroSequence::selfDraw(){
	
	
	tunnelMesh.drawVertices();
	
}

void CloudsIntroSequence::selfExit(){
	
}
void CloudsIntroSequence::selfBegin(){
	
}

void CloudsIntroSequence::selfEnd(){
	
}

void CloudsIntroSequence::selfKeyPressed(ofKeyEventArgs & args){
	
}

void CloudsIntroSequence::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsIntroSequence::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfSetupSystemGui(){
	
}

void CloudsIntroSequence::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsIntroSequence::selfSetupRenderGui(){
	
}
void CloudsIntroSequence::guiRenderEvent(ofxUIEventArgs &e){
	
}
