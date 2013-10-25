//
//  CloudsVisualSystemExampleOpenSteer.cpp
//

#include "CloudsVisualSystemExampleOpenSteer.h"


//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemExampleOpenSteer::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
	customGui->addButton("Custom Button", false);
	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemExampleOpenSteer::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemExampleOpenSteer::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemExampleOpenSteer::selfSetupSystemGui(){
	
}

void CloudsVisualSystemExampleOpenSteer::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemExampleOpenSteer::selfSetupRenderGui(){

}

void CloudsVisualSystemExampleOpenSteer::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemExampleOpenSteer::selfSetup(){
    // add the simulations to the vector and initialize the first one
	simulations.push_back(&flocking);
	simulations.push_back(&pathFollowing);
	simulations.push_back(&obstacleAvoidance);
    simulations.push_back(&pursuitAndEvade);
		
//	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemExampleOpenSteer::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemExampleOpenSteer::selfBegin(){
    ofEnableAlphaBlending();
	
	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
	glEnable(GL_DEPTH_TEST);
	
	// the sample simulations are quite small in scale, so
	// we wanna make sure our camera is close enough
	cam.setDistance(50);
    
    currentSimulation = NULL;
	setSimulation(0);
}

void CloudsVisualSystemExampleOpenSteer::setSimulation( int simulationIndex ){
	// just make sure we are inside out vector
	this->simulationIndex = ofClamp(simulationIndex, 0, simulations.size() - 1);
	
	// exit the current simulation
	if(currentSimulation) currentSimulation->exit();
	
	// define the current simulation
	currentSimulation = simulations[simulationIndex];
	
	// initialize the current simulation
	currentSimulation->setup();
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemExampleOpenSteer::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemExampleOpenSteer::selfUpdate(){
    currentSimulation->update();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemExampleOpenSteer::selfDraw(){
    cam.begin();
    currentSimulation->draw();
	cam.end();
	
}

// draw any debug stuff here
void CloudsVisualSystemExampleOpenSteer::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemExampleOpenSteer::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemExampleOpenSteer::selfEnd(){

}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemExampleOpenSteer::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemExampleOpenSteer::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemExampleOpenSteer::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemExampleOpenSteer::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemExampleOpenSteer::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemExampleOpenSteer::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemExampleOpenSteer::selfMouseReleased(ofMouseEventArgs& data){
	
}