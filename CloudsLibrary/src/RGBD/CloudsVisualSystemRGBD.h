#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"
//#include "ParticleConnectionGenerator.h"
#include "CloudsQuestion.h"
#include "GPUParticles/Controller.h"
#include "ofxGameCamera.h"

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
	
	
	//???: LB- I changed this so that we could use the "transitionCam" to position our in and out nodes
	ofCamera& getCameraRef(){
		if(currentCamera != NULL)	return  *currentCamera;
		return cloudsCamera;
	}

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
	
	bool drawTransitionNodes;
	
	void lookThroughTransitionIn();
	void lookThroughTransitionOut();

	ofxGameCamera transitionCam;
	ofNode* transitionCamTargetNode;
	
	void printTransitionNodes();
	void setTransitionNodes( RGBDTransitionType transitionType );

  protected:
	

	ofxUISuperCanvas *meshGui;
	ofxUISuperCanvas *cameraGui;
	ofxUISuperCanvas *particleGui;
	ofxUISuperCanvas *questionGui;
	ofxUISuperCanvas *connectorGui;
	
	ofTrueTypeFont displayFont;
	ofMesh horizontalScanLines;
	ofMesh verticalScanLines;
	ofMesh pointGrid;
	ofMesh randomPoints;
	
//	ParticleConnectionGenerator generator;
	GPUParticles::Controller particulateController;
	
	void updateQuestions();
	void drawQuestions();
	
	vector<CloudsQuestion*> questions;
	
	ofVec4f pointColor;

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
	
	//ofMatrix4x4 transitionMatrix;
	
	RGBDTransitionType transitionType;
	
	float transitionVal;
	
	
};