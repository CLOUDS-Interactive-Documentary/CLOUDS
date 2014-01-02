
#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"
#include "ofxOcean.h"
#include "ofxOceanRenderer.h"
#include "ofxOceanCamera.h"
#include "CloudsGlobal.h"
#include "CloudsAudioEvents.h"
#include "ofxTonic.h"

using namespace Tonic;


class CloudsVisualSystemOcean : public CloudsVisualSystem {
  public:
	CloudsVisualSystemOcean();

	string getSystemName();

	void selfSetup();
	void selfSetupGuis();
	void selfPresetLoaded(string presetPath);
	
	void selfUpdate();
	void selfDrawBackground();
	void selfDrawDebug();
	void selfSceneTransformation();
	void selfDraw();
	void selfExit();
	void selfBegin();
	void selfEnd();

	void selfKeyPressed(ofKeyEventArgs & args);
	void selfKeyReleased(ofKeyEventArgs & args);

	void selfMouseDragged(ofMouseEventArgs& data);
	void selfMouseMoved(ofMouseEventArgs& data);
	void selfMousePressed(ofMouseEventArgs& data);
	void selfMouseReleased(ofMouseEventArgs& data);

	void selfSetupGui();
	void selfGuiEvent(ofxUIEventArgs &e);

	void selfSetupSystemGui();
	void guiSystemEvent(ofxUIEventArgs &e);

	void selfSetupRenderGui();
	void guiRenderEvent(ofxUIEventArgs &e);
	
	ofCamera& getCameraRef(){
		return useOceanCam ? oceanCamera : CloudsVisualSystem::getCameraRef();
	}
	
  protected:
	
	ofxOcean ocean;
	ofxOceanRenderer renderer;
	ofxOceanCamera oceanCamera;
	ofShader oceanShader;
	
	ofBlendMode blendMode;
	
	bool needsRegenerate;
	bool useOceanCam;
	
	bool drawPoints;
	float pointAlpha;
	bool drawWireframe;
	float wireframeAlpha;
	bool drawOcean;
	float oceanAlpha;
	
	float pointSize;
	
	bool shouldRegenerateOcean;
	void generateOcean();
	
	ofxUISuperCanvas* oceanGui;
	ofxUISuperCanvas* oceanCameraGui;
	
	//Generator variables
	float windSpeed;
    float oceanTileSizeX;
    float oceanTileSizeY;
	
	float fogDensity;

	float waveScale;
	float choppyScale;
	float waveSpeed;

	bool depthTesting;
	float cameraSpeed;
	float currentLookRot;
	float currentSteerRot;
	float maxLookUpRot;
	float maxLookDownRot;
	
	// Sound
    ofxUISuperCanvas* soundGui;
    string soundFiles[2] = {
        "Vocal_harmonic_high_shorter.aif",
        "vocal_harmony_bass.aif"};
    bool playSample[2];
    ControlTrigger soundTriggers[2];
    ofxTonicSynth synth;
    Generator buildSynth();
	void audioRequested(ofAudioEventArgs& args);
};