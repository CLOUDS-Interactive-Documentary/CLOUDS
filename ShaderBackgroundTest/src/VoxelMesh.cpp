//
//  VoxelMesh.cpp
//  emptyExample
//
//  Created by James George on 4/10/14.
//
//

#include "VoxelMesh.h"

VoxelMesh::VoxelMesh(){
	numVoxels = 10;
	voxelWidth = 100;
	
}

void VoxelMesh::setup(){
	
	regenerateGeometry();
	reloadShaders();
}

void VoxelMesh::update(){
	
}

void VoxelMesh::draw(){

	//pointVbo.draw(GL_POINTS, 0, nPointIndices);
	lineVbo.draw(GL_LINES, 0, nLineIndices);
}

void VoxelMesh::reloadShaders(){
	pointShader.load("shaders/pointShader");
	lineShader.load("shaders/lineShader");
}

void VoxelMesh::regenerateGeometry(){
	
	ofMesh pointMesh;
	
	float center = numVoxels*voxelWidth/2;
	//genreate point grid
	for(int z = 0; z < numVoxels; z++){
		for(int y = 0; y < numVoxels; y++){
			for(int x = 0; x < numVoxels; x++){
				pointMesh.addVertex(ofVec3f(x*voxelWidth - center,
											y*voxelWidth - center,
											z*voxelWidth - center));
			}
		}
	}

	pointVbo.clear();
	pointVbo.setMesh(pointMesh, GL_STATIC_DRAW);
	nPointIndices = pointMesh.getNumVertices();

	ofMesh lineMesh;

	for(int z = 0; z < numVoxels; z++){
		for(int y = 0; y < numVoxels; y++){
			for(int x = 0; x < numVoxels; x++){
				ofVec3f baseVert = ofVec3f(x*voxelWidth - center,
										   y*voxelWidth - center,
										   z*voxelWidth - center);
				ofVec3f rightVert = baseVert + ofVec3f(voxelWidth,0,0);
				ofVec3f downVert  = baseVert + ofVec3f(0,voxelWidth,0);
				ofVec3f backVert  = baseVert + ofVec3f(0,0,voxelWidth);

				if(x != numVoxels - 1){
					
					lineMesh.addVertex(baseVert);
					lineMesh.addNormal(rightVert);
					
					lineMesh.addVertex(rightVert);
					lineMesh.addNormal(baseVert);
				}
				
				if(y != numVoxels - 1){
					lineMesh.addVertex(baseVert);
					lineMesh.addNormal(downVert);

					lineMesh.addVertex(downVert);
					lineMesh.addNormal(baseVert);
				}
				
				if(z != numVoxels - 1){
					lineMesh.addVertex(baseVert);
					lineMesh.addNormal(backVert);
					
					lineMesh.addVertex(backVert);
					lineMesh.addNormal(baseVert);
				}
			}
		}
	}
	
	lineVbo.clear();
	lineVbo.setMesh(lineMesh, GL_STATIC_DRAW);
	nLineIndices = lineMesh.getNumVertices();
}
