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

	reloadShaders();
}

void CloudsIntroSequence::reloadShaders(){
	tunnelShader.load(getDataPath() + "shaders/Intro/IntroTunnel");
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
	if(args.key == 'R'){
		reloadShaders();
	}
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

	
	gui->addSlider("Min Point Size", 0, 5, &pointSize.min);
	gui->addSlider("Min Point Size", 0, 5, &pointSize.max);
	
	gui->addSlider("Perlin Amplitude", 0, 100, &perlinAmplitude);
	gui->addSlider("Perlin Density", 0, 10, &perlinDensity);
	gui->addSlider("Perlin Speed", 0, 1, &perlinSpeed);
	
}

void CloudsIntroSequence::guiRenderEvent(ofxUIEventArgs &e){
	
}

