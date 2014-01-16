//
//  CloudsVisualSystemEmpty.cpp
//

#include "CloudsVisualSystemMazeGenerator2.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events

CloudsVisualSystemMazeGenerator2::CloudsVisualSystemMazeGenerator2()
{
}

void CloudsVisualSystemMazeGenerator2::selfSetupGui()
{
    ParamManager2* pm = &ParamManager2::getInstance();

    customGui = new ofxUISuperCanvas("MAZE BUILDER 2", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
	customGui->setName("MAZE BUILDER 2");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

    customGui->addSpacer();
    customGui->addLabel("CAMERA");
    customGui->addSlider("CAM SPEED", 0, 20, &pm->cameraSpeed);
    customGui->addSlider("CAM HEIGHT", 0, 1500, &pm->cameraHeight);
    customGui->addSlider("CAM ANGLE", 0, 360, &pm->cameraAngle);

    customGui->addSlider("SHOW AHEAD", 10, 150, &pm->showAhead);

    float length = (customGui->getGlobalCanvasWidth()-customGui->getWidgetSpacing()*5)/3.;
    float dim = customGui->getGlobalSliderHeight();

    customGui->addSpacer();
    customGui->addToggle("LIGHT DIRECTION ANGLE", &bLights);
    customGui->addSlider("LX", 0, 360, &lightAng.x);
    customGui->addSlider("LY", 0, 360, &lightAng.y);
    customGui->addSlider("LZ", 0, 360, &lightAng.z);
    
    customGui->addSpacer();
    customGui->addLabel("COLORS");
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
    
    customGui->addLabel("SIDE WALLS COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("SH", 0.0, 255, &(pm->sideWallsColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("SS", 0.0, 255, &(pm->sideWallsColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("SB", 0.0, 255, &(pm->sideWallsColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    customGui->addLabel("GENERATOR COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("NH", 0.0, 255, &(pm->generatorColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("NS", 0.0, 255, &(pm->generatorColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("NB", 0.0, 255, &(pm->generatorColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemMazeGenerator2::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemMazeGenerator2::selfGuiEvent(ofxUIEventArgs &e)
{

}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemMazeGenerator2::selfSetupSystemGui(){
	
}

void CloudsVisualSystemMazeGenerator2::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemMazeGenerator2::selfSetupRenderGui(){
    
}

void CloudsVisualSystemMazeGenerator2::guiRenderEvent(ofxUIEventArgs &e)
{
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemMazeGenerator2::selfSetup()
{
//    maze = new Maze(30, 4, 30);
    maze = new Maze2(60, 8, 40);
    maze->generate();
    
    mazeCam = new MazeCamera2(maze->getWidth()/2,
                              ParamManager2::getInstance().cameraHeight, 100);
    
    light = new ofLight();
    light->setDirectional();
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemMazeGenerator2::selfPresetLoaded(string presetPath)
{

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemMazeGenerator2::selfBegin()
{
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemMazeGenerator2::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemMazeGenerator2::selfUpdate()
{
    mazeCam->update();
    
    maze->update(mazeCam);

    if (bLights) {
        setLightOri(light, lightAng);

    }
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemMazeGenerator2::selfDraw()
{
    if (bLights) {
        light->enable();
    }
    
    mazeCam->begin();

    maze->draw(mazeCam);
    
    mazeCam->end();
    
    if (bLights) {
        light->disable();
    }
}

// draw any debug stuff here
void CloudsVisualSystemMazeGenerator2::selfDrawDebug()
{
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemMazeGenerator2::selfDrawBackground()
{
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemMazeGenerator2::selfEnd()
{
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemMazeGenerator2::selfExit()
{
    delete maze;
    delete light;
    delete mazeCam;
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemMazeGenerator2::selfKeyPressed(ofKeyEventArgs & args)
{
	
}
void CloudsVisualSystemMazeGenerator2::selfKeyReleased(ofKeyEventArgs & args)
{
	
}

void CloudsVisualSystemMazeGenerator2::selfMouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemMazeGenerator2::selfMouseMoved(ofMouseEventArgs& data)
{
}

void CloudsVisualSystemMazeGenerator2::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMazeGenerator2::selfMouseReleased(ofMouseEventArgs& data){
	
}

void
CloudsVisualSystemMazeGenerator2::setLightOri(ofLight* light, ofVec3f rot)
{
    ofVec3f xax(1, 0, 0);
    ofVec3f yax(0, 1, 0);
    ofVec3f zax(0, 0, 1);
    ofQuaternion q;
    q.makeRotate(rot.x, xax, rot.y, yax, rot.z, zax);
    light->setOrientation(q);
}
