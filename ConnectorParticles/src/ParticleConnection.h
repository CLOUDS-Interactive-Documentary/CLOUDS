
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
	
	ParticleConnector* a;
	ParticleConnector* b;
	
	ofMesh connectionLines;
};

