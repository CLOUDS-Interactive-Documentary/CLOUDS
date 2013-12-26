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
		
	
	
	for(int l = 0; l < 2; l++){ //three layers
		
		//either 2 or 5 shards per ring
		int numShards = (ofRandom(2,5)+.5);
		//how much of the ring does each shard represent
		//fill a vector with random variables and normalize it to see
		float shardPercents[numShards];
		float totalWeight = 0;
		for(int i = 0; i < numShards; i++){
			shardPercents[i] = ofRandom(.25, 1.0);
			totalWeight+=shardPercents[i];
		}
		for(int i = 0; i < numShards; i++){	
			shardPercents[i] /= totalWeight;
		}
		
		int totalSegments = 0;
		for(int i = 0; i < numShards; i++){
			PortalShard shard;
			shard.numSegments = ( (shardPercents[i] * parent->ringSegments)+.5);
			totalSegments += shard.numSegments;
			shards.push_back(shard);
		}
		
		cout << "total segments " << totalSegments << "/" << parent->ringSegments << endl;
		if(totalSegments < parent->ringSegments){
			shards[0].numSegments++;
		}
		if(totalSegments > parent->ringSegments){
			shards[0].numSegments--;
		}

		int currentSegment = 0;
		for(int i = 0; i < shards.size(); i++){
			
			PortalShard& shard = shards[i];
			shard.startIndex = portalGeo.getNumVertices();
			ofFloatColor baseColor = ofColor::fromHex(0x5798eb);
			if(ringIndex == 0){
				baseColor.setSaturation(0);
				baseColor.setBrightness(baseColor.getBrightness()*.25);
			}
			else if(ringIndex == 1){
				baseColor.setSaturation(baseColor.getSaturation()*.5);
				baseColor.setBrightness(baseColor.getBrightness()*.5);
			}
			//attenuate all saturation a little bit
			baseColor.setSaturation(baseColor.getSaturation()*ofRandom(.7,1.0));
//			if(l == 0){
//				shard.direction = 0.0;
//			}
//			else{
				shard.direction = ofRandomuf() > .5 ? -1. : 1;
				shard.direction *= ofMap(ringIndex, 0, 3, 1.0, .5); //slow it down
				shard.direction *= ofRandom(.5,1.5);
//			}
			
			shard.innerColor = shard.outerColor = baseColor;
			for(int s = currentSegment; s < currentSegment + shard.numSegments; s++){
			
				pair<ofIndexType, ofIndexType> segmentPair;
				segmentPair.first = portalGeo.getNumVertices();
				float startAngle = degreesPerSegment*s;
				float endAngle	 = degreesPerSegment*(s+1);
				float segmentPercent = (1.0 * currentSegment / parent->ringSegments);
				addVertsAtAngle(shard,startAngle,segmentPercent,true);
				
				for(float angle = startAngle; angle <= endAngle; angle += degreesPerQuad){
					addVertsAtAngle(shard,angle,segmentPercent, false);
				}
				
				addVertsAtAngle(shard,endAngle,segmentPercent,true);
				
				shard.innerColor.setBrightness(shard.innerColor.getBrightness()*.9);
				shard.outerColor.setBrightness(shard.outerColor.getBrightness()*.9);
				
				segmentPair.second = portalGeo.getNumVertices()-1;
				shard.segments.push_back( segmentPair );

			}
			
			currentSegment += shard.numSegments;
			shard.endIndex = portalGeo.getNumVertices()-1;
		}
	}
}

void CloudsPortalRing::addVertsAtAngle(PortalShard& shard, float angle, float segmentPercent, bool endCap){
	
	ofVec3f vInner(0,1,0);
	ofVec3f vOuter(0,1,0);
	vInner.rotate(angle, ofVec3f(0,0,1));
	vOuter.rotate(angle, ofVec3f(0,0,1));
	vInner *= radius;
	vOuter *= radius+thickness;
	
	geo->addColor(endCap ? ofFloatColor(0,0) : shard.innerColor);
	geo->addNormal(ofVec3f(shard.direction, segmentPercent, 1.));
	geo->addVertex(vInner);
	
	geo->addColor(endCap ? ofFloatColor(0,0) : shard.outerColor);
	geo->addNormal(ofVec3f(shard.direction, segmentPercent, 1.));
	geo->addVertex(vOuter);
}

void CloudsPortalRing::update(){
	
}