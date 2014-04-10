//
//  VoxelMesh.h
//  emptyExample
//
//  Created by James George on 4/10/14.
//
//

#pragma once

#include "ofMain.h"

class VoxelMesh {
  public:
	VoxelMesh();
		
	
	void setup();
	void update();
	void draw();

	void reloadShaders();	
	void regenerateGeometry();

	int numVoxels;
	int voxelWidth;

  protected:

	ofVbo pointVbo;
	ofVbo lineVbo;
	int nPointIndices;
	int nLineIndices;

	ofShader pointShader;
	ofShader lineShader;
};