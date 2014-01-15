#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"
#include "CloudsPortal.h"
#include "CloudsQuestion.h"
#include "CloudsEvents.h"
#include "GPUParticles/Controller.h"
#include "ofxGameCamera.h"
#include "ofxFTGL.h"
#include "CloudsPortalEvents.h"

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

	void speakerChanged();
    
    ////////QUESTIONS
    void addQuestion(CloudsClip& questionClip, string topic, string question);
    void clearQuestions();
    bool isQuestionSelectedAndClipDone();
	bool isQuestionSelected();
    CloudsPortal* getSelectedQuestion();
    string getQuestionText();
    ////////QUESTIONS

	float visualSystemFadeValue;
	
	ofCamera& getCameraRef(){
		if(placingTransitionNodes){
			return transitionCam;
		}
		return cloudsCamera;
	}
	
	/////TRANSITIONS
	void startCurrentTransitionOut();
	void startCurrentTransitionIn();
	void startTransitionOut(RGBDTransitionType transitionType, string option="default");
	void startTransitionIn(RGBDTransitionType transitionType, string option="default");
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
	
	void setTransitionNodes( RGBDTransitionType transitionType, string option="default" );
    
	void loadTransitionOptions(string filename );
	void saveTransitionSettings(string filename = "Transitions");
	void setTransitionNodes( string transitionName );
	
	void addTransitionGui(string guiName);
	void clearTransitionMap();
	
	void addTransionEditorsToGui();
	void setTransitionNodes( string type, string option );
	void setOutOption( OutOption outOption );
	
	void resetTransitionNodes();
	
	void StopEditTransitionMode();
    //////////TRANSITIONS
    
	void playTestVideo();
    
    static CloudsVisualSystemEvents events;
    
  protected:
	
	//TRANSITIONS
	map< string, TransitionInfo> transitionOptionMap;
	map<string, string> transitionTypes;
	map<string, map<string, TransitionInfo> > transitionMap;
	map<string, ofxUISuperCanvas*> transitionsGuis;
	bool bTransitionsAddedToGui;
	string currentTransitionType;
	ofxTween::ofxEasingType transitionEase;
	OutOption transitionOutOption;
	bool bMoveTransitionCameraUp, bMoveTransitionCameraDown;
	float transitionScrubIn, transitionScrubOut;
	
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
    
	float pointXSimplify;
    float pointYSimplify;
    
    int pointCount;
	bool drawPoints;
	float pointAlpha;
	int numRandomPoints;
	ofRange pointSize;
	float pointHeadOverlap;
	float pointFlowPosition;
	float pointFlowSpeed;
	bool pointsFlowUp;
    float pointColorBoost;
    float pointSkinBoost;
	bool refreshPointcloud;
	void generatePoints();
	
	//LINES
	ofxUISuperCanvas *linesGui;
	ofShader lineShader;
	ofVbo lines;
    int lineVertexCount;
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
    float lineColorBoost;
    float lineSkinBoost;
    float lineMaxActuatorRetract;
    float lineRandomOffset;
	bool refreshLines;
	void generateLines();
	
	//MESH
	ofxUISuperCanvas *meshGui;
	ofShader meshShader;
//	ofVboMesh mesh;
    ofVbo mesh;
    int meshVertexCount;
	bool drawMesh;
	float meshAlpha;
	float xSimplify;
	float ySimplify;
	float randomVariance;
	float meshFaceMinRadius;
	float meshFaceFalloff;
	float meshRetractionFalloff;
	float meshForceGeoRetraction;
    float meshColorBoost;
    float meshSkinBoost;
    float meshMaxActuatorRetract;
    bool refreshMesh;
	void generateMesh();

    ///OCCLUSION
	ofxUISuperCanvas *occlusionGui;
    bool bDrawOcclusion;
    bool drawOcclusionDebug;

    ofShader occlusionShader;
    ofVbo occlusion;
    int occlusionVertexCount;
   	float occlusionXSimplify;
	float occlusionYSimplify;
    float occlusionMeshFaceMinRadius;
    float occlusionMeshFaceFalloff;
	float occlusionMeshRetractionFalloff;
    bool refreshOcclusion;
    void generateOcclusion();
    void drawOcclusionLayer();
    
    ///ACTUATORS
    float actuatorSpinPosition;
    float actuatorSpinSpeed;
    float actuatorShineAngle;

    void updateActuators();
    ofVec3f pointActuator;
    ofVec3f lineActuator;
    ofVec3f meshActuator;
    
    
	
	void loadShader();

	ofxUISuperCanvas *cameraGui;
	ofxUISuperCanvas *particleGui;
	ofxUISuperCanvas *questionGui;
    ofxUISuperCanvas *actuatorGui;
    
	//TODO: move to hud
	//ofxFTGLSimpleLayout displayFont;
	bool drawParticulate;
	float attenuatedCameraDrift;
	GPUParticles::Controller particulateController;
    ofVec4f pointColor;


    
    ///PORTALS
	float portalScale;
    ofRange portalTugDistance;
	float minDistanceToQuestion;
	bool bPortalDebugOn;
	ofVec3f portalBaseHover;
	CloudsPortal leftPortal;
	CloudsPortal rightPortal;
	vector<CloudsPortal*> portals;
	CloudsPortal* caughtPortal;
    CloudsPortal* selectedPortal;
	void updateQuestions();
	void drawQuestions();

	
	bool placingTransitionNodes;
	bool drawTransitionNodes;
	
	//transition
	bool transitioning, transitioningIn, transitioningOut, bResetLookThoughs;
	float transitionStartTime, transitionEndTime, transitionStartVal, transitionTargetVal;
	string activeTransition;
	
	RGBDTransitionType transitionType;
	
	float transitionVal;
	
//	ofVec3f questionXZ;
//	float questionDriftRange;
//	float questionYCenter;
//	float questionYDriftRange;
//	float questionYRange;
//	float questionLifeSpan; //minutes
	
//	ofFloatColor questionBaseHSB;
//	ofFloatColor questionHoverHSB;
};
