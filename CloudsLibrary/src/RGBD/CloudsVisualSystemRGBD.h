#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"
#include "ParticleConnectionGenerator.h"
#include "CloudsRGBDCamera.h"
#include "CloudsQuestion.h"

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

  protected:
	
	CloudsRGBDCamera cloudsCamera;
	ofMesh horizontalScanLines;
	ofMesh verticalScanLines;
	ofMesh pointGrid;
	ofMesh randomPoints;
	ParticleConnectionGenerator connectionGenerator;

	vector<CloudsQuestion> questions;
	
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

	bool drawMesh;
	float meshAlpha;
	
	float eyeMultiplier;
	float mouthMultiplier;
	float skinMultiplier;
	
	float cloudFlow;
	float currentFlowPosition;
	
	float numRandomPoints;
	void generateRandomPoints();
	
	bool refreshScanlineMesh;
	float verticalScanlineAlpha;
	float verticalScanlineThickness;
	float horizontalScanlineAlpha;
	float horizontalScanlineThickness;
	void generateScanlines();
	
	
};