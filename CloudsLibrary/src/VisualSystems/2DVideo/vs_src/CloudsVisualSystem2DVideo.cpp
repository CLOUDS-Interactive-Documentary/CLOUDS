//
//  CloudsVisualSystem2DVideo.cpp
//

#include "CloudsVisualSystem2DVideo.h"

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfSetupGui()
{
    /*
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addSpacer();
    customGui->addToggle("RESTART", &bRestart);
    customGui->addToggle("2D", &bIs2D);
    customGui->addSlider("RADIUS", 1.0, 50.0, &radius);
    
    customGui->addSpacer();
    customGui->addSlider("FG HUE 1", 0.0f, 0.99999f, &fgParams1[0]);
    customGui->addSlider("FG SAT 1", 0.0f, 1.0f, &fgParams1[1]);
    customGui->addSlider("FG BRI 1", 0.0f, 1.0f, &fgParams1[2]);
    
    customGui->addSpacer();
    customGui->addSlider("FG HUE 2", 0.0f, 0.99999f, &fgParams2[0]);
    customGui->addSlider("FG SAT 2", 0.0f, 1.0f, &fgParams2[1]);
    customGui->addSlider("FG BRI 2", 0.0f, 1.0f, &fgParams2[2]);

    customGui->addSpacer();
    customGui->addSlider("FADE", 0.0, 0.5, &fade);  // Fucks up if I go up to 1.0, don't know why, don't care anymore...
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystem2DVideo::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
     */
}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfGuiEvent(ofxUIEventArgs &e)
{

}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystem2DVideo::selfSetupSystemGui(){
	
}

void CloudsVisualSystem2DVideo::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystem2DVideo::selfSetupRenderGui(){

}

void CloudsVisualSystem2DVideo::guiRenderEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfSetup()
{

}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::restart()
{

}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfPresetLoaded(string presetPath)
{

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystem2DVideo::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystem2DVideo::selfSceneTransformation(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfUpdate()
{

}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfDraw()
{

}

// draw any debug stuff here
void CloudsVisualSystem2DVideo::selfDrawDebug(){

}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::selfDrawBackground()
{
    
}

//--------------------------------------------------------------
void CloudsVisualSystem2DVideo::render()
{

}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystem2DVideo::selfEnd(){

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystem2DVideo::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystem2DVideo::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystem2DVideo::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystem2DVideo::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystem2DVideo::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystem2DVideo::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystem2DVideo::selfMouseReleased(ofMouseEventArgs& data){
	
}