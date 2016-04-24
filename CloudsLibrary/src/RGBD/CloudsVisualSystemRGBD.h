#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"
#include "CloudsPortal.h"
#include "CloudsEvents.h"

#ifdef HAS_GAMECAM
#include "ofxGameCamera.h"
#endif

#include "ofxFTGL.h"
#include "CloudsPortalEvents.h"
#include "CloudsRGBDPointLayer.h"
#include "VoxelMesh.h"

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

typedef struct {
    CloudsClip* clip;
    string question;
    string topic;
} QuestionQueue;

class CloudsVisualSystemRGBD : public CloudsVisualSystem {
  public:
	
	enum OutOption{
		OutLeft,
		OutRight
	};

	CloudsVisualSystemRGBD();

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
    
	void loadBackgroundGUISFromName(string presetName);
	void loadPointcloudGUISFromName(string presetName);
	vector<ofxUISuperCanvas*> pointcloudGuis;
	vector<ofxUISuperCanvas*> backgroundGuis;
	
    ////////QUESTIONS
    void addQuestion(CloudsClip* questionClip, string topic, string question);
    void clearQuestions();
	void assignAvailableQuestion(CloudsPortal& p);
	
    bool isQuestionSelectedAndClipDone();
	bool isQuestionSelected();
	bool isResetSelected();

    CloudsPortal* getSelectedQuestion();
    string getQuestionText();
    vector<QuestionQueue>& getQuestionQueue();
    //called from Playback controller during screening mode to remove selected question by clustermap
    void removeQuestionFromQueue(CloudsClip* clip);
//	#ifdef CLOUDS_SCREENING
//	bool hasQuestionsRemaining();
//	void clearQuestionQueue();
//	#endif
    ////////QUESTIONS

	float getRGBDTransitionValue();
	float visualSystemFadeValue;
    int questionToReplace;
    
	ofCamera& getCameraRef(){
#ifdef HAS_GAMECAM
		if(placingTransitionNodes){
			return transitionCam;
		}
#endif
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
	ofNode transitionOutReset;

	ofVec3f transitionEndPosition;
	ofQuaternion transitionEndRotation;
	
	bool bTransitionIn, bTransitionOut;
	bool bLookThroughIn, bLookThroughOutLeft, bLookThroughOutRight;
	bool bSaveTransition;
	
	void lookThroughTransitionIn();
	void lookThroughTransitionOutLeft();
	void lookThroughTransitionOutRight();

	#ifdef HAS_GAMECAM
	ofxGameCamera transitionCam;
	#endif
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
	void resetInTransitionNode();
	void resetLeftTransitionNode();
	void resetRightTransitionNode();
	
	void StopEditTransitionMode();
    //////////TRANSITIONS
    
	void playTestVideo(CloudsClip* clip);
	void playTestVideo(int index = 0);
    
    static CloudsVisualSystemEvents events;
    
    bool paused;
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

	//ofxUISuperCanvas *pointsGui;
    CloudsRGBDPointLayer pointLayer1;
    CloudsRGBDPointLayer pointLayer2;
    
	ofShader pointShader;
	
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

    //FILL
	ofxUISuperCanvas *fillGui;
    bool bEnableFill;
    float fillAlpha;
	float fillFaceFalloff;
	float fillRetractionFalloff;
    float fillFaceMinRadius;

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
	ofxUISuperCanvas *backgroundMeshGui;
	ofxUISuperCanvas *questionGui;
    ofxUISuperCanvas *actuatorGui;
    
	bool drawParticulate;
	float attenuatedCameraDrift;
	VoxelMesh voxelMesh;
    float particleCount;
    ofVec4f pointColor;
    
    ///PORTALS
	float portalScale;
    ofRange portalTugDistance;
	float minDistanceToQuestion;
	bool bPortalDebugOn;
	ofVec3f portalBaseHover;
	ofVec3f resetHoverPosition;

	CloudsPortal leftPortal;
	CloudsPortal rightPortal;
#ifdef OCULUS_RIFT
	CloudsPortal resetPortal;
#endif
	CloudsPortal* portalToClear; //clears when started
	string questionText;
	
	ofxFTGLFont questionFont;
	int questionFontSize;
	int currentQuestionFontSize;
	float questionFontScale;
	float questionYOffset;
	float questionFontTracking;
	float questionFontSplitWidth;
	void drawQuestionType();
	
	vector<CloudsPortal*> portals;
    vector<QuestionQueue> questions;
	CloudsPortal* caughtPortal;
    CloudsPortal* selectedPortal;
	void updateQuestions();
	void drawQuestions();
	
    GLuint cullFace;
    void drawCursors();
    ofVec3f cursor;
    ofVec3f stickyCursor;
    
	
	//transition
	bool placingTransitionNodes;
	bool drawTransitionNodes;
	bool transitioning, transitioningIn, transitioningOut, bResetLookThoughs, bResetRight,bResetLeft,bResetIn;
	float transitionStartTime, transitionEndTime, transitionStartVal, transitionTargetVal;
	string activeTransition;
	
	RGBDTransitionType transitionType;
	
	float transitionVal;
};