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
#include "ofxMPMFluid.h"

#include "ofxTonic.h"
#include "CloudsAudioEvents.h"
#include "CloudsGlobal.h"

//using namespace Tonic;

//TODO: rename this to your own visual system
class CloudsVisualSystemExampleMPMFluid : public CloudsVisualSystem {
  public:
    
    CloudsVisualSystemExampleMPMFluid() {}
    
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "ExampleMPMFluid";
	}

	void selfSetDefaults();

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
	
    void selfInteractionMoved(CloudsInteractionEventArgs& args);
	void selfInteractionStarted(CloudsInteractionEventArgs& args);
	void selfInteractionDragged(CloudsInteractionEventArgs& args);
	void selfInteractionEnded(CloudsInteractionEventArgs& args);

    // if you use a custom camera to fly through the scene
	// you must implement this method for the transitions to work properly
	ofCamera& getCameraRef(){
		return cam;
	}

protected:
    
    ofxMPMFluid fluid;
    ofxMPMObstacle* obstacle;
    
    map<int, int> currentPlayers;
    
    float scaleFactor;
    
    // preset stuff
    ofxUISuperCanvas* customGui;
    ofxUIRadio *interModeRadio;
    float nParticles;
    float density;
    float stiffness;
    float bulkViscosity;
    float elasticity;
    float viscosity;
    float gravity;
    float obstacleSize;
    ofFloatColor pColor;
    float lineWidth;
    float mouseForce;

	// Sound
    ofxUISuperCanvas* soundGui;
    float volume[4];
    float fMainGain;
    Tonic::ControlParameter mainGain;
    Tonic::ControlParameter volumeControl[4];
    Tonic::ControlTrigger soundTriggers[3];
    Tonic::ControlParameter mouseX, mouseY, mouseSpeed, totalSpeed;
    int prevMouseX, prevMouseY;
    Tonic::ofxTonicSynth synth;
    Tonic::Generator buildSynth();
	void audioRequested(ofAudioEventArgs& args);
};
