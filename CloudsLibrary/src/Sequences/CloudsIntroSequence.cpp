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
	
	ofxObjLoader::load(getDataPath() + "intro/OBJ/ParticleCube_tight.obj", tunnelMesh);
//	cloudsType.loadFont(getDataPath() + "/font/materiapro_light.ttf", 20);
	
//	int fontsize = 20;
//    int depth = 50;
	currentFontExtrusion = -1;
	currentFontSize = -1;
	
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
	
	if(currentFontSize != fontSize ||
	   currentFontExtrusion != fontExtrusion)
	{
		currentFontSize = fontSize;
		currentFontExtrusion = fontExtrusion;
		cloudsTypeMesh.init(getDataPath() + "/font/materiapro_light.ttf", currentFontSize, currentFontExtrusion, "CLOUDS");
	}

}

void CloudsIntroSequence::selfDrawBackground(){
	fullscreenFbo.begin();
	
	camera.begin();
	
	glEnable(GL_DEPTH_TEST);
	
	ofClear(0);
	
	ofEnableSmoothing();
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
	glEnable(GL_POINT_SMOOTH);
	ofPushMatrix();
	
	ofRotate(180, 0, 0, 1);
	
	ofTranslate(-cloudsTypeMesh.getWidth()/2.0*fontScale, cloudsTypeMesh.getHeight()/2.0*fontScale);
	ofScale(fontScale, fontScale, fontScale);
	
	ofSetColor(255);
	for(int i = 0; i < cloudsTypeMesh.letters.size(); i++){
		cloudsTypeMesh.letters[i].front.drawFaces();
	}
	
	ofPopMatrix();
	
	tunnelShader.begin();
	tunnelShader.setUniform1f("minPointSize", pointSize.min);
	tunnelShader.setUniform1f("maxPointSize", pointSize.max);
	tunnelShader.setUniform1f("minDistance", distanceRange.min);
	tunnelShader.setUniform1f("maxDistance", distanceRange.max);

	tunnelShader.setUniform1f("noiseAmplitude", perlinAmplitude);
	tunnelShader.setUniform1f("noiseDensity", perlinDensity);
	tunnelShader.setUniform1f("noisePosition", ofGetElapsedTimef());
	
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

void CloudsIntroSequence::drawCloudsType(){
	
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
	sysGui->addButton("RESET GAME CAMERA", false);
}

void CloudsIntroSequence::guiSystemEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "RELOAD SHADER" && ((ofxUIButton*)e.widget)->getValue()){
		cout << "Loaded shader" << endl;
		reloadShaders();
	}
	else if(e.widget->getName() == "RESET GAME CAMERA" && ((ofxUIButton*)e.widget)->getValue()){
		camera.setPosition(0, 0, 0);
		camera.setOrientation(ofQuaternion());
		camera.rotate(180, ofVec3f(0,1,0));
		camera.setAnglesFromOrientation();

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
	
	gui->addSlider("Font Size", 1, 10, &fontSize);
	gui->addSlider("Font Extrusion", 0, 10, &fontExtrusion);
	gui->addSlider("Font Scale", .1, 10, &fontScale);
}

void CloudsIntroSequence::guiRenderEvent(ofxUIEventArgs &e){
	
}

