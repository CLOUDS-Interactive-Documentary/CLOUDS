
#pragma once

#include "ofMain.h"
#include "ParticleConnection.h"
#include "ParticleConnector.h"

class ParticleConnectionGenerator {
  public:
	ParticleConnectionGenerator();

	int	numParticles;

	void setup();
	void update();
	void draw();

	void setMinDistance(int distance);

  protected:
	ofMesh pointMesh;

	float minDistance;
	vector<ParticleConnector*> connectors;
	vector<ParticleConnection> connections;

	int searchStartIdx;
	int searchStepSize;
};