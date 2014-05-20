
#pragma once

#include "CloudsVisualSystem.h"

#include "CloudsClusterNode.h"
//#include "CloudsQuestion.h"
#include "CloudsRun.h"
#include "ofxFTGLFont.h"
#include "ofxNearestNeighbour.h"

typedef struct{
	ofIndexType startIndex;
	ofIndexType endIndex;
} TraversalSegment;

typedef struct{
    int numSubTopics;
    vector<ofVec3f> subTopicPositions;
	string keyword;
	ofVec3f position;
	ofVec2f screenPosition;
	int numClips;
	float normalizedTopicScale;
} TopicPoint;

class CloudsFCPParser;
class CloudsVisualSystemClusterMap : public CloudsVisualSystem {
  public:
    
	CloudsVisualSystemClusterMap();
	
	void buildEntireCluster(CloudsFCPParser& parser);
	void setRun(CloudsRun& run);
	void setAct(CloudsAct* newAct);
//	void setQuestions(vector<CloudsClip*>& questions);
	void allocateFlickerTexture();
//	CloudsQuestion* getSelectedQuestion();

    void setCurrentTopic(string topic);
    
	//will add the latest state of the run to the traversal
	void traverse();

	void traverseToClip(CloudsClip* clip);

	void clearTraversal();
    
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

	void selfDrawOverlay();
	
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
		if(lockCameraAxis){
			return axisCamera;
		}
		return easyCamera;
	}

	void reloadShaders();
	bool autoTraversePoints;
	bool finishedTraversing;
    void parseAssociations();
    
  protected:

	ofxUISuperCanvas* nodesGui;
	ofxUISuperCanvas* linesGui;
	ofxUISuperCanvas* optionPathsGui;
	ofxUISuperCanvas* traversalGui;
	ofxUISuperCanvas* followCamGui;
	ofxUISuperCanvas* typeGui;
	
	ofEasyCam easyCamera;
	ofCamera axisCamera;
	CloudsAct* act;
	
	ofVec2f flickerCoord;
	int flickerWidth;

	CloudsFCPParser* parser;
	CloudsRun* run;
	void resetGeometry();
	
	bool traverseNextFrame;
	
	bool drawNodes;
	bool drawLines;
	bool drawTraversal;
	bool drawOptionPaths;
	
	vector<CloudsClusterNode> nodes;
	map<string,int> clipIdToNodeIndex;

	ofVboMesh traversalMesh;
	ofVboMesh optionsMeshPrev,optionsMeshNext;
	ofVboMesh nodeMesh;
	ofVboMesh networkMesh;
	
	ofShader nodesShader;
	ofShader networkShader;
	ofShader traversalShader;
	ofShader optionsShader;
	
	vector<TraversalSegment> traversalPath;
	ofVec3f currentTraversalPosition;
	ofVec3f currentTraversalDirection;
	ofIndexType lastTraverseStartedIndex;
	
	bool firstClip;
	ofVec3f currentNodePosition;
	ofIndexType currentNodeIndex;
	int currentTraversalIndex;
	
	ofVec3f cameraStartPosition;
	float percentToDest;
	
	ofImage sprite;
	
	float meshExpansion;
	ofRange pointSize;

    ofVec3f networkCentroid;
	ofVec3f trailHead;

	float nodePopLength;
	float lineDensity;
	float lineFocalDistance;
	float lineFocalRange;
	float networkLineWidth;
	
	bool drawTraversalPoints;
	bool lockCameraAxis;
	float traverseCamFOV;
	float traversCameraDistance;
	float traversedNodeSize;
	float traverseStepSize;
	float traverseAngleDampen;
	float traverseHomingMinDistance;
	float traverseMinSolvedDistance;
	float traverseLineWidth;

	bool bConstrainTraversal;
    float maxTraverseAngle;
    float maxTraverseDistance;
    float minTraverseDistance;
    int minTraverseNextOptions;
    ofxNearestNeighbour3D kdtree;
    
	//colors~
	bool matchLineColor;
	ofFloatColor lineNodeColorHSV;
	ofFloatColor lineNodeColorRGB;
	ofFloatColor lineEdgeColorHSV;
	ofFloatColor lineEdgeColorRGB;
	float lineColorMixExponent;
	bool drawLineFlickerDebug;
	float lineFlickerIntensity;
	float lineFlickerFrequency;
	float lineFlickerDampening;
	bool bSmoothLines;
	
	bool matchTraversalColor;
	ofFloatColor traverseHeadColorHSV;
	ofFloatColor traverseHeadColorRGB;
	ofFloatColor traverseTailColorHSV;
	ofFloatColor traverseTailColorRGB;
	float traverseFalloff;
	float traverseRevealVerts;
	
	ofFloatImage flickerNoise;
	ofFloatPixels flickerNoiseTarget;
	ofFloatColor optionColorHSV;
	ofFloatColor optionColorRGB;
	
	//animate params
	float traverseAnimationDuration;
	float optionsAnimationDuration;
	float optionLineWidth;
	
	bool drawHomingDistanceDebug;
	
	float traverseStartTime;
	float percentTraversed;
	float percentOptionsRevealed;
	
	//type vars
	void populateTopicPoints();
	vector<TopicPoint> topicPoints;
	vector<ofxFTGLFont> topicFont;
	bool drawType;
	int baseFontSize;
	ofIntRange clipsShowTopic;
	ofRange clipCountRange;
	ofRange typeDistanceRange;
	ofIntRange typeSizeRange;
	ofIntRange currentTypeSizeRange;
	
    void populateAssociations();
    map<string,string> associations;
    bool drawAssociation;
    int associationFontSize;
    int currentAssociationFont;
    string currentTopic;
    ofVec2f trailheadScreenPos;
    ofxFTGLFont associationFont;
    
	ofVec3f randomDirection();
    int numTraversed;
    
	//vector<CloudsQuestion> questions;
	//CloudsQuestion* selectedQuestion;

};
