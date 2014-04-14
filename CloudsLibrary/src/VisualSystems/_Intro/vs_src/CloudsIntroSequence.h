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
	float cursorDistance;
	bool hover;
	float hoverStartTime;
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
	
	ofxFTGLSimpleLayout questionFont;
    void rebuildQuestionFont();
    int questionFontSize;
	
	float questionScale;
	ofRange questionTugDistance;
	ofRange questionAttenuateDistance;
	
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
    void drawIntroNodes();
	
	//intro sequence
	float introNodeSize;
	float introNodeMinDistance;
	float introNodeHoldTime;
	ofVec3f introNodeOffset; //mirrored along the axis
	IntroNode introNodeOne;
	IntroNode introNodeTwo;

	float timeSinceLastPrompt;
	float promptTime;
	bool promptShown;
	
	void updateWaiting();
	void updateTitle();
	void updateQuestions();
	
	//intro state machien stuff
	bool startedOnclick;
	bool introNodeHoverOne;
	bool introNodeHoverTwo;
	
	float perlinAmplitude;
	float perlinDensity;
	float perlinSpeed;
	
	//we find these from the model when we open the tunnel
	ofVec3f tunnelMax;
	ofVec3f tunnelMin;
	float tunnelDistance;
	float tunnelStartZ;
	
    //hack with get input not working
    ofVec2f inputPosition;
    
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

