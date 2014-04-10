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
	noisePosition = 0;
}

void VoxelMesh::setup(){
	
	regenerateGeometry();
	reloadShaders();
}

void VoxelMesh::update(){
	if(currentNumVoxels != numVoxels ||
	   currentVoxelWidth != voxelWidth)
	{
		regenerateGeometry();
	}
	noisePosition += noiseSpeed;
}

void VoxelMesh::draw(){

	glDisable(GL_DEPTH_TEST);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	lineShader.begin();
	lineShader.setUniform1f("sphereRadius", sphereRadius);
	lineShader.setUniform1f("spherePercent", spherePercent);
	
	lineShader.setUniform1f("minDistance", minDistance*voxelWidth);
	lineShader.setUniform1f("maxDistance", maxDistance*voxelWidth);
	
	lineShader.setUniform1f("twistFactor", twistFactor+noisePosition*.001);

	lineShader.setUniform1f("noiseDistort", powf(noiseDistort, 2.0f));
	lineShader.setUniform1f("noiseDensity", powf(noiseDensity, 2.0f));
	lineShader.setUniform1f("noisePosition", noisePosition);

	lineVbo.draw(GL_LINES, 0, nLineIndices);
	lineShader.end();
	
	//pointVbo.draw(GL_POINTS, 0, nPointIndices);
	
//	ofPushStyle();
//	ofNoFill();
//	ofSetColor(0,0,255,100);
//	ofSphere(0,0,0,300);
//	ofPopStyle();
}

void VoxelMesh::reloadShaders(){
//	pointShader.load("shaders/pointShader");
	lineShader.load("shaders/voxel_lines");
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
	
	currentNumVoxels = numVoxels;
	currentVoxelWidth = voxelWidth;

}
