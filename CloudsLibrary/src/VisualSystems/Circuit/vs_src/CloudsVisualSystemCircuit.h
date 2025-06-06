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
#include "ofxCv.h"

typedef struct {
    int lineIndex;
    float vertIndex;
	int meshIndex;
    float speed;
    ofFloatColor color;
    ofVec3f pos;
} Blip;


//TODO: point shader
//	-- suze attenuator
//	-- color attenuate

class CloudsVisualSystemCircuit : public CloudsVisualSystem {
  public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "Circuit";
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
	
	void selfSetupCameraGui();
	
	ofCamera& getCameraRef();
	#ifdef HAS_GAMECAM
	ofxGameCamera cam;
	#endif
	
protected:
    
    //  Your Stuff
    //
	
	ofxUISuperCanvas* blipGui;
	bool bDrawBlips;
	ofRange pointSizeRange;
	ofRange pointDistanceRange;
	float blipScaleExponent;
	float blipSpeed;

	ofxUISuperCanvas* lineGui;
	bool bDrawLine;
	
	ofxUISuperCanvas* meshGui;
	float lastElevation;
	float currentElevation;
	bool bDrawMesh;
	
	float nearClippingPlane;
	
	ofShader blipShader;
	ofShader lineShader;
	ofShader fastBlipShader;
	ofShader meshShader;
	
	void reloadShaders();
	void generateCircuit();
	
	
	
	ofxCv::ContourFinder contourFinder;
    vector<Blip> blips;
	vector<ofPolyline> contourLines;
	vector<ofVec3f> centers;
    vector<ofColor> colors;
	
    vector<ofIndexType> elevated;
    

    ofImage heightMap;
    ofVboMesh heightMesh;
	ofVboMesh lineMesh;
	ofVboMesh blipMesh;

};
