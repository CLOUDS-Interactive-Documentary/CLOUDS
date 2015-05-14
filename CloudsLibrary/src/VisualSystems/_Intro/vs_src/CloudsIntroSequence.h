//
//  IntroSequence.h
//  ComputerTicker
//
//  Created by James George on 5/26/13.
//
//

#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"
#include "CloudsClip.h"
#include "CloudsEvents.h"
#include "CloudsPortal.h"
#include "ofxFTGL.h"
#include "CloudsPortalEvents.h"
#include "CloudsCalibrationNode.h"


typedef struct {
    ofRectangle bounds;
    string label;
    bool visible;
    bool hovered;
    bool pressed;
    bool clicked;
    float attenuation;
    float baseAlpha;
    float targetAlpha;
} CloudsMenuItem;

typedef enum {
    
    CLOUDS_INTRO_LOADING = 0,
    CLOUDS_INTRO_VHX_WAITING_CODE,
    CLOUDS_INTRO_VHX_SHOWING_CODE,
    CLOUDS_INTRO_VHX_NO_PURCHASE,
    CLOUDS_INTRO_VHX_RENTAL_EXPIRED,
    CLOUDS_INTRO_VHX_PACKAGE_VALIDATED,
    CLOUDS_INTRO_VHX_ERROR,
    
    CLOUDS_INTRO_MENU,
    CLOUDS_INTRO_MENU_NEW_RESUME,
    CLOUDS_INTRO_PLAYING,
    CLOUDS_INTRO_RESUMING,
    CLOUDS_INTRO_RESEARCH,
    CLOUDS_INTRO_NO_MEDIA,
    CLOUDS_INTRO_ABOUT
    
} CloudsIntroState;

class CloudsIntroSequence : public CloudsVisualSystem {
  public:
	CloudsIntroSequence();
	
	string getSystemName();
	
    void selfSetup();
    void selfSetupGuis();
    void selfSetDefaults();
	
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();

	void timelineBangEvent(ofxTLBangEventArgs& args);
	
    void selfDraw();
    void selfExit();
    void selfBegin();
	void selfEnd();

