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
//#include "ofxGameCamera.h"
#include "CloudsClip.h"
#include "CloudsPortal.h"
//#include "CloudsRun.h"
#include "ofxFTGL.h"

class CloudsIntroSequence : public CloudsVisualSystem {
  public:
	CloudsIntroSequence();
	~CloudsIntroSequence();
	
	string getSystemName();
	
    void selfSetup();
    void selfSetupGuis();
    
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
	
	void selfDrawOverlay();
	void selfPostDraw();

	void selfGuiEvent(ofxUIEventArgs &e);
	
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);

	void selfSetupCameraGui();
	
	ofCamera& getCameraRef(){
//		if(useDebugCamera){
//			return camera;
//		}
//		else{
			return warpCamera;
//		}
	}
	
	void selfPresetLoaded(string presetPath);
	
	void setStartQuestions(vector<CloudsClip>& possibleStartQuestions);
	bool isStartQuestionSelected();
	void autoSelectQuestion();
	CloudsPortal* getSelectedQuestion();
//	CloudsRun& getSelectedRun(){
//		return currentRun;
//	};
	
  protected:
		
	ofxUISuperCanvas* questionGui;
	ofxUISuperCanvas* tunnelGui;
	ofxUISuperCanvas* typeGui;
	
	bool showingQuestions;
	float questionWrapDistance;
	float cameraForwardSpeed;
	
	float questionTugMinDepth;
	float questionTugMinDistance;
	float questionTugMaxDistance;
	float questionSize;
	float questionTunnelInnerRadius;
	
	bool startedOnclick;
	
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
	float questionLineLength;
    float questionLineSpacing;

	
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
	
	bool useDebugCamera;
//	ofxGameCamera camera;
	ofCamera warpCamera;
	
	float camWobbleRange;
	float camWobbleSpeed;

	ofShader tunnelShader;
	ofShader questionShader;
	ofShader chroma;
	ofShader typeShader;
	
	ofRange pointSize;	
	ofRange distanceRange;
	
	void drawCloudsType();

	float maxChromaDistort;
	float perlinAmplitude;
	float perlinDensity;
	float perlinSpeed;
	
	//we find these from the model when we open the tunnel
	ofVec3f tunnelMax;
	ofVec3f tunnelMin;
	float tunnelDistance;
	float tunnelStartZ;
	
	void generateTunnel();
	float looseTunnelResolutionX;
	float looseTunnelResolutionZ;
	
	ofFloatColor tint;
	ofFloatColor questionNodeTint;
	
	ofImage sprite;

    ofVec3f cursor;
    
};