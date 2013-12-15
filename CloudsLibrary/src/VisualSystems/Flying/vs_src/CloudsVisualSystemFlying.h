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
#include "ofxRules.h"
#include "ofxPostProcessing.h"

struct Plant
{
    unsigned meshIdx;
    ofVec3f pos;
};

class CloudsVisualSystemFlying : public CloudsVisualSystem
{
  public:
    static const unsigned NUM_RULES_FILES = 3;
    //static const unsigned NUM_RULES_FILES = 1;
    static const string RULES_FILES[NUM_RULES_FILES];
    static const float CAM_DAMPING;
    
    CloudsVisualSystemFlying();
    
	string getSystemName(){
		return "Flying";
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
    
    void selfPostDraw();
		
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

	//
	ofCamera& getCameraRef(){
		if(videoLoaded){
			return cloudsCamera;
		}
		return CloudsVisualSystem::getCameraRef();
	}

protected:
    //  Your Stuff
    //
    void generate();
    void onWindowResized(ofResizeEventArgs& args);
    
    ofxPostProcessing post;
    
    bool cameraControl;
    float fogStart, fogEnd;
    
    // cam
    float xRot, yRot, zSpeed;
    float camAvoidDist;
    
    // plants
    ofShader plantsShader;
    unsigned numPlantMeshes;
    float numNearbyPlants;
    vector<ofxRules> plantMeshes;
    list<Plant> plants;
    float growDist;
    bool drawPlantPosns;
    
    // floor
    ofVboMesh floor;
    float floorW, floorD, floorHalfW, floorHalfD;
    unsigned xResolution, zResolution;
    ofImage floorTex;
    ofShader floorShader;
    ofVec3f hsv0, hsv1;
    float noiseFreq, noiseAmp;
    float xStep, zStep;
    ofVec3f floorLookAt;
    
	ofxUISuperCanvas* customGui;
	bool customToggle;
	float customFloat1;
	float customFloat2;
	
	bool videoLoaded;
	ofImage someImage;
	ofShader pointcloudShader;
	ofVboMesh simplePointcloud;
};
