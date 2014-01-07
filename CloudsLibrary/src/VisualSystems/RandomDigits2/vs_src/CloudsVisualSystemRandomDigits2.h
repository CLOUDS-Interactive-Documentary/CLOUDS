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
#include "RandomNumber.h"

#include "CloudsGlobal.h"
#include "CloudsAudioEvents.h"
#include "ofxTonic.h"

using namespace Tonic;



//TODO: rename this to your own visual system
class CloudsVisualSystemRandomDigits2 : public CloudsVisualSystem {
  public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "RandomDigits2";
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
	
    // if you use a custom camera to fly through the scene
	// you must implement this method for the transitions to work properly
//	ofCamera& getCameraRef(){
//		return myCustomCamera;
//	}


protected:
    
    
    //  Your Stuff
    //
    void initRandomNumbers();
    ofFloatColor getRGBfromHSV(ofFloatColor& hsv);
    void addColorToGui(ofxUISuperCanvas* gui,string prefix,ofFloatColor& col, bool doAlpha = false, float min=0.0f, float max = 1.0f);
    
    ofxUISuperCanvas* customGui;
    map< ofFloatColor*, ofxUILabel*> labelColors;

    vector<RandomNumber> numbers;
    ofVbo vbo;
    ofImage numbersImg;
    int xCount, yCount, zCount;
    int xSpacing, ySpacing, zSpacing;
    float xNoise, yNoise, zNoise;
    float dark, light;

    // Sound
    ofxUISuperCanvas* soundGui;
    int nSamples = 4;
    string soundFiles[4] = {"EchoVortex.aif",
        "wormholeZoom.aif",
        "wormholeZoom2.aif",
        "slowgrains_short.aif"};
    bool playSample[4];
    ControlTrigger soundTriggers[4];
    ofxTonicSynth synth;
    Generator buildSynth();
	void audioRequested(ofAudioEventArgs& args);
};
