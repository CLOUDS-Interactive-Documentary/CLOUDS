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
	int numSegments;
	float curentRot;
	ofIndexType startIndex;
	ofIndexType endIndex;
	ofFloatColor innerColor;
	ofFloatColor outerColor;
	vector< pair<ofIndexType, ofIndexType> > segments;
} PortalShard;

class CloudsPortal;
class CloudsPortalRing {
  public:
	CloudsPortalRing();
	void setup(CloudsPortal* parent, ofVboMesh& portalGeo, int ringIndex);
	vector<PortalShard> shards;
	void addVertsAtAngle(PortalShard& shard, float angle, bool endCap = false);
	
	float thickness; 
	float radius;
	float degreesPerSegment;
	float degreesPerQuad;
	ofFloatColor shardColorOut;
	ofFloatColor shardColorIn;
	ofVboMesh* geo;
};
