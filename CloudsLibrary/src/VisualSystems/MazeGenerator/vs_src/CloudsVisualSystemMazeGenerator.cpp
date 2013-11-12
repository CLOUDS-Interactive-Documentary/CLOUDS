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
    customGui = new ofxUISuperCanvas("MAZE BUILDER", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
	customGui->setName("MAZE BUILDER");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

    customGui->addSpacer();
    customGui->addSlider("CAM SPEED", -100, 300, &settings.cameraSpeed);
    customGui->addSlider("CAM HEIGHT", 0, 5000, &settings.cameraHeight);
    customGui->addSlider("CAM ANGLE", 0, 360, &settings.cameraAngle);
    customGui->addToggle("GROUND CAM", &settings.groundCam);
    customGui->addSlider("GCAM SPEED", 0, 1, &settings.groundCamSpeed);
    customGui->addSlider("GCAM LOOKAT", 0, 1, &settings.groundCamLookAt);
    
    customGui->addSpacer();
    customGui->addSlider("SHOW AHEAD", 10, 150, &settings.showAhead);
    customGui->addSlider("FOG DENSITY", 0, .1, &settings.fogDensity);
    customGui->addSlider("FOG START", 0, 10000, &settings.fogStart);
    customGui->addSlider("FOG END", 0, 10000, &settings.fogEnd);
    
    customGui->addSpacer();
    customGui->addSlider("NUM BALLS", 0, 150, &settings.numberOfBalls);
    customGui->addSlider("BALLS RADIUS", 0, 40, &settings.ballRadius);
    customGui->addSlider("BALLS SPEED", 0, 0.01, &settings.ballMaxSpeed);
    
    customGui->addSlider("WALL HEIGHT VARIANCE", 0, 1, &settings.heightRandom);
    

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
    customGui->addMinimalSlider("FH", 0.0, 255, &(settings.fogColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("FS", 0.0, 255, &(settings.fogColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("FB", 0.0, 255, &(settings.fogColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    customGui->addLabel("GROUND COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("GH", 0.0, 255, &(settings.groundColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("GS", 0.0, 255, &(settings.groundColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("GB", 0.0, 255, &(settings.groundColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    customGui->addLabel("WALL COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("WH", 0.0, 255, &(settings.wallColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("WS", 0.0, 255, &(settings.wallColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("WB", 0.0, 255, &(settings.wallColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    customGui->addLabel("BALL COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("SH", 0.0, 255, &(settings.ballColor.r), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("SS", 0.0, 255, &(settings.ballColor.g), length, dim)->setShowValue(true);
    customGui->addMinimalSlider("SB", 0.0, 255, &(settings.ballColor.b), length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        
    ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemMazeGenerator::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemMazeGenerator::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.getName() == "GROUND CAM") {
        if (!settings.groundCam)
        {
            mazeCam.setFlyOver(maze[0]->getWidth()/2);
        }        
    }
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
    maze[0] = new Maze(60, 8, 60, &settings);
    maze[0]->generate();
    
    mazeCam.setup(maze[0]->getWidth()/2, settings.cameraHeight, 100, &settings);

    light = new ofLight();
    light->setPointLight();
    light->setSpecularColor(ofColor(200));
    light->setDiffuseColor(ofColor(200));
    light->setAmbientColor(ofColor(50));
    
    camPath = maze[0]->createSimpleSpline(50, 50, 500);
    mazeCam.setPath(camPath);
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemMazeGenerator::selfPresetLoaded(string presetPath)
{
    if (!settings.groundCam)
    {
        mazeCam.setFlyOver(maze[0]->getWidth()/2);
    }
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

	
	//JG seems that these were smashing your system and set to random values in your presets,
	//and by calling your own camera it overode the bug
	xRot->setHome(0);
	yRot->setHome(0);
	zRot->setHome(0);

    mazeCam.update();
    
    maze[0]->update(&mazeCam);

    if (bLights) {
        light->setPosition(lightPos.x, lightPos.y, lightPos.z);
    }
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemMazeGenerator::selfDraw()
{
	
	glEnable(GL_DEPTH_TEST);
    // draw fog
    ofFloatColor fc = settings.getFogColor();
    GLfloat fogColor[4] = {fc.r, fc.g, fc.b, 1.0};
    glFogfv (GL_FOG_COLOR, fogColor);
	glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
	glFogi(GL_FOG_MODE, GL_LINEAR);
    glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_DENSITY, powf(settings.fogDensity, 2));
    glFogf(GL_FOG_START, settings.fogStart);
    glFogf(GL_FOG_END, settings.fogEnd);
    
	glEnable(GL_FOG);
    

//JG this is already happening
//    ofEnableLighting();
    if (bLights) {
        light->enable();
    }

//JG -- OCULUS BUG: don't start and stop your own camera, this is what getCameraRef() is
//	mazeCam.begin();
    maze[0]->draw(&mazeCam, lightPos);
//	mazeCam.end();
    
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
    
//    delete mazeCam;
    delete camPath;
	delete light;
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
	if(!bUseOculusRift){
		mazeCam.mouseMove(ofVec2f(data.x, data.y));
	}
}

void CloudsVisualSystemMazeGenerator::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMazeGenerator::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMazeGenerator::setLightOri(ofLight* light, ofVec3f rot)
{
    ofVec3f xax(1, 0, 0);
    ofVec3f yax(0, 1, 0);
    ofVec3f zax(0, 0, 1);
    ofQuaternion q;
    q.makeRotate(rot.x, xax, rot.y, yax, rot.z, zax);
    light->setOrientation(q);
}
