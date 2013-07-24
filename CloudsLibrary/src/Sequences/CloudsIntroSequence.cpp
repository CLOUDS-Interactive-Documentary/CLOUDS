//
//  IntroSequence.cpp
//  ComputerTicker
//
//  Created by James George on 5/26/13.
//
//

#include "CloudsIntroSequence.h"
#include "ofxObjLoader.h"
#include "CloudsGlobal.h"

CloudsIntroSequence::CloudsIntroSequence(){
		
}

CloudsIntroSequence::~CloudsIntroSequence(){
	
}

string CloudsIntroSequence::getSystemName(){
	return "IntroSeqence";
}

void CloudsIntroSequence::selfSetup(){

	perlinOffset = 0;
	
	camera.setup();
	camera.autosavePosition = true;
	camera.loadCameraPosition();

	setCurrentCamera(camera);
	
	ofxObjLoader::load(getDataPath() + "intro/OBJ/ParticleCube_supertight.obj", tunnelMeshTight);
	ofxObjLoader::load(getDataPath() + "intro/OBJ/ParticleCube_loose.obj", tunnelMeshLoose);
	
//	ofxObjLoader::load(getDataPath() + "intro/OBJ/CLOUDS_type_thin_02.obj",thinTypeMesh);
	
	ofxObjLoader::load(getDataPath() + "intro/OBJ/CLOUDS_type_thick.obj",thickTypeMesh);
//	ofxObjLoader::load(getDataPath() + "intro/OBJ/CLOUDS_type_thin_02.obj",thinTypeMesh);
	thinTypeMesh.load(getDataPath() + "intro/OBJ/CLOUDS_type_thin_02.ply");
	thinTypeMesh.clearColors();
	
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
	sysGui->addButton("RESET GAME CAMERA", false);
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
//		cloudsTypeMesh.init(getDataPath() + "/font/materiapro_light.ttf", currentFontSize, currentFontExtrusion, "CLOUDS");
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
	
	ofRotate(180, 0, 1, 0);
	ofScale(fontScale, fontScale, fontScale);
//	ofSetColor(255);
//	for(int i = 0; i < cloudsTypeMesh.letters.size(); i++){
//		cloudsTypeMesh.letters[i].front.drawFaces();
//	}
	
	thinTypeMesh.draw();

	//DRAW MESH
//	thickTypeMesh.drawWireframe();
//	ofEnableAlphaBlending();
//	ofSetColor(255, 100);
//	thickTypeMesh.draw();

	ofPopMatrix();
	
	ofPushStyle();
	
	tunnelShader.begin();
	tunnelShader.setUniform1f("minPointSize", pointSize.min);
	tunnelShader.setUniform1f("maxPointSize", pointSize.max);
	tunnelShader.setUniform1f("minDistance", distanceRange.min);
	tunnelShader.setUniform1f("maxDistance", distanceRange.max);

	tunnelShader.setUniform1f("noiseAmplitude", perlinAmplitude);
	tunnelShader.setUniform1f("noiseDensity", perlinDensity);
	perlinOffset += perlinSpeed;
	tunnelShader.setUniform1f("noisePosition", perlinOffset);
	
//	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	
	ofSetColor(255);
	tunnelMeshTight.drawVertices();
	
	ofSetColor(255*wireframeAlpha);
	tunnelMeshLoose.drawWireframe();
	
	tunnelShader.end();
	ofPopStyle();
	
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
	
	rdrGui->addSlider("Min Point Size", 0, 7, &pointSize.min);
	rdrGui->addSlider("Max Point Size", 0, 7, &pointSize.max);
	rdrGui->addSlider("Min Distance", 0, 5000, &distanceRange.min);
	rdrGui->addSlider("Max Distance", 0, 10000, &distanceRange.max);

	rdrGui->addSlider("Chroma Max Distort", 0, 10, &maxChromaDistort);
	
	rdrGui->addSlider("Perlin Amplitude", 0, 10, &perlinAmplitude);
	rdrGui->addSlider("Perlin Density", 0, 50, &perlinDensity);
	rdrGui->addSlider("Perlin Speed", 0, .3, &perlinSpeed);
	
//	rdrGui->addSlider("Font Size", 1, 10, &fontSize);
//	rdrGui->addSlider("Font Extrusion", 0, 10, &fontExtrusion);
	rdrGui->addSlider("Font Scale", .1, 10, &fontScale);
	
	rdrGui->addSlider("Wireframe Alpha", 0, 1.0, &wireframeAlpha);
}

void CloudsIntroSequence::guiRenderEvent(ofxUIEventArgs &e){
	
}

