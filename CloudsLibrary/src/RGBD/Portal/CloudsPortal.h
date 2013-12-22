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
	
	bool selected;
	float selectedTime;
	
	float ringSegments;
	float ringRadiusStep;
	float ringThickness;
	float ringThicknessMultiplier;
	float ringStartRadius;
	float ringDensity;
	
	ofShader portalShader;
	ofVec3f position;
	vector<CloudsPortalRing> rings;
	void toggleFakeSelection();
	ofVboMesh portalGeo;
};