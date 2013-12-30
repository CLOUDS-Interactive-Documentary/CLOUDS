//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
// Welcome to the Astrolabe CloudsVisualSystem
//
//
//

#pragma once

#include "CloudsVisualSystem.h"
#include "CloudsGlobal.h"
#include "Astrolabe.h"

//TODO: rename this to your own visual system
class CloudsVisualSystemAstrolabe : public CloudsVisualSystem {
  public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "Astrolabe";
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
	
	void selfSetDefaults();
	

    // if you use a custom camera to fly through the scene
	// you must implement this method for the transitions to work properly
//	ofCamera& getCameraRef(){
//		return myCustomCamera;
//	}

	//
//	ofCamera& getCameraRef(){
//		if(videoLoaded){
//			return cloudsCamera;
//		}
//		return CloudsVisualSystem::getCameraRef();
//	}

protected:
	
	void loadShaders();
	void unloadShaders();
	
	void setAstrolabesColors();
	void resetRingRotations();
    
    void setupRings(int count = 15,
					float innerRad = 5,
					float width = 40,
					float thickness = 10,
					float xRot = 45,
					float yRot = 40,
					float zRot = 60,
					float spacing = 2,
					float xTickSpeed = 100,
					float lowRadians = PI,
					float hiRadians = TWO_PI,
					float initialOffsetScale = .25);
	
	void clearAstrolabes();
	
	
	//RINGS
	ofxUISuperCanvas* ringsGui;
	ofxUISuperCanvas* shaderGui;
	ofxUISuperCanvas* ticksGui;
	
	int c0_hue,c1_hue,c2_hue;
	int c0_brightness,c1_brightness,c2_brightness;
	int c0_saturation,c1_saturation,c2_saturation;
	int c0_alpha,c1_alpha,c2_alpha;
	ofColor c0, c1, c2;
	
	bool bSetupRings;
	int ringsCount;
	float ringsInnerRad;
	float ringsWidth;
	float ringsThickness;
	int ringsXRot;
	int ringsYRot;
	int ringsZRot;
	float ringsSpacing;
	float ringsMinTickSpeed;
	float ringsLowRadians;
	float ringsHiRadians;
	float ringsDelayOffsetScale;
	float ringsNoiseyness;
	float innerSpeed, outerSpeed;
	bool bResetTicks;
	
	ofBlendMode currentBlendMode;
	bool bDepthTest;
	
	int xTickSpeed, yTickSpeed, zTickSpeed;
	int xTickDelay, yTickDelay, zTickDelay;
	bool bAutoReverseX, bAutoReverseY, bAutoReverseZ;
	float ringsFalloff;
	
	
	ofxUISuperCanvas* customGui;
	
	ofxUISuperCanvas* clockGui;
	
	//
	vector<Astrolabe*> astrolabes;
	vector<ofImage> palettes;
	vector<string> paletteNames;
//	map<string, ofImage> palettes;
	
	ofShader normalShader;
	ofShader facingRatio;
	
	ofxEasingQuint 	easingquint;
	ofxEasingQuad easequad;
	ofxEasingCubic easecubic;
	
	ofxEasingBounce easebounce;
	ofxEasingSine easesine;
	ofxEasingLinear easelinear;
	
	map<string, ofxEasing*> easeMap;
	
	int skipAhead;
};
