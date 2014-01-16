//
//  CloudsRGBDPointLayer.h
//  CloudsPointcloudViewer
//
//  Created by Tekrental on 1/15/14.
//
//

#pragma once
#include "ofMain.h"
#include "ofRange.h"
#include "ofxUI.h"

class CloudsRGBDPointLayer
{
  public:
    
    CloudsRGBDPointLayer();
    ~CloudsRGBDPointLayer();
    
    void setDefaults();
    
    ofxUISuperCanvas* pointsGui;
    
    ofxUISuperCanvas* createGui(ofxUISuperCanvas* gui, string name);
    void selfGuiEvent(ofxUIEventArgs &e);
 
    void update();
    void draw();
    
    //set from the main app
	ofShader* pointShader;
    float* visualSystemFadeValue;

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
    float pointFaceMinRadius;
    float pointFaceFalloff;
    float edgeAttenuate;

	bool refreshPointcloud;
	void generatePoints();
    bool eventsRegistered;
    
    float actuatorPosition;
    float actuatorSpeed;
    float actuatorPhase;
    float actuatorAngle;
    float actuatorMaxAffect;
    ofVec3f actuator;
    
    float normalMix;
    float normalExtend;
    
};
