
#pragma once

#include "CloudsVisualSystem.h"
#include "ofxGameCamera.h"
#include "CloudsClusterNode.h"
#include "CloudsQuestion.h"
#include "CloudsRun.h"

class CloudsFCPParser;
class CloudsVisualSystemClusterMap : public CloudsVisualSystem {
  public:
    
	void buildEntireCluster(CloudsFCPParser& parser);

	void setRun(CloudsRun& run);
	void setQuestions(vector<CloudsClip>& questions);
	CloudsQuestion* getSelectedQuestion();

	//will add the latest state of the run to the traversal
	void traverse();
	
	void traverseToClip(CloudsClip& clip);
	
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "_ClusterMap";
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

	void selfSetupTimeline();
	
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
	

    // if you use a custom camera to fly through the scene
	// you must implement this method for the transitions to work properly
	ofCamera& getCameraRef(){
//		return cam;
		return easeCamera;
	}

	void reloadShaders();
	
	//this is for the secondary display
	bool incrementalTraversalMode;

  protected:
    
    //  Your Stuff
    //
	ofEasyCam easeCamera;
	ofxUISuperCanvas* generatorGui;
	ofxUISuperCanvas* displayGui;
	
	ofxGameCamera cam;
	CloudsRun* run;
	
	ofxTLColorTrack* lineColor;
	ofxTLColorTrack* nodeColor;

	//TODO pick a better font renderer
	ofTrueTypeFont font;
	vector<CloudsQuestion> questions;
	CloudsQuestion* selectedQuestion;
	ofShader clusterShader;
	ofShader lineShader;
	ofShader traversalShader;
	
	bool firstClip;
	ofVec3f lastClipPosition;
	
	float currentVertIndex;
	
	ofImage sprite;
	
	ofVboMesh traversalMesh;
	ofVboMesh clusterMesh;
	ofVboMesh connectionMesh;
	
	vector<CloudsClusterNode> nodes;
	map<string,int> clipIdToNodeIndex;
	
	float maxTraverseAngle;
	
	float meshExpansion;
	float pointSize;
	
	float lineAlpha;
	float lineFocalDistance;
	float lineFocalRange;
	float lineDissolve;
	ofVec3f trailHead;
	
	float nodePopLength;
	float traversedNodeSize;
};
