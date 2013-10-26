//
//  CloudsVisualSystemEmpty.cpp
//

#include "CloudsVisualSystemMazeGenerator.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events

CloudsVisualSystemMazeGenerator::CloudsVisualSystemMazeGenerator()
{
}

void CloudsVisualSystemMazeGenerator::selfSetupGui()
{
    ParamManager* pm = &ParamManager::getInstance();

    customGui = new ofxUISuperCanvas("MAZE BUILDER", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
	customGui->setName("MAZE BUILDER");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

    customGui->addSpacer();
    customGui->addLabel("CAMERA");
    customGui->addSlider("CAM SPEED", 0, 10, &pm->cameraSpeed);

    float length = (customGui->getGlobalCanvasWidth()-customGui->getWidgetSpacing()*5)/3.;
    float dim = customGui->getGlobalSliderHeight();

    customGui->addSpacer();
    customGui->addLabel("LOOK");
    customGui->addLabel("GROUND COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("GH", 0.0, 255, &(pm->groundColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("GS", 0.0, 255, &(pm->groundColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("GB", 0.0, 255, &(pm->groundColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    customGui->addLabel("WALL COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("WH", 0.0, 255, &(pm->wallColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("WS", 0.0, 255, &(pm->wallColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("WB", 0.0, 255, &(pm->wallColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

#if 0
    customGui->addLabel("PHYSICS");
    customGui->addSlider("PARTICLES", 0, 50000, &nParticles);
    customGui->addSlider("DENSITY", 0, 30, &density);
    customGui->addSlider("STIFFNESS", 0, 2, &stiffness);
    customGui->addSlider("VISCOSITY", 0, 4, &viscosity);
    customGui->addSlider("BULK VISCOSITY", 0, 10, &bulkViscosity);
    customGui->addSlider("ELASTICITY", 0, 4, &elasticity);
    customGui->addSlider("GRAVITY", 0, 0.2f, &gravity);
    
    
    float length = (customGui->getGlobalCanvasWidth()-customGui->getWidgetSpacing()*5)/3.;
    float dim = customGui->getGlobalSliderHeight();

    /* Particle color */
    //customGui->addSpacer();
    customGui->addSpacer();
    customGui->addLabel("LOOK");
    customGui->addLabel("PARTICLE COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("H", 0.0, 255, &pColor.r, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("S", 0.0, 255, &pColor.g, length, dim)->setShowValue(true);
    customGui->addMinimalSlider("B", 0.0, 255, &pColor.b, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    customGui->addSlider("PARTICLE LINE", 0, 5, &lineWidth);

    customGui->addSpacer();
    customGui->addLabel("INTERACTIVITY");
    vector<string> modes;
    modes.push_back("Mouse as force");
    modes.push_back("Mouse as obstacle");
    interModeRadio = customGui->addRadio("INTERACTIVE MODE", modes);
    customGui->addSlider("OBSTACLE SIZE", 0, 30, &obstacleSize);
    customGui->addSlider("MOUSE FORCE", 0, 20, &mouseForce);
#endif
    
    ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemMazeGenerator::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemMazeGenerator::selfGuiEvent(ofxUIEventArgs &e)
{

}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemMazeGenerator::selfSetupSystemGui(){
	
}

void CloudsVisualSystemMazeGenerator::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemMazeGenerator::selfSetupRenderGui(){
    
}

void CloudsVisualSystemMazeGenerator::guiRenderEvent(ofxUIEventArgs &e)
{
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemMazeGenerator::selfSetup()
{
    maze = new Maze(30, 4, 30);
    maze->generate();
    
    mazeCam = new MazeCamera(maze->getWidth()/2, 180, 0);
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemMazeGenerator::selfPresetLoaded(string presetPath)
{

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemMazeGenerator::selfBegin()
{
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemMazeGenerator::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemMazeGenerator::selfUpdate()
{
    mazeCam->update();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemMazeGenerator::selfDraw()
{
    mazeCam->begin();
    
    maze->draw(mazeCam);
    
    mazeCam->end();
}

// draw any debug stuff here
void CloudsVisualSystemMazeGenerator::selfDrawDebug()
{
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemMazeGenerator::selfDrawBackground()
{
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemMazeGenerator::selfEnd()
{
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemMazeGenerator::selfExit()
{
    delete maze;
    delete mazeCam;
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemMazeGenerator::selfKeyPressed(ofKeyEventArgs & args)
{
	
}
void CloudsVisualSystemMazeGenerator::selfKeyReleased(ofKeyEventArgs & args)
{
	
}

void CloudsVisualSystemMazeGenerator::selfMouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemMazeGenerator::selfMouseMoved(ofMouseEventArgs& data)
{
}

void CloudsVisualSystemMazeGenerator::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMazeGenerator::selfMouseReleased(ofMouseEventArgs& data){
	
}
