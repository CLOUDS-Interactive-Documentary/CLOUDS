#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"
//#include "ParticleConnectionGenerator.h"
//#include "CloudsCaption.h"
#include "CloudsQuestion.h"
#include "GPUParticles/Controller.h"
#include "ofxGameCamera.h"
#include "ofxDelaunay.h"
#include "ofxFTGL.h"

struct TransitionInfo{
	ofVec3f inStartPos;
	ofVec3f outTargetPos;
	ofQuaternion inStartQuat;
	ofQuaternion outTargetQuat;
	string optionName;
	string transitionName;
};

class CloudsVisualSystemRGBD : public CloudsVisualSystem {
  public:

	string getSystemName();
    
	void selfSetDefaults();
    void selfSetup();
    void selfSetupGuis();
	void selfPresetLoaded( string presetName );

    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    void selfDraw();	
	void selfDrawOverlay();
    void selfExit();
    void selfBegin();
	void selfEnd();
    
	void speakerChanged();
	
	void addFakeQuestion(vector<string> testPngFilePaths);
	void addQuestion(CloudsClip& q,string topic, string question);
    void setSelectedQuestion();

    void clearQuestions();
    bool isQuestionSelectedAndClipDone();
	bool isQuestionSelected();
    CloudsQuestion* getSelectedQuestion();
    
//    ofxFTGLFont captionFont;
//    void rebuildCaptionFont();
//    int captionFontSize;
	
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

	float visualSystemFadeValue;
	
	ofCamera& getCameraRef(){
//		if(currentCamera != NULL)	return  *currentCamera;
		if(placingTransitionNodes){
			return transitionCam;
		}
		return cloudsCamera;
	}
	
	//TODO
	void startTransitionOut(RGBDTransitionType transitionType);
	void startTransitionIn(RGBDTransitionType transitionType);
	void updateTransition(float percentComplete);
	void transtionFinished();
	
	//DEPRECATED
	void transitionIn( RGBDTransitionType transitionType, float duration, float startTime=ofGetElapsedTimef() );
	void transitionOut( RGBDTransitionType transitionType, float duration, float startTime=ofGetElapsedTimef() );
	void transition( float duration=3, float startTime=ofGetElapsedTimef() );
	
	ofNode* transitionTarget;
	ofNode transitionInStart;
	ofNode transitionOutTarget;
	ofVec3f transitionEndPosition;
	ofQuaternion transitionEndRotation;
	
	void transitionIn( ofNode& targetNode, float duration, float startTime );
	void transitionOut( ofNode& startNode, float duration, float startTime );
	
	bool bTransitionIn, bTransitionOut;
	bool bLookThroughIn, bLookThroughOut;
	bool bSaveTransition;
	
	void lookThroughTransitionIn();
	void lookThroughTransitionOut();

	ofxGameCamera transitionCam;
	ofNode* transitionCamTargetNode;
	
	void printTransitionNodes();
	void setTransitionNodes( RGBDTransitionType transitionType );
	
	void playTestVideo();
	
	ofVec3f bottomRight;
	
	void loadTransitionOptions(string filename );
	void saveTransitionSettings(string filename = "Transitions");
	void setTransitionNodes( string transitionName );
	void updateTransition();
	void updateTransitionGui();
	
	void addTransitionGui(string guiName);
	void clearTransitionMap();
	
	void addTransionEditorsToGui();
	void setTransitionNodes( string type, string option );
	
	//TODO: delete this
	void tester(string s){	cout<<"tester: " + s<<endl;}

  protected:
	
	ofxUISuperCanvas *transitionEditorGui;
	map< string, TransitionInfo> transitionOptionMap;
	map<string, string> transitionTypes;
	
	map<string, map<string, TransitionInfo> > transitionMap;
	map<string, ofxUISuperCanvas*> transitionsGuis;
	bool bTransitionsAddedToGui;
	
	string currentTransitionType;
	
	ofxUISuperCanvas *globalMeshGui;
	bool drawRGBD;
	float edgeAttenuate;
	float edgeAttenuateExponent;
	float skinBrightness;
	
	//POINTS
	ofxUISuperCanvas *pointsGui;
	ofShader pointShader;
	ofVboMesh points;
	bool drawPoints;
	float pointAlpha;
	int numRandomPoints;
	ofRange pointSize;
	float pointHeadOverlap;
	float pointFlowPosition;
	float pointFlowSpeed;
	bool pointsFlowUp;
	bool refreshPointcloud;
	void generatePoints();
	
	//LINES
	ofxUISuperCanvas *linesGui;
	ofShader lineShader;
	ofVboMesh lines;
	bool drawLines;
	float lineAlpha;
	float lineThickness;
	float lineSpacing;
	float lineDensityVariance;
	float lineGranularity;
	float lineFlowPosition;
	float lineFlowSpeed;
	float lineHeadOverlap;
	bool linesFlowUp;
	bool refreshLines;
	void generateLines();
	
	//MESH
	ofxUISuperCanvas *meshGui;
	ofShader meshShader;
	ofVboMesh mesh;
	bool drawMesh;
	float meshAlpha;
	float xSimplify;
	float ySimplify;
	float randomVariance;
	float meshFaceMinRadius;
	float meshFaceFalloff;
	float meshRetractionFalloff;
	float meshForceGeoRectraction;
	
	bool refreshMesh;
	void generateMesh();
	
	void loadShader();

	ofxUISuperCanvas *cameraGui;
	ofxUISuperCanvas *particleGui;
	ofxUISuperCanvas *questionGui;

	//TODO: move to hud
	ofxFTGLSimpleLayout displayFont;

	GPUParticles::Controller particulateController;
	
	void updateQuestions();
	void drawQuestions();
	
	vector<CloudsQuestion*> questions;
	CloudsQuestion* caughtQuestion;
    CloudsQuestion* selectedQuestion;
	ofVec4f pointColor;
	
	bool placingTransitionNodes;
	bool drawTransitionNodes;

	bool drawParticulate;
	float attenuatedCameraDrift;
	
	//transition
	bool transitioning, transitioningIn, transitioningOut, bResetLookThoughs;
	float transitionStartTime, transitionEndTime, transitionStartVal, transitionTargetVal;
	string activeTransition;
	
	RGBDTransitionType transitionType;
	ofxEasingSine transitionEase;
	
	float transitionVal;
	
	ofVec3f questionXZ;
	float questionDriftRange;
	float questionYCenter;
	float questionYDriftRange;
	float questionYRange;
	float questionLifeSpan; //minutes
	
	ofFloatColor questionBaseHSB;
	ofFloatColor questionHoverHSB;
	
};