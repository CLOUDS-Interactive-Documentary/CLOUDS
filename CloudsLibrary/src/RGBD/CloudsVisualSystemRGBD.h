#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"
#include "CloudsPortal.h"
//#include "ParticleConnectionGenerator.h"
//#include "CloudsCaption.h"
#include "CloudsQuestion.h"
#include "GPUParticles/Controller.h"
#include "ofxGameCamera.h"
#include "ofxFTGL.h"

struct TransitionInfo{
	ofVec3f inStartPos;
	ofQuaternion inQuat;
	
	ofVec3f outRightPos;
	ofQuaternion outRightQuat;
	
	ofVec3f outLeftPos;
	ofQuaternion outLeftQuat;
	
	string optionName;
	string transitionName;
};

class CloudsVisualSystemRGBD : public CloudsVisualSystem {
  public:
	
	enum OutOption{
		OutLeft,
		OutRight
	};

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
	
	//TODO REMOVE
	void addFakeQuestion(vector<string> testPngFilePaths){}
	void addQuestion(CloudsClip& q,string topic, string question);
    void setSelectedQuestion();

    void clearQuestions();
    bool isQuestionSelectedAndClipDone();
	bool isQuestionSelected();
    CloudsPortal* getSelectedQuestion();
    
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
		if(placingTransitionNodes){
			return transitionCam;
		}
		return cloudsCamera;
	}
	
	void startTransitionOut(RGBDTransitionType transitionType);
	void startTransitionIn(RGBDTransitionType transitionType);
	void updateTransition(float percentComplete);
	void transtionFinished();
	
	ofNode* transitionTarget;
	ofNode transitionInStart;
	ofNode transitionOutLeft;
	ofNode transitionOutRight;
	ofVec3f transitionEndPosition;
	ofQuaternion transitionEndRotation;
	
	bool bTransitionIn, bTransitionOut;
	bool bLookThroughIn, bLookThroughOutLeft, bLookThroughOutRight;
	bool bSaveTransition;
	
	void lookThroughTransitionIn();
	void lookThroughTransitionOutLeft();
	void lookThroughTransitionOutRight();

	ofxGameCamera transitionCam;
	ofNode* transitionCamTargetNode;
	
	void setTransitionNodes( RGBDTransitionType transitionType );
	void playTestVideo();
	
	
	void loadTransitionOptions(string filename );
	void saveTransitionSettings(string filename = "Transitions");
	void setTransitionNodes( string transitionName );
	void updateTransition();
	void updateTransitionGui();
	
	void addTransitionGui(string guiName);
	void clearTransitionMap();
	
	void addTransionEditorsToGui();
	void setTransitionNodes( string type, string option );
	void setOutOption( OutOption outOption );
	
	void resetTransitionNodes();
	
	void StopEditTransitionMode();

  protected:
	
	//TRANSITIONS
	ofxUISuperCanvas *transitionEditorGui;
	map< string, TransitionInfo> transitionOptionMap;
	map<string, string> transitionTypes;
	map<string, map<string, TransitionInfo> > transitionMap;
	map<string, ofxUISuperCanvas*> transitionsGuis;
	bool bTransitionsAddedToGui;
	string currentTransitionType;
	ofxTween::ofxEasingType transitionEase;
	OutOption transitionOutOption;
	bool bMoveTransitionCameraUp, bMoveTransitionCameraDown;
	
	//MESH
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

	float portalScale;
	float portalTugMaxDistance;
	float portalTugMinDistance;
	float distanceToQuestion;

	
	bool bPortalDebugOn;
	ofVec3f portalBaseHover;
	CloudsPortal leftPortal;
	CloudsPortal rightPortal;
	
//	void addPortalPositionParams(CloudsPortal& portal, string side);
	
	vector<CloudsPortal*> portals;
	CloudsPortal* caughtPortal;
    CloudsPortal* selectedPortal;

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