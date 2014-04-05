//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
// Welcome to the EMPTY CloudsVisualSystem
//
//
//

#pragma once

#include "CloudsVisualSystem.h"
#include "Walker.h"



//TODO: rename this to your own visual system
class CloudsVisualSystemOpenP5Spaghetti: public CloudsVisualSystem {
  public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    CloudsVisualSystemOpenP5Spaghetti(){
		justStartingOut = true;
		amplitude = 100;
		period = 120;
		numPreloads = 1000;
		saturation = 100;
		brightness = 200;
		saturation1 = 0;
		brightness1 = 0;
		saturation2 = 0;
		brightness2 = 0;
		hue1 = 50;
		hue2 = 10;
	};
    
    string getSystemName(){
		return "OpenP5Spaghetti";
	}

	//These methods let us add custom GUI parameters and respond to their events
    void selfSetDefaults();
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
    
    void initialize();
	
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
    
    //void setColorBetween(ofColor color1, ofColor color2);
    
    void regenerate(); 
   
    bool justStartingOut;
    
	vector<Walker> walkers;
    
    float NWalkers;
    bool smooth;
//    static bool drawTriangles;
    bool shouldRegenerate;
    bool rainbow;
    bool oscillate;
    bool dichromatic;
        
//  ofImage colorMap;
	ofColor color1;
    ofColor color2;
    ofColor randomColor;
    ofColor newColor;
    int numPreloads;
    float saturation;
    float brightness;
    float saturation1;
    float brightness1;
    float saturation2;
    float brightness2;
    float hue1;
    float hue2;
    
    //spin speed
    float currSpin;
    float spinSpeed;

    // if you use a custom camera to fly through the scene
	// you must implement this method for the transitions to work properly
//	ofCamera& getCameraRef(){
//		return myCustomCamera;
//	}


protected:
    
    //  Your Stuff
    //

    ofxUISuperCanvas* customGui;
	ofxUISuperCanvas* customCameraGui;
     
    //oscillator
    
    float osc;
    float amplitude;
    float period;
    float frameCount;
    
    //float osc = amplitude * cos(TWO_PI * frameCount / period);


};
