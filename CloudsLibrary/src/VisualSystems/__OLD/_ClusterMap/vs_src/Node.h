
#pragma once

#include "ofMain.h"
#include "ofxTLColorTrack.h"

static ofVec3f ofRandomPointOnUnitSphere(){
	float phi = 2*PI*ofRandomuf();
	float theta = acos(ofRandomf());
	return ofVec3f(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
}

class Node {
  public:
	
	Node();
	
	void replicate(vector<Node*>& allNodes, vector<ofVec3f>& fusePoints, vector<ofVec3f>& fuseDistances);
	void terminate();
	void fade(int maxEnergy, bool leaf);
	void finalize();
	
	int startEnergy;
	int endEnergy;
	
	ofIndexType startIndex;
	ofIndexType endIndex;
	ofxTLColorTrack* nodeColorTrack;
	ofVboMesh* mesh;
	
	ofIndexType nodePointIndex;
	ofFloatColor lineColor;
	ofFloatColor nodeColor;
	bool targeted;
	int sourceId;
	bool replicated;
	int numIterations;
	Node* parent;
	ofVec3f direction;
	ofVec3f position;
	bool terminated;
	bool leaf;
	int numConnections;
	
	float actualDistance;
	float replicatePointSize;
	
	int numPointsAtReplicate;
	float replicatePointDistance;

	int numReplications;
	float minDistance;
	float distanceRange;
	float stepSize;
	int generation;
	
	int numSurvivingBranches;
	float minAttractRadius;
	float minRepelRadius;
	float minFuseRadius;

	float maxAttractForce;
	float maxRepelForce;
};