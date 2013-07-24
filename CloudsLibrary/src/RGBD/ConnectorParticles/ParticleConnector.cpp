//
//  ParticleConnector.cpp
//  emptyExample
//
//  Created by James George on 5/29/13.
//


#include "ParticleConnector.h"

ParticleConnector::ParticleConnector(ofVec3f pos)
	: position(pos)
{
	direction = ofVec3f(1,0,0).getRotated(ofRandom(360), ofVec3f(0,0,1)) * 5;

}

ParticleConnector::~ParticleConnector(){
	
}

void ParticleConnector::update(){
	position.set(fmod(640 + position.x + direction.x, 640),
				 fmod(480 + position.y + direction.y, 480));

}

