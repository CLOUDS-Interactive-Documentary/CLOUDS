
#pragma once

#include "ofMain.h"

//class for showing a dot in the constellation of the ClusterMap
typedef struct {
	string destinationClipId;
	ofIndexType startIndx;
	ofIndexType endIndx;
} TraversalCurve;

class CloudsClusterNode {
  public:
	CloudsClusterNode();

	ofVec3f networkPosition;
	
	string clipId;
	//clip ID's that are connected to us
	vector<string> adjascentClipIds;

	//indeces of edge ends that we connect to
	vector<ofIndexType> connectionMeshVertexIds;
	map<string, TraversalCurve> connectionCurves;
	ofVboMesh* mesh;
	
	int vertexIndex;
	bool visible;
	bool traversed;
	
	//pushes the variables into the vertex settings on the mesh
	void updateVertexAttributes();
};
