/*
 *  CloudsPortalRing.cpp
 *  Empty
 *
 *  Created by Jim on 12/21/13.
 *  Copyright 2013 University of Washington. All rights reserved.
 *
 */

#include "CloudsPortalRing.h"
#include "CloudsPortal.h"

CloudsPortalRing::CloudsPortalRing(){

}

void CloudsPortalRing::setup(CloudsPortal* parent, ofVboMesh& portalGeo, int ringIndex){
	
	//one shard for now
	thickness = parent->ringThickness + ringIndex*(parent->ringThicknessMultiplier);
	radius = parent->ringStartRadius + ringIndex*(thickness+parent->ringRadiusStep); //radius
	degreesPerSegment = 360. / parent->ringSegments;
	degreesPerQuad = degreesPerSegment / parent->ringDensity;
	geo = &portalGeo;
	
	cout << "Ring " << ringIndex << endl;
	cout << "	rad " << radius << endl;
	cout << "	thickness " << radius << endl;
	cout << "	deg / seg " << degreesPerSegment << endl;
	cout << "	deg / quad " << degreesPerQuad << endl;
	
	//for now make one shard
	PortalShard shard;
	shard.startIndex = portalGeo.getNumVertices();
	for(int i = 0; i < parent->ringSegments; i++){
		
		pair<ofIndexType, ofIndexType> segmentPair;
		segmentPair.first = portalGeo.getNumVertices();
		float startAngle = degreesPerSegment*i;
		float endAngle	 = degreesPerSegment*(i+1);

		addVertsAtAngle(startAngle,true);
		
		for(float angle = startAngle; angle <= endAngle; angle += degreesPerQuad){
			addVertsAtAngle(angle);
		}
		
		addVertsAtAngle(endAngle,true);
		
		segmentPair.second = portalGeo.getNumVertices()-1;
		shard.segments.push_back( segmentPair );
	}
	
	shard.endIndex = portalGeo.getNumVertices()-1;
	shards.push_back(shard);
	
}

void CloudsPortalRing::addVertsAtAngle(float angle, bool endCap){
	
	ofVec3f vInner(0,1,0);
	ofVec3f vOuter(0,1,0);
	vInner.rotate(angle, ofVec3f(0,0,1));
	vOuter.rotate(angle, ofVec3f(0,0,1));
	vInner *= radius;
	vOuter *= radius+thickness;
	
	cout << "added geo at " << vInner << " " << vOuter << endl;
	
	geo->addVertex(vInner);
	geo->addColor(endCap ? ofFloatColor(0,0) : ofFloatColor::red);
	geo->addVertex(vOuter);
	geo->addColor(endCap ? ofFloatColor(0,0) : ofFloatColor::blue);
}