
#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"

#include "ofxAnimationPrimitives.h"
#include "ofxTonic.h"
#include "CloudsAudioEvents.h"
#include "CloudsGlobal.h"
#include "TonicSample.h"

//using namespace Tonic;


class CloudsVisualSystemReplicator : public CloudsVisualSystem {
 public:
	
	struct Data {
		float x, y;
		float w, h;
		float rx, ry;
	};
	
	typedef ofxAnimationPrimitives::Replicator_<Data> Replicator;
	
	Replicator repl;
	float local_time;


	string getSystemName();
	
	void selfSetup();
	void selfSetupGuis();
	
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
    void selfSetDefaults();

	// Sound
    ofxUISuperCanvas* soundGui;
    
//    ofVboMesh vboRect;
//    ofShader instanceShader;
//    int matsLoc;
//    
//    ofTexture dataTex;
//    
    vector<TonicSample> tonicSamples;
    Tonic::ofxTonicSynth synth;
    Tonic::Generator buildSynth();
    Tonic::ControlParameter volumeControl;
    float gain;
	void audioRequested(ofAudioEventArgs& args);
};

