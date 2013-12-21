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
	float thickness = parent->ringThickness + ringIndex*(parent->ringThicknessMultiplier);
	float radius = parent->ringStartRadius + ringIndex*(thickness+parent->ringRadiusStep); //radius
	float degreesPerSegment = 360. / parent->ringSegments;
	float degreesPerQuad = degreesPerSegment / parent->ringDensity;
	
	cout << "Ring " << ringIndex << endl;
	cout << "	rad " << radius << endl;
	cout << "	thickness " << radius << endl;
	cout << "	deg / seg " << degreesPerSegment << endl;
	cout << "	deg / quad " << degreesPerQuad << endl;
	
	for(int i = 0; i < parent->ringSegments; i++){
		
		for(float angle = degreesPerSegment*i; angle <= degreesPerSegment*(i+1); angle += degreesPerQuad){
			ofVec3f vInner(0,1,0);
			ofVec3f vOuter(0,1,0);
			vInner.rotate(angle, ofVec3f(0,0,1));
			vOuter.rotate(angle, ofVec3f(0,0,1));
			vInner *= radius;
			vOuter *= radius+thickness;
			
			cout << "added geo at " << vInner << " " << vOuter << endl;
			
			portalGeo.addVertex(vInner);
			portalGeo.addColor(ofColor::red);
			portalGeo.addVertex(vOuter);
			portalGeo.addColor(ofColor::blue);
		}
	}
}