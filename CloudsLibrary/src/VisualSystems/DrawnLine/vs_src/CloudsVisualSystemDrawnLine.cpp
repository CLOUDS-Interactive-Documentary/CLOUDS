//
//  CloudsVisualSystemDrawnLine.cpp
//

#include "CloudsVisualSystemDrawnLine.h"
//#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemDrawnLine::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
	customGui->addButton("Custom Button", false);
	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemDrawnLine::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemDrawnLine::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemDrawnLine::selfSetupSystemGui(){
	
}

void CloudsVisualSystemDrawnLine::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemDrawnLine::selfSetupRenderGui(){

}

void CloudsVisualSystemDrawnLine::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemDrawnLine::selfSetup(){

	//make sure to include getVisualSystemDataPath() when accessing data
//	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemDrawnLine::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemDrawnLine::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemDrawnLine::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemDrawnLine::selfUpdate(){

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemDrawnLine::selfDraw(){
		
}

// draw any debug stuff here
void CloudsVisualSystemDrawnLine::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemDrawnLine::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemDrawnLine::selfEnd(){
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemDrawnLine::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemDrawnLine::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemDrawnLine::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemDrawnLine::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemDrawnLine::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemDrawnLine::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemDrawnLine::selfMouseReleased(ofMouseEventArgs& data){
	
}