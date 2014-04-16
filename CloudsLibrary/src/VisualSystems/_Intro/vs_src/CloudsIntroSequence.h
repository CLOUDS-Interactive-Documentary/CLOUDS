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

//basic little struct for intro node

typedef struct{
	ofVec3f worldPosition;
	ofVec2f screenPosition;
	ofVec2f cursorDirection;
	float cursorDistance;
	bool hover;
	bool finished;
	float percentComplete;
	float hoverStartTime;
	float gazePercent;
	int multiplier;
} IntroNode;

class CloudsIntroSequence : public CloudsVisualSystem {
  public:
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
	
	void setStartQuestions(vector<CloudsClip>& possibleStartQuestions);

	bool isStartQuestionSelected();
	bool istStartQuestionHovering();
	
	string getQuestionText();
	
	void autoSelectQuestion();
	CloudsPortal* getSelectedQuestion();
    
    static CloudsVisualSystemEvents events;
    
	ofCamera& getCameraRef(){
		return warpCamera;
	}


	
  protected:
		
	ofxUISuperCanvas* questionGui;
	ofxUISuperCanvas* tunnelGui;
	ofxUISuperCanvas* typeGui;
	ofxUISuperCanvas* introGui;
	ofxUISuperCanvas* helperTextGui;
	
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
	float questionMinZDistance;
	float currentFontSize;
	float currentFontExtrusion;

	void positionStartQuestions();
	
	vector<CloudsPortal> startQuestions;
	CloudsPortal* selectedQuestion;
	CloudsPortal* caughtQuestion;

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

	ofShader tunnelShader;
	ofShader typeShader;
	
	ofRange pointSize;	
	ofRange distanceRange;
	
	void drawCloudsType();
    void drawIntroNodes();//rift only
	void drawHelperType();
	void drawTunnel();
	void drawPortals();
	
	void updateIntroNodePosition(IntroNode& node);
	void updateIntroNodeInteraction(IntroNode& node);
	
	//intro sequence
	float introNodeSize;
	float introNodeMinDistance;
	float introNodeHoldTime;
	
	ofVec3f introNodeOffset; //mirrored along the axis

	IntroNode introNodeOne;
	IntroNode introNodeTwo;
	IntroNode introNodeThree;
	vector<IntroNode*> introNodes;
	float nodeAlphaAttenuate;
	float nodeActivatedTime;
	ofVec2f hintCursorEndPoint;


	
	float timeSinceLastPrompt;
	float promptTime;
	bool promptShown;
	float kinectHelperAlpha;
	float kinectHelperTargetAlpha;
	
	void updateCamera();
	void updateWaiting();
	void updateTitle();
	void updateQuestions();
	
	//intro state machien stuff
	bool startedOnclick;
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
	
	ofFloatColor tint;
	ofFloatColor questionNodeTint;
	
	ofImage sprite;

    ofVec3f cursor;
    
#ifdef OCULUS_RIFT
    bool bCursorInCenter;
    float startTimeCursorInCenter;
#endif
    
};

