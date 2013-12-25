/*
 *  CloudsPortal.h
 *  Empty
 *
 *  Created by Jim on 12/21/13.
 *  Copyright 2013 CLOUDS. All rights reserved.
 *
 */

#pragma once

#include "CloudsPortalRing.h"

class CloudsPortal {
  public:
	CloudsPortal();
	~CloudsPortal();	
	
	void setup();
	void update();
	void draw();
	
	bool hovering;
	float hoverStartTime;
	
	bool selected;
	float selectedTime;

	//GUI VARIABLES
	float ringSegments;
	float ringRadiusStep;
	float ringThickness;
	float ringThicknessMultiplier;
	float ringStartRadius;
	float ringDensity;
	
	float minSelectDistance;
	float maxHoverTime;
	
	//GUI VARIABLES
	
	float hoverPercentComplete;
	ofVec3f hoverPosition;
	ofVec2f screenPosition;//filled out on update
	
	ofCamera* cam;
	
	ofShader portalShader;
	vector<CloudsPortalRing> rings;
	void toggleFakeSelection();
	ofVboMesh portalGeo;
};