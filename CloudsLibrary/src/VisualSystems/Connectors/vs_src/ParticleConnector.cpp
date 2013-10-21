//
//  ParticleConnector.cpp
//  emptyExample
//
//  Created by James George on 5/29/13.
//


#include "ParticleConnector.h"
#include "ofRange.h"

//http://demonstrations.wolfram.com/RandomPointsOnASphere/

ofVec3f randomPointOnSphere(){
	
	float theta = ofRandom(TWO_PI);
	float u = ofRandomf();
	float usqrtinv = sqrt(1. - powf(u,2.));
	return ofVec3f(cos(theta) * usqrtinv,
				   sin(theta) * usqrtinv, u);	
};

ParticleConnector::ParticleConnector()
{
	
	direction = randomPointOnSphere();
	position = direction*500;
	binIndex1 = binIndex2 = 0;
}

ParticleConnector::~ParticleConnector(){
	
}

void ParticleConnector::update(){

	position += direction;
	
	if(position.x > upperBounds.x) position.x = lowerBounds.x + (position.x - upperBounds.x);
	if(position.y > upperBounds.y) position.y = lowerBounds.y + (position.y - upperBounds.y);
	if(position.z > upperBounds.z) position.z = lowerBounds.z + (position.z - upperBounds.z);
	
	if(position.x < lowerBounds.x) position.x = upperBounds.x - (position.x - lowerBounds.x);
	if(position.y < lowerBounds.y) position.y = upperBounds.y - (position.y - lowerBounds.y);
	if(position.z < lowerBounds.z) position.z = upperBounds.z - (position.z - lowerBounds.z);
	
}

