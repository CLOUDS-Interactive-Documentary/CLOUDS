
#pragma once

#include "ofMain.h"
#include "ParticleConnector.h"

class ParticleConnection {
  public:
	ParticleConnection();
	
	void createConnections();
	void updateConnections();
	
	float currentColor;
	int steps;
	int createFrame;
	int minDistance;
	bool dead;
	ParticleConnector* a;
	ParticleConnector* b;
	ofVboMesh* connectionMeshRef;
	
	int connectionVboIndex;
};

