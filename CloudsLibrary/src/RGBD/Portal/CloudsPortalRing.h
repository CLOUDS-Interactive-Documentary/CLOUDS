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
	float direction;
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
	void setup(CloudsPortal* parent, ofVboMesh& portalGeo, int index);
	void update();
	
	//updated by update function in CloudsPortal;
	float hoverPercentComplete;
	
  protected:
	vector<PortalShard> shards;
	void addVertsAtAngle(PortalShard& shard, float angle, float segmentPercent, bool endCap = false);
	
	int ringIndex;
	float thickness; 
	float radius;
	float degreesPerSegment;
	float degreesPerQuad;
	ofFloatColor shardColorOut;
	ofFloatColor shardColorIn;
	ofVboMesh* geo;
};