	void selfDrawOverlay();
	void selfPostDraw();

    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);

	void selfGuiEvent(ofxUIEventArgs &e);
	
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);

	void selfSetupCameraGui();
	
	void selfPresetLoaded(string presetPath);
	
	void setStartQuestions(vector<CloudsClip*>& possibleStartQuestions);

    bool isResearchModeSelected();
    bool isAboutScreenSelected();
    
	bool isStartQuestionSelected();
	bool istStartQuestionHovering();
	
	string getQuestionText();
	
	void autoSelectQuestion();
	CloudsPortal* getSelectedQuestion();
    
    bool userHasBegun();
    bool introNodesShown;
    float introNodeChangeTime;
    
    static CloudsVisualSystemEvents events;
    
    void loadingFinished();    
    float percentLoaded;
    bool firstPlay;
    bool shouldArchiveAct;
    
	ofCamera& getCameraRef(){
		return warpCamera;
	}

    //vhx stuff
    void vhxSetAuthCode(string code);
    void vhxNotPurchase();
    void vhxRentalExpired();
    void vhxAuthenticated();
    void vhxError();
         
  protected:
		
	ofxUISuperCanvas* questionGui;
	ofxUISuperCanvas* tunnelGui;
	ofxUISuperCanvas* typeGui;
	ofxUISuperCanvas* introGui;
	ofxUISuperCanvas* helperTextGui;
	ofxUISuperCanvas* menuGui;
	
	bool showingQuestions;
	float questionWrapDistance;
	float cameraForwardSpeed;
	
	ofxFTGLFont extrudedTitleText; //for the title
	float currentTitleOpacity;
	float titleTypeOpacity;
	int titleFontSize;
	int titleFontExtrude;
	float titleTypeTracking;
	float titleTypeOffset;
	
	float titleNoiseDensity;
	float titleMaxGlow;
	float titleMinGlow;
	float titleNoiseSpeed;
	float titleNoisePosition;

	float titleRectWidth;
	float titleRectHeight;
	ofRectangle titleRect;
	bool hoveringTitle;
	
	ofxFTGLFont helperFont;
    int helperFontSize;
	int currentHelperFontSize;
	float helperFontTracking;
	float helperFontY;
	float helperFontScale;
	
	float questionScale;
	ofRange questionTugDistance;
	ofRange questionAttenuateDistance;
	float questionPauseDuration;
	
	bool bQuestionDebug;
	float questionLineLength;
    float questionLineSpacing;
	float questionTunnelInnerRadius;
	ofRange questionZStopRange;
	float currentFontSize;
	float currentFontExtrusion;

	void positionStartQuestions();
	bool firstQuestionStopped;
    float firstQuestionStoppedTime;
    
	vector<CloudsPortal> startQuestions;
    CloudsPortal resumePortal;
	CloudsPortal* selectedQuestion;
	CloudsPortal* caughtQuestion;
	float selectedQuestionTime;
	ofVec3f selectQuestionStartPos;
	ofQuaternion selectQuestionStartRot;
		
	vector<bool> questionChannels; //0-3 for the four qeustion slots
	vector<float> channelPauseTime;
	
	vector<string> loadedQuestions;
	float perlinOffset;
	float wireframeAlpha;
	
	void reloadShaders();
	bool paused;

	ofMesh blocksMesh;
	ofMesh tunnelMeshTight;
	ofMesh tunnelMeshLoose;
	
	ofCamera warpCamera;
	
	float camWobbleRange;
	float camWobbleSpeed;
	
	float cursorAlpha;

	ofShader tunnelShader;
	ofShader typeShader;
	
	ofRange pointSize;	
	ofRange distanceRange;
	
	void drawCloudsType();
    void drawIntroNodes(); //rift only
	void drawHelperType();
	void drawTunnel();
	void drawPortals();
    void drawCursors();
    void drawMenu();
    
	void updateIntroNodePosition(CalibrationNode& node);
	void updateIntroNodeInteraction(CalibrationNode& node);
	
	//intro sequence
	float introNodeSize;
	float introNodeMinDistance;
	float introNodeHoldTime;
	float introNodeYAdjust;
	
	ofVec3f introNodeOffset; //mirrored along the axis

	CalibrationNode introNodeOne;
	CalibrationNode introNodeTwo;
	CalibrationNode introNodeThree;
	vector<CalibrationNode*> introNodes;
	float nodeAlphaAttenuate;
	float nodeActivatedTime;
	ofVec2f hintCursorEndPoint;

	float timeSinceLastPrompt;
	float promptTime;
	bool promptShown;
	float kinectHelperAlpha;
	float kinectHelperTargetAlpha;

	float mouseLastMovedTime;
    
    //menu stuff
    //intro state machine stuff
    CloudsIntroState currentState;
    void changeState(CloudsIntroState newState);
    float stateChangedTime;
    

    int menuFontSize;
    int currentMenuFontSize;
    int menuToolTipFontSize;
    int currentMenuToolTipFontSize;
    float menuButtonPad;
    float loadingCompleteTime;
    float menuYOffset;
    float menuWidth;
    float newResumeSpace;
	ofxFTGLFont menuFont;
	ofxFTGLFont menuToolTipFont;
    CloudsMenuItem researchMenuItem;
    CloudsMenuItem playMenuItem;
    CloudsMenuItem aboutMenuItem;
    CloudsMenuItem newMenuItem;
    CloudsMenuItem resumeMenuItem;
    bool researchSelected;
    bool aboutSelected;
    
    vector<CloudsMenuItem*> menuItems;
    
    
	void updateCamera();
	void updateWaiting();
	void updateTitle();
	void updateQuestions();
    void updateMenu();
	
	float perlinAmplitude;
	float perlinDensity;
	float perlinSpeed;
	
	//we find these from the model when we open the tunnel
	ofVec3f tunnelMax;
	ofVec3f tunnelMin;
	float tunnelDistance;
	float tunnelStartZ;
	
	bool regenerateTunnel;
	void generateTunnel();
	float looseTunnelResolutionX;
	float looseTunnelResolutionZ;

	ofVec2f cameraSwingRange;
	ofVec2f curCameraSwing;
	float cameraSwingDamp;

	ofFloatColor tint;
	ofFloatColor questionNodeTint;
	
	ofImage sprite;

    ofVec3f cursor;
    ofVec3f stickyCursor;
    bool successfullyPurchased;
    bool loadDidFinish;
    string currentAuthCode;
    bool showVHXPrompt;
    string vhxPromptScreen;
    
#ifdef OCULUS_RIFT
    bool bCursorInCenter;
    float startTimeCursorInCenter;
#endif
    
};

