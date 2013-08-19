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
#include "ofxGameCamera.h"
#include "CloudsClip.h"
#include "CloudsQuestion.h"

class CloudsIntroSequence : public CloudsVisualSystem {
  public:
	CloudsIntroSequence();
	~CloudsIntroSequence();
	
	virtual string getSystemName();
	
    virtual void selfSetup();
    virtual void selfSetupGuis();
    
    virtual void selfUpdate();
    virtual void selfDrawBackground();
    virtual void selfDrawDebug();

	virtual void timelineBangEvent(ofxTLBangEventArgs& args);
	
    virtual void selfDraw();
    virtual void selfExit();
    virtual void selfBegin();
	virtual void selfEnd();
	
    virtual void selfKeyPressed(ofKeyEventArgs & args);
    virtual void selfKeyReleased(ofKeyEventArgs & args);
    
    virtual void selfMouseDragged(ofMouseEventArgs& data);
    virtual void selfMouseMoved(ofMouseEventArgs& data);
    virtual void selfMousePressed(ofMouseEventArgs& data);
    virtual void selfMouseReleased(ofMouseEventArgs& data);
	
	virtual void selfDrawOverlay();
	virtual void selfPostDraw();

	virtual void selfGuiEvent(ofxUIEventArgs &e);
	
    virtual void selfSetupSystemGui();
    virtual void guiSystemEvent(ofxUIEventArgs &e);
    
    virtual void selfSetupRenderGui();
    virtual void guiRenderEvent(ofxUIEventArgs &e);

	virtual void selfSetupCameraGui();
	
	virtual ofCamera& getCameraRef(){
		if(useDebugCamera){
			return camera;
		}
		else{
			return warpCamera;
		}
	}
	
	void selfPresetLoaded(string presetPath);
	
	void setStartQuestions(vector<CloudsClip>& possibleStartQuestions);
	bool isStartQuestionSelected();
	CloudsQuestion* getSelectedQuestion();

  protected:
	
	ofxUISuperCanvas* questionGui;
	ofxUISuperCanvas* tunnelGui;
	
	bool showingQuestions;
	float questionWrapDistance;
	float cameraForwardSpeed;
	
	float questionTugMinDepth;
	float questionTugMinDistance;
	float questionTugMaxDistance;
	float questionSize;
	float questionTunnelInnerRadius;
	
	//TODO pick a better font renderer
	ofTrueTypeFont font;
	
	void positionStartQuestions();
	vector<CloudsQuestion> startQuestions;
	CloudsQuestion* selectedQuestion;
	CloudsQuestion* caughtQuestion;
	
	float fontSize;
	float fontExtrusion;
	float fontScale;
	
	float currentFontSize;
	float currentFontExtrusion;
	
	float perlinOffset;
	float wireframeAlpha;
	
	void reloadShaders();
	bool paused;
	void pauseAtBeginning();
	
	ofMesh blocksMesh;
	ofMesh tunnelMeshTight;
	ofMesh tunnelMeshLoose;
	
	bool useDebugCamera;
	ofxGameCamera camera;
	ofCamera warpCamera;
	
	ofShader tunnelShader;
	ofShader questionShader;
	ofShader chroma;
	
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
	float tightTunnelStepX;
	float tightTunnelStepZ;
	float looseTunnelStepX;
	float looseTunnelStepZ;
	
//	ofxExtrudedText cloudsTypeMesh;
	ofMesh thickTypeMesh;
	ofMesh thinTypeMesh;


};