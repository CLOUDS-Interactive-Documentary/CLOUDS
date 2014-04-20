
#pragma once

#include "ofMain.h"
#include <set>

class ParticleConnection;
class ParticleConnector {
  public:
	ParticleConnector();
	~ParticleConnector();

	void update(float speed);
	ofVec3f position;
	ofVec3f direction;
    float speed;
	
	set<ParticleConnector*> connectingParticles;
	set<ParticleConnection*> connectingLines;
	
	int binIndex1;
	int binIndex2;
	
	ofVec3f lowerBounds;
	ofVec3f upperBounds;
  protected:
	
};