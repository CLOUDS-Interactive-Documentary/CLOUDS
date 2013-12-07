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

#include "ofMain.h"
#include "CloudsVisualSystem.h"
#include "ofxBox2d.h"

#include "ofxTonic.h"

using namespace Tonic;

#define WAVE_GEN_NUM    150
class DummyApp;

//TODO: rename this to your own visual system
class CloudsVisualSystemExampleBox2D : public CloudsVisualSystem {
  public:
    
    CloudsVisualSystemExampleBox2D() {}
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "ExampleBox2D";
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
	ofCamera& getCameraRef(){
		return cam;
	}

protected:
    
    void createRandomObjects();
    void addRandomCircle();
    void addRandomRect();
    void addCircle(ofVec2f pos, ofVec2f vel, float rad);
    void addRect(ofVec2f pos, ofVec2f vel, ofVec2f size);
    void addStaticPlatforms();
    void removeStaticPlatforms();
    void addRandomPlatform();
    void removeRandomPlatform();
    void handleObjectLimit();
    void reinitBounds();
    float getGaussian();
    
    // triggered on collisions
    void contactStart(ofxBox2dContactArgs &e);
    
    ofxBox2d box2d;
    
    ofVec2f prevScreenSize;
    
    ofVec2f prevMouse;
    
    vector <ofxBox2dCircle> circles;
    vector <ofxBox2dRect> rects;
    vector <ofxBox2dPolygon> platforms;
    vector <ofxBox2dPolygon> randomPlatforms;
    
    // preset stuff
    ofxUISuperCanvas* customGui;

    bool bGravityMod;
    bool bCircles;
    bool bRects;
    
    ofVec2f gravityForce;
    ofVec2f gravityLine;
    int minObjectCount;
    float maxCircles;
    float maxRects;
    
    
    bool bFill;
    bool bRandomPlatforms;
    bool bStaticPlatforms;
    float triggerForce;
    
    int randomPlatformCounter;
    
    ofFloatColor circleHSB;
    ofFloatColor circleLineHSB;
    ofFloatColor rectHSB;
    
    float circleSizeDev;
    float circleSizeMean;
    float rectSizeDev;
    float rectSizeMean;
    
    ofxTonicSynth synth;
    ControlTrigger collisionTrigger[WAVE_GEN_NUM];
    ControlRandom noteControl[WAVE_GEN_NUM];
    float noteMax;
    float noteMin;
    int triggerIndex;
    Generator output;
//    Generator collisionGen[WAVE_GEN_NUM];
    DummyApp* dummyApp;
};

class DummyApp : public ofBaseApp
{
public:
    ofxTonicSynth *synth;
    DummyApp(ofxTonicSynth *s);
    
    void audioRequested (float * output, int bufferSize, int nChannels);
};
