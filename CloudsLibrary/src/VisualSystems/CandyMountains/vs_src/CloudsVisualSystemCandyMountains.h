//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
// Welcome to the Candy Mountains CloudsVisualSystem
//
//
//

#pragma once

#include "CloudsVisualSystem.h"

//addons
#include "ofxMeshUtils.h"

//managers
#include "audioAnalytics.h"
#include "colorSchemeDesigner.h"
#include "terrainManager.h"
#include "cameraManager.h"
#include "lightManager.h"
#include "postProcManager.h"

//utilities
#include "decayer.h"

//TODO: rename this to your own visual system
class CloudsVisualSystemCandyMountains : public CloudsVisualSystem {
public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "CandyMountains";
	}
    
	//These methods let us add custom GUI parameters and respond to their events
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
	//Use system gui for global or logical settings, for exmpl
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
	//use render gui for display settings, like changing colors
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
	//This is called whenever a new preset is loaded, before selfSetup()
	//use it to ensure all your simple variables are initialized to an
	//acceptable default state
	void selfSetDefaults();
	
	// selfSetup is called when the visual system is first instantiated
	// This will be called during a "loading" screen, so any big images or
	// geometry should be loaded here
    void selfSetup();
    
	// selfBegin is called when the system is ready to be shown
	// this is a good time to prepare for transitions
	// but try to keep it light weight as to not cause stuttering
    void selfBegin();
    
	// selfPresetLoaded is called whenever a new preset is triggered
	// it'll be called right before selfBegin() and you may wish to
	// refresh anything that a preset may offset, such as stored colors or particles
	void selfPresetLoaded(string presetPath);
    
	//do things like ofRotate/ofTranslate here
	//any type of transformation that doesn't have to do with the camera
    void selfSceneTransformation();
	
	//normal update call
	void selfUpdate();
    
	// selfDraw draws in 3D using the default ofEasyCamera
	// you can change the camera by returning getCameraRef()
    void selfDraw();
    
    // draw any debug stuff here
	void selfDrawDebug();
    
	// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
	void selfDrawBackground();
    
	// this is called when your system is no longer drawing.
	// Right after this selfUpdate() and selfDraw() won't be called any more
	void selfEnd();
    
	// this is called when you should clear all the memory and delet anything you made in setup
    void selfExit();
    
	//events are called when the system is active
	//Feel free to make things interactive for you, and for the user!
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
	
    
protected:
    //  Your Stuff
    //
//    audioAnalytics aa;
    ofxAudioUnitFilePlayer player;
    ofxAudioUnitTap tap;
    ofxAudioUnitOutput output;
    colorSchemeDesigner cs;
    terrainManager tm;
    cameraManager cm;
    lightManager lm;
    postProcManager pm;
    
    int state;
    
    bool useLights;
    bool updateCam;
	
    //vs sys standards
	
	ofxUISuperCanvas* customGui;
	bool customToggle;
	float customFloat1;
	float customFloat2;
	
};



/*



//MY LCASS


#pragma once

#include "ofMain.h"

#include "ofxUI.h"
#include "ofxMeshUtils.h"


#include "audioAnalytics.h"
#include "colorSchemeDesigner.h"
#include "terrainManager.h"
#include "cameraManager.h"
#include "lightManager.h"
#include "postProcManager.h"

#include "decayer.h"

class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void setupGUI();
    void guiEvent(ofxUIEventArgs &e);

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
	
	audioAnalytics aa;
    colorSchemeDesigner cs;
    terrainManager tm;
    cameraManager cm;
    lightManager lm;
    postProcManager pm;
    
    int state;

    ofxUICanvas *gui;
    
    bool useLights;
    bool updateCam;
    
};

*/