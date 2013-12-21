/*
 *  CloudsPortalRing.h
 *  Empty
 *
 *  Created by Jim on 12/21/13.
 *  Copyright 2013 University of Washington. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

typedef struct {
	float curentRot;
	ofIndexType startIndex;
	ofIndexType endIndex;
	vector< pair<ofIndexType, ofIndexType> > segments;
} PortalShard;

class CloudsPortal;
class CloudsPortalRing {
  public:
	CloudsPortalRing();
	void setup(CloudsPortal* parent, ofVboMesh& portalGeo, int ringIndex);
	vector<PortalShard> shards;
	void addVertsAtAngle(float angle, bool endCap = false);
	
	float thickness; 
	float radius;
	float degreesPerSegment;
	float degreesPerQuad;
	ofVboMesh* geo;
};
