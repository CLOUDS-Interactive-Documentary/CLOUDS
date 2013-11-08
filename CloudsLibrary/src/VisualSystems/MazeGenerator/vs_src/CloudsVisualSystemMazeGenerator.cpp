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
    customGui->addSlider("CAM SPEED", -100, 300, &pm->cameraSpeed);
    customGui->addSlider("CAM HEIGHT", 0, 5000, &pm->cameraHeight);
    customGui->addSlider("CAM ANGLE", 0, 360, &pm->cameraAngle);
    customGui->addToggle("GROUND CAM", &pm->groundCam);
    customGui->addSlider("GCAM SPEED", 0, 1, &pm->groundCamSpeed);
    customGui->addSlider("GCAM LOOKAT", 0, 1, &pm->groundCamLookAt);
    
    customGui->addSpacer();
    customGui->addSlider("SHOW AHEAD", 10, 150, &pm->showAhead);
    customGui->addSlider("FOG DENSITY", 0, .1, &pm->fogDensity);
    customGui->addSlider("FOG START", 0, 10000, &pm->fogStart);
    customGui->addSlider("FOG END", 0, 10000, &pm->fogEnd);
    
    customGui->addSpacer();
    customGui->addSlider("NUM BALLS", 0, 150, &pm->numberOfBalls);
    customGui->addSlider("BALLS RADIUS", 0, 40, &pm->ballRadius);
    customGui->addSlider("BALLS SPEED", 0, 0.01, &pm->ballMaxSpeed);
    
    customGui->addSlider("WALL HEIGHT VARIANCE", 0, 1, &pm->heightRandom);
    

    float length = (customGui->getGlobalCanvasWidth()-customGui->getWidgetSpacing()*5)/3.;
    float dim = customGui->getGlobalSliderHeight();

    customGui->addSpacer();
    customGui->addToggle("LIGHT", &bLights);
    customGui->addSlider("LX", 0, 10000, &lightPos.x);
    customGui->addSlider("LY", 0, 1000, &lightPos.y);
    customGui->addSlider("LZ", 0, 100000, &lightPos.z);
    
    customGui->addSpacer();
    customGui->addLabel("COLORS");

    customGui->addLabel("FOG COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("FH", 0.0, 255, &(pm->fogColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("FS", 0.0, 255, &(pm->fogColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("FB", 0.0, 255, &(pm->fogColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

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
    
    customGui->addLabel("BALL COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("SH", 0.0, 255, &(pm->ballColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("SS", 0.0, 255, &(pm->ballColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("SB", 0.0, 255, &(pm->ballColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        
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
//    maze = new Maze(30, 4, 30);
    maze[0] = new Maze(60, 8, 60);
    maze[0]->generate();
    
    mazeCam = new MazeCamera(maze[0]->getWidth()/2, ParamManager::getInstance().cameraHeight, 100);

    light = new ofLight();
    light->setPointLight();
    light->setSpecularColor(ofColor(200));
    light->setDiffuseColor(ofColor(200));
    light->setAmbientColor(ofColor(50));
    
    camPath = maze[0]->createSimpleSpline(50, 50, 500);
    mazeCam->setPath(camPath);
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
    
    maze[0]->update(mazeCam);

    if (bLights) {
        light->setPosition(lightPos.x, lightPos.y, lightPos.z);
    }
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemMazeGenerator::selfDraw()
{
    // draw fog
    ofFloatColor fc = ParamManager::getInstance().getFogColor();
    GLfloat fogColor[4] = {fc.r, fc.g, fc.b, 1.0};
    glFogfv (GL_FOG_COLOR, fogColor);
	glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
	glFogi(GL_FOG_MODE, GL_LINEAR);
    glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_DENSITY, powf(ParamManager::getInstance().fogDensity, 2));
    glFogf(GL_FOG_START, ParamManager::getInstance().fogStart);
    glFogf(GL_FOG_END, ParamManager::getInstance().fogEnd);
    
	glEnable(GL_FOG);
    

    ofEnableLighting();
    if (bLights) {
        light->enable();
    }
    
    mazeCam->begin();

    maze[0]->draw(mazeCam, lightPos);
    
    mazeCam->end();
    
    if (bLights) {
        light->disable();
    }
    
    // disable fog
    glDisable(GL_FOG);
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
    delete maze[0];
    
    delete mazeCam;
    
    delete camPath;
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
    mazeCam->mouseMove(ofVec2f(data.x, data.y));
}

void CloudsVisualSystemMazeGenerator::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMazeGenerator::selfMouseReleased(ofMouseEventArgs& data){
	
}

void
CloudsVisualSystemMazeGenerator::setLightOri(ofLight* light, ofVec3f rot)
{
    ofVec3f xax(1, 0, 0);
    ofVec3f yax(0, 1, 0);
    ofVec3f zax(0, 0, 1);
    ofQuaternion q;
    q.makeRotate(rot.x, xax, rot.y, yax, rot.z, zax);
    light->setOrientation(q);
}
