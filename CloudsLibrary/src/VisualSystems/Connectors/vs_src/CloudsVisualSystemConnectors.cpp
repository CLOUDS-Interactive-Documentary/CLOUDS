//
//  CloudsVisualSystemConnectors.cpp
//

#include "CloudsVisualSystemConnectors.h"
#include "CloudsRGBDVideoPlayer.h"

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemConnectors::selfSetupGui(){

	connectorGui = new ofxUISuperCanvas("CONNECTORS", gui);
	connectorGui->copyCanvasStyle(gui);
	connectorGui->copyCanvasProperties(gui);
	connectorGui->setName("Custom");
	connectorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	connectorGui->addSlider("Num Particles", 50, 64*64, &generator.numParticles);
	connectorGui->addToggle("Draw Connections", &generator.drawConnections);
	connectorGui->addSlider("Min Connection Distance", 1, 100, &generator.minDistance);
	connectorGui->addSlider("Boundary Size", 100, 1000, &generator.boundarySize);
	
	connectorGui->addSlider("Max Connections", 1, 10, &generator.maxConnections);
	
    connectorGui->addSlider("Particle speed", 0, 10, &generator.connectorsSpeed);
    
	guis.push_back(connectorGui);
	guimap[connectorGui->getName()] = connectorGui;
}

void CloudsVisualSystemConnectors::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}
void CloudsVisualSystemConnectors::selfSetDefaults(){
    primaryCursorMode = CURSOR_MODE_CAMERA;
    secondaryCursorMode = CURSOR_MODE_INACTIVE;
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemConnectors::selfSetupSystemGui(){
	
}

void CloudsVisualSystemConnectors::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemConnectors::selfSetupRenderGui(){

}

void CloudsVisualSystemConnectors::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemConnectors::selfSetup(){
	generator.setup();

//	generator.setBounds(ofVec3f(-500,-500,-500), ofVec3f(500,500,500), 5);
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemConnectors::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemConnectors::selfBegin(){
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemConnectors::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemConnectors::selfUpdate(){
	generator.update();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemConnectors::selfDraw(){

	ofPushStyle();
	ofSetLineWidth(1.f);
	ofDisableDepthTest();
	ofDisableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	ofNoFill();
	ofSetColor(20);
	ofBox(0,0,0, generator.boundarySize*2);

	generator.draw();
	ofPopStyle();
}

// draw any debug stuff here
void CloudsVisualSystemConnectors::selfDrawDebug(){
	generator.drawBins();
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemConnectors::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemConnectors::selfEnd(){
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemConnectors::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemConnectors::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemConnectors::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemConnectors::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemConnectors::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemConnectors::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemConnectors::selfMouseReleased(ofMouseEventArgs& data){
	
}

