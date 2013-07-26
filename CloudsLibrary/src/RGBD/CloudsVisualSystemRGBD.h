#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"
#include "ParticleConnectionGenerator.h"
#include "CloudsRGBDCamera.h"
#include "CloudsQuestion.h"
#include "GPUParticles/Controller.h"

class CloudsVisualSystemRGBD : public CloudsVisualSystem {
  public:

	string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    void selfDraw();
	void selfDrawOverlay();
    void selfExit();
    void selfBegin();
	void selfEnd();
    
	
	void addQuestion(CloudsClip& q);
	
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
	
	ofCamera& getCameraRef(){
		return  cloudsCamera;
	}
	
	void transitionIn( RGBDTransitionType transitionType, float duration, float startTime=ofGetElapsedTimef() );
	void transitionOut( RGBDTransitionType transitionType, float duration, float startTime=ofGetElapsedTimef() );
	
	void transition( ofVec3f startPos, ofVec3f endPos, ofQuaternion startRotf, ofQuaternion endRot, float duration=3, float startTime=ofGetElapsedTimef() );

  protected:

	ofxUISuperCanvas *meshGui;
	ofxUISuperCanvas *cameraGui;
	ofxUISuperCanvas *particleGui;
	ofxUISuperCanvas *questionGui;
	
	ofTrueTypeFont displayFont;
	vector<CloudsQuestion> currentQuestions;
	
	ofVec3f translatedHeadPosition;
	CloudsRGBDCamera cloudsCamera;
	ofMesh horizontalScanLines;
	ofMesh verticalScanLines;
	ofMesh pointGrid;
	ofMesh randomPoints;
	
	ParticleConnectionGenerator connectionGenerator;
	GPUParticles::Controller particulateController;
	
	void updateQuestions();
	void drawQuestions();
	
	vector<CloudsQuestion*> questions;
	
	ofVec4f pointColor;
	
	float pointcloudScale;
	float pointcloudOffsetZ;

	ofVec2f scanlineSimplify;
	
	bool drawPoints;
	
	bool refreshPointcloud;
	float pointSizeMin;
	float pointSizeMax;
	
	float pointGridAlpha;
	float randomPointAlpha;

	bool drawScanlines;
	float pointVerticalSpace;
	float pointHorizontalSpace;
	void generatePointGrid();
	
	bool drawParticulate;
	bool drawCloud;
	bool drawMesh;
	float meshAlpha;
	
	float eyeMultiplier;
	float mouthMultiplier;
	float skinMultiplier;
	
	float cloudFlow;
	float currentFlowPosition;
	
	float numRandomPoints;
	void generateRandomPoints();
	
	ofShader rgbdShader;
	
	//QUESTION
//	ofVec3f questionOffset;
//	float questionRange;
//	float questionFloat;
	
	bool refreshScanlineMesh;
	float verticalScanlineAlpha;
	float verticalScanlineThickness;
	float horizontalScanlineAlpha;
	float horizontalScanlineThickness;
	void generateScanlines();
	
	//transition
	void updateTransition();
	bool transitioning, transitioningIn, transitioningOut;
	float transitionStartTime, transitionEndTime, transitionStartVal, transitionTargetVal;
	
	
	//???: maybe we should combine these into a struct?
	ofVec3f transitionStartPos, transitionEndPos;
	ofQuaternion transitionStartRot, transitionEndRot;
	ofxEasingQuint transitionEase;
	
	ofMatrix4x4 transitionMatrix;
	
	RGBDTransitionType transitionType;
	
	float transitionVal;
};