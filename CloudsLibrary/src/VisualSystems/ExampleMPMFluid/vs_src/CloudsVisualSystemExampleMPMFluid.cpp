//
//  CloudsVisualSystemEmpty.cpp
//

#include "CloudsVisualSystemExampleMPMFluid.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events

void CloudsVisualSystemExampleMPMFluid::selfSetupGui(){
    customGui = new ofxUISuperCanvas("EXAMPLE MPM FLUID", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
	customGui->setName("EXAMPLE MPM FLUID");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

#if 0
    customGui->addToggle("GAVITY CONTROL MODE", &bGravityMod);
    
    customGui->addToggle("CIRCLES", &bCircles);
    customGui->addToggle("RECTANGLES", &bRects);
    
    customGui->addSlider("TRIGGER FORCE", 0, 30, &triggerForce);
    customGui->addSlider("MAX CIRCLES", 10, 1000, &maxCircles);
    customGui->addSlider("MAX RECTANGLES", 10, 1000, &maxRects);
    
    customGui->addSlider("CIRCLE MAX MEAN", 5, 50, &circleSizeMean);
    customGui->addSlider("CIRCLE SIZE DEVIATION", 5, 50, &circleSizeDev);
    customGui->addSlider("RECT MAX MEAN", 5, 50, &rectSizeMean);
    customGui->addSlider("RECT SIZE DEVIATION", 5, 50, &rectSizeDev);
    
    customGui->addToggle("FILL", &bFill);
    customGui->addToggle("STATIC PLATFORMS", &bStaticPlatforms);
    customGui->addToggle("RANDOM PLATFORMS", &bRandomPlatforms);
    
    float length = (customGui->getGlobalCanvasWidth()-customGui->getWidgetSpacing()*5)/3.;
    float dim = customGui->getGlobalSliderHeight();
    
    /* Circle color */
    customGui->addLabel("CIRCLE COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("H1", 0.0, 255, &circleHSB.r, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("S1", 0.0, 255, &circleHSB.g, length, dim)->setShowValue(true);
    customGui->addMinimalSlider("B1", 0.0, 255, &circleHSB.b, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    /* Circle Line color */
    customGui->addLabel("CIRCLE LINE COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("H2", 0.0, 255, &circleLineHSB.r, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("S2", 0.0, 255, &circleLineHSB.g, length, dim)->setShowValue(true);
    customGui->addMinimalSlider("B2", 0.0, 255, &circleLineHSB.b, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    /* Rectangle color */
    customGui->addLabel("RECT COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("H3", 0.0, 255, &rectHSB.r, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("S3", 0.0, 255, &rectHSB.g, length, dim)->setShowValue(true);
    customGui->addMinimalSlider("B3", 0.0, 255, &rectHSB.b, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
#endif
    ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemExampleMPMFluid::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemExampleMPMFluid::selfGuiEvent(ofxUIEventArgs &e)
{

}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemExampleMPMFluid::selfSetupSystemGui(){
	
}

void CloudsVisualSystemExampleMPMFluid::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemExampleMPMFluid::selfSetupRenderGui(){
    
}

void CloudsVisualSystemExampleMPMFluid::guiRenderEvent(ofxUIEventArgs &e)
{
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemExampleMPMFluid::selfSetup()
{

}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemExampleMPMFluid::selfPresetLoaded(string presetPath)
{

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemExampleMPMFluid::selfBegin()
{

}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemExampleMPMFluid::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemExampleMPMFluid::selfUpdate()
{
    

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemExampleMPMFluid::selfDraw()
{
    
}

// draw any debug stuff here
void CloudsVisualSystemExampleMPMFluid::selfDrawDebug()
{
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemExampleMPMFluid::selfDrawBackground()
{
    ofSetColor(255, 0, 0);
    ofFill();
    
    ofEllipse(ofRandom(ofGetWidth()), ofRandom(ofGetHeight()), 30, 30);
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemExampleMPMFluid::selfEnd()
{
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemExampleMPMFluid::selfExit()
{
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemExampleMPMFluid::selfKeyPressed(ofKeyEventArgs & args)
{
	
}
void CloudsVisualSystemExampleMPMFluid::selfKeyReleased(ofKeyEventArgs & args)
{
	
}

void CloudsVisualSystemExampleMPMFluid::selfMouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemExampleMPMFluid::selfMouseMoved(ofMouseEventArgs& data)
{
}

void CloudsVisualSystemExampleMPMFluid::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemExampleMPMFluid::selfMouseReleased(ofMouseEventArgs& data){
	
}
