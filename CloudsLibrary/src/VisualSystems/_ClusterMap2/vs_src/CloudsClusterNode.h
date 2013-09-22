
#pragma once

#include "ofMain.h"

//class for showing a dot in the constellation of the ClusterMap

class CloudsClusterNode {
  public:
	CloudsClusterNode();

	string clipId;
	vector<string> adjascentClipIds;
	
	ofVboMesh* mesh;
	int vertexIndex;
	bool visible;
	bool traversed;
	
	//pushes the variables into the vertex settings on the mesh
	void updateVertexAttributes();
};
