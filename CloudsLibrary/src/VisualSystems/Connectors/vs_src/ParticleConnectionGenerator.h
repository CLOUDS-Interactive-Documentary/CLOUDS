
#pragma once

#include "ofMain.h"
#include "ParticleConnection.h"
#include "ParticleConnector.h"

class ParticleConnectorBin;
class ParticleConnectionGenerator {
  public:
	ParticleConnectionGenerator();

	bool drawConnections;
	float numParticles;
	int maxTotalConnections;
	float maxConnections;
	
	float minDistance;
	float boundarySize;
	
	void setup();
	void update();
	void draw();

	void drawBins();
	
  protected:
	ofMesh pointMesh;
	float lastMinDistance;
	float lastBoundarySize;
	float currentBinSize;
	
	ofVboMesh binMesh;

	ofVboMesh connectionMesh;
	set<int> freeConnectionIndeces;
	void deleteConnection(vector<ParticleConnection*>::iterator it);
	
	void setBounds(float bounds, float minDistance);
	void checkParticleCounts();
	void updateConnectors();
	void updateConnections();
	
	int currentSubdivisions;
	vector<ParticleConnector*> connectors;
	vector<ParticleConnection*> connections;
	
	bool bBinMeshDirty;
	int positionToBinIndex(ofVec3f pos);
	
	int searchStartIdx;
	int searchStepSize;
};