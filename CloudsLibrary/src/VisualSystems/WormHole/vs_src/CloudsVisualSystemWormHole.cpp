//
//  CloudsVisualSystemWormHole.cpp
//

#include "CloudsVisualSystemWormHole.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemWormHole::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);	
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemWormHole::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemWormHole::selfGuiEvent(ofxUIEventArgs &e){
	string name = e.getName();
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemWormHole::selfSetupSystemGui(){
	
}

void CloudsVisualSystemWormHole::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemWormHole::selfSetupRenderGui(){

}

void CloudsVisualSystemWormHole::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemWormHole::selfSetup(){
	
//	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}

void CloudsVisualSystemWormHole::selfBegin(){
	
}


//normal update call
void CloudsVisualSystemWormHole::selfUpdate(){

}

void CloudsVisualSystemWormHole::selfDraw(){

}

void CloudsVisualSystemWormHole::selfPresetLoaded(string presetPath){
	
}
void CloudsVisualSystemWormHole::selfSceneTransformation(){
	
}

// draw any debug stuff here
void CloudsVisualSystemWormHole::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemWormHole::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemWormHole::selfEnd()
{

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemWormHole::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemWormHole::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemWormHole::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemWormHole::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemWormHole::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemWormHole::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemWormHole::selfMouseReleased(ofMouseEventArgs& data){
	
}