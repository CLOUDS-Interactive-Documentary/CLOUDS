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
#include "CloudsGlobal.h"
#include "ofxSimpleSpline.h"
#include "CloudsPathCamera.h"
#include "ofxObjLoader.h"
#include "glm.h"

#include "CloudsGlobal.h"
#include "CloudsAudioEvents.h"
#include "ofxTonic.h"

using namespace Tonic;

//TODO: rename this to your own visual system
class CloudsVisualSystemWormHole : public CloudsVisualSystem {
  public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "WormHole";
	}

	//These methods let us add custom GUI parameters and respond to their events
    void selfSetupGui();
    void selfSetupCameraGui();
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

	//
	ofCamera& getCameraRef(){
		return CloudsVisualSystem::getCameraRef();
	}
	
	void selfSetDefaults();

protected:
	
	void loadShaders();
	void loadShader( string shaderName );
	void loadMesh(string name);
	
	string vec3ToString( ofVec3f v, int precision );
	ofVec3f normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2);
	void smoothMesh( ofMesh& facetedMesh, ofMesh& targetMesh, int precision = 3);
	void facetMesh( ofMesh& smoothedMesh, ofMesh& targetMesh );
	
	
	ofxUISuperCanvas* customGui;
	ofxUISuperCanvas* cameraGui;
	ofxUISuperCanvas* meshGui;
	ofxUISuperCanvas* shaderGui;
	ofxUISuperCanvas* fogGui;
	ofxUISuperCanvas* wormholeLightGui;
	ofxUISuperCanvas* displacementGui;
	
	ofImage colorSampleImage;
	bool bCullBackface;
	bool bDoShader;
	ofShader normalShader;
	ofShader facingRatio;
	ofShader WormholeShader;
	
	map<string, ofShader*> shaderMap;
	map<string, ofBlendMode> blendModes;
	
	ofShader* currentShader;
	
	ofNode meshNode;
	bool bDrawPoints;
	ofVboMesh mesh;
	string modelPath;
	string currentMeshName;
	
	vector<string> meshNames;
	vector<string> cameraPathNames;
		
	float fogDist, fogExpo;
	int fogHue, fogSaturation, fogBrightness;
	ofColor fogColor;
	
	ofVec3f lightPos;
	float lightLinearAttenuation, lightQuadraticAttenuation, lightConstantAttenuation, lightPathOffset;
	int lightHue, lightSaturation, lightBrightness;
	
	ofColor lightColor;
	float lightFallOff;
	
	string cameraPathPath;
	bool bUseCameraPath;
	float cameraPathPosition;
	CloudsPathCamera pathCamera;
	
	float sampleTime, lastTime, speed;
	float shininess,facingRatioExpo;
	ofBlendMode currentBlendMode;
	bool bDepthTest;
	
	int c1Hue, c1Sat, c1Bri;
	int c2Hue, c2Sat, c2Bri;
	ofColor c1, c2;
		
	bool bUseNoiseDisplacement;
	float noiseDisplacement, noiseSpeed, noiseTime, noiseScale;
	ofVec3f noiseOffset, noiseDir;
    
    float nearClipPlane;

    // Sound
    ofxUISuperCanvas* soundGui;
    int nSamples = 4;
    string soundFiles[4] = {"EchoVortex.aif",
        "wormholeZoom.aif",
        "wormholeZoom2.aif",
        "slowgrains.aif"};
    bool playSample[4];
    ControlTrigger soundTriggers[4];
    ofxTonicSynth synth;
    Generator buildSynth();
	void audioRequested(ofAudioEventArgs& args);
};
