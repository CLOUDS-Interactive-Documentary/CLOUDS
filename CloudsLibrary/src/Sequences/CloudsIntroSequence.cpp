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
	
	ofxObjLoader::load(getDataPath() + "intro/OBJ/ParticleCube_supertight.obj", tunnelMesh);

	
	reloadShaders();
}

void CloudsIntroSequence::reloadShaders(){
	tunnelShader.load(getDataPath() + "shaders/Intro/IntroTunnel");
	chroma.load("",getDataPath() + "shaders/BarrelChromaAb.fs");
}

void CloudsIntroSequence::selfSetupGuis(){
	
	camGui->addSlider("Camera Speed", 0, 5, &camera.speed);
}

void CloudsIntroSequence::selfUpdate(){
	
	if(!fullscreenFbo.isAllocated() ||
	   fullscreenFbo.getWidth() != ofGetWidth() ||
	   fullscreenFbo.getHeight() != ofGetHeight())
	{
		fullscreenFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
	}
	
	camera.applyRotation = camera.applyTranslation = !cursorIsOverGUI();
}

void CloudsIntroSequence::selfDrawBackground(){
	fullscreenFbo.begin();
	
	camera.begin();
	
	ofClear(0);
	
	ofEnableSmoothing();
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
	glEnable(GL_POINT_SMOOTH);
	
	tunnelShader.begin();
	tunnelShader.setUniform1f("minPointSize", pointSize.min);
	tunnelShader.setUniform1f("maxPointSize", pointSize.max);
	tunnelShader.setUniform1f("minDistance", distanceRange.min);
	tunnelShader.setUniform1f("maxDistance", distanceRange.max);

	tunnelShader.setUniform1f("noiseAmplitude", perlinAmplitude);
	tunnelShader.setUniform1f("noiseDensity", perlinDensity);

	tunnelMesh.drawVertices();
	
	tunnelShader.end();
	
	fullscreenFbo.end();
	
	camera.end();
	
	chroma.begin();
	chroma.setUniform2f("resolution", ofGetWidth(),ofGetHeight());
	chroma.setUniform1f("max_distort", maxChromaDistort);
	fullscreenFbo.draw(0,ofGetHeight(),ofGetWidth(),-ofGetHeight());
	chroma.end();

}

void CloudsIntroSequence::selfDrawDebug(){

}

void CloudsIntroSequence::selfDraw(){
	

	
	
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
	sysGui->addButton("RELOAD SHADER", false);
}

void CloudsIntroSequence::guiSystemEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "RELOAD SHADER" && ((ofxUIButton*)e.widget)->getValue()){
		cout << "Loaded shader" << endl;
		reloadShaders();
	}
}

void CloudsIntroSequence::selfSetupRenderGui(){
	
	gui->addSlider("Min Point Size", 0, 7, &pointSize.min);
	gui->addSlider("Max Point Size", 0, 7, &pointSize.max);
	gui->addSlider("Min Distance", 0, 5000, &distanceRange.min);
	gui->addSlider("Max Distance", 0, 10000, &distanceRange.max);

	gui->addSlider("Chroma Max Distort", 0, 10, &maxChromaDistort);
	
	gui->addSlider("Perlin Amplitude", 0, 100, &perlinAmplitude);
	gui->addSlider("Perlin Density", 0, 10, &perlinDensity);
	gui->addSlider("Perlin Speed", 0, 1, &perlinSpeed);
	
}

void CloudsIntroSequence::guiRenderEvent(ofxUIEventArgs &e){
	
}

