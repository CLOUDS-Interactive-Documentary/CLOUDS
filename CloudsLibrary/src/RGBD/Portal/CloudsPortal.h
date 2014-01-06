/*
 *  CloudsPortal.h
 *  Empty
 *
 *  Created by Jim on 12/21/13.
 *  Copyright 2013 CLOUDS. All rights reserved.
 *
 */

#pragma once

#include "ofxFTGL.h"
#include "CloudsClip.h"
#include "CloudsPortalRing.h"

class CloudsPortal {
  public:
	static void reloadShader();
    static ofShader shader;
    
	CloudsPortal();
	~CloudsPortal();	
	
	void setup();
	void update();
	void draw();
    void drawOverlay(bool anchorToScreen = false);
    
    CloudsClip clip; //must have a question
	string topic;
	string question;
    
    void startHovering();
	void stopHovering();
    bool hovering;
	float hoverStartTime;
	
	bool isSelected();
    bool selected;
	float selectedTime;
    
    float charsPerSecond;

	//GUI VARIABLES
	float ringSegments;
	float ringRadiusStep;
	float ringThickness;
	float ringThicknessMultiplier;
	float ringStartRadius;
	float ringDensity;
	
	float minSelectDistance;
	float maxHoverTime;
		
	float hoverPercentComplete;
	ofVec3f hoverPosition;
	ofVec2f screenPosition;//filled out on update
    
    float scale;
	
	ofCamera* cam;
	ofxFTGLSimpleLayout* font;

	vector<CloudsPortalRing> rings;
	void toggleFakeSelection();
	ofVboMesh portalGeo;
};