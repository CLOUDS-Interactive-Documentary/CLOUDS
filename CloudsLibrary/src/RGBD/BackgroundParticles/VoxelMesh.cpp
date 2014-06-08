//
//  VoxelMesh.cpp
//  emptyExample
//
//  Created by James George on 4/10/14.
//
//

#include "VoxelMesh.h"
#include "CloudsGlobal.h"

VoxelMesh::VoxelMesh(){
	setDefaults();
}

void VoxelMesh::setDefaults(){
	
	numVoxels = 10;
	voxelWidth = 100;
	noisePosition = 0;
	twistPositionX = 0;
	twistPositionY = 0;
	drawPoints = false;
	drawLines = false;
	
	shouldRegenerate = false;
	
	numVoxels = 10;
	voxelWidth = 100;
	
	sphereRadius = 100;
	spherePercent = 0.0;
	
	minDistance = 0;
	maxDistance = 10;
	
	twistFactorY = 0;
	twistSpeedY = 0;
	
	twistFactorX = 0;
	twistSpeedX = 0;
	
	spiralFactor = 0;
	
	noiseDistort = ofVec4f();
	noiseDensity = 0;
	noiseSpeed = 0;
	
	centerDecayMinRadius = 0;
	centerDecayMaxRadius = 0;
	center = ofVec3f(0,0,0);
	
    pointSize = 1.0;
    lineWidth = 1.0;

	drawPoints = false;
	drawLines = false;
}

void VoxelMesh::addGui(ofxUISuperCanvas* gui){
	
	gui->addToggle("draw points", &drawPoints);
	gui->addToggle("draw lines", &drawLines);
	
	gui->addIntSlider("num voxels", 10, 100, &numVoxels);
	gui->addSlider("voxel spacing", 10, 100, &voxelWidth);
	
	gui->addSlider("sphere radius",  100, 800, &sphereRadius);
	gui->addSlider("sphere percent", -2.0, 2.0, &spherePercent);
	
	gui->addRangeSlider("distance range", 0.0, 2.0, &minDistance, &maxDistance);
	
	gui->addSlider("twist speed x",  0, .001, &twistSpeedX);
	gui->addSlider("twist factor x", 0, 1.0, &twistFactorX);
	gui->addSlider("twist speed y",  0, .001, &twistSpeedY);
	gui->addSlider("twist factor y", 0, 1.0, &twistFactorY);
	
	gui->addSlider("noise distort x", 0, 1.0, &noiseDistort.x);
	gui->addSlider("noise distort y", 0, 1.0, &noiseDistort.y);
	gui->addSlider("noise distort z", 0, 1.0, &noiseDistort.z);
	gui->addSlider("noise distort radial", 0, 1.0, &noiseDistort.w);
	gui->addSlider("noise density", 0, 0.2, &noiseDensity);
	gui->addRangeSlider("center decay", 0, 1.0, &centerDecayMinRadius, &centerDecayMaxRadius);
	
	gui->addSlider("noise speed", 0, 0.1, &noiseSpeed);
	

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
	
	noisePosition  += noiseSpeed;
	twistPositionX += twistSpeedX;
	twistPositionY += twistSpeedY;
}

void VoxelMesh::draw(){

	ofPushMatrix();
	ofPushStyle();
	ofTranslate(center);
	
	glDisable(GL_DEPTH_TEST);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	if(drawLines){
        ofSetLineWidth(lineWidth);
        
		lineShader.begin();
		lineShader.setUniform1f("sphereRadius", sphereRadius);
		lineShader.setUniform1f("spherePercent", spherePercent);
		
		lineShader.setUniform1f("minDistance", minDistance*voxelWidth);
		lineShader.setUniform1f("maxDistance", maxDistance*voxelWidth);
		
		lineShader.setUniform1f("twistFactorX", twistPositionX*twistFactorX);
		lineShader.setUniform1f("twistFactorY", twistPositionY*twistFactorY);

		lineShader.setUniform4f("noiseDistort",
								powf(noiseDistort.x, 2.0f),
								powf(noiseDistort.y, 2.0f),
								powf(noiseDistort.z, 2.0f),
								powf(noiseDistort.w, 2.0f));
		
		lineShader.setUniform1f("noiseDensity", powf(noiseDensity, 2.0f));
		lineShader.setUniform1f("noisePosition", noisePosition);

		lineShader.setUniform1f("centerDecayMinRadius", centerDecayMinRadius*currentVoxelWidth*numVoxels*.5);
		lineShader.setUniform1f("centerDecayMaxRadius", centerDecayMaxRadius*currentVoxelWidth*numVoxels*.5);

		lineVbo.draw(GL_LINES, 0, nLineIndices);
		lineShader.end();
	}
	
	if(drawPoints){
        
        glPointSize(pointSize);
        
		pointShader.begin();
		pointShader.setUniform1f("sphereRadius",  sphereRadius);
		pointShader.setUniform1f("spherePercent", spherePercent);
		
		pointShader.setUniform1f("minDistance", minDistance*voxelWidth);
		pointShader.setUniform1f("maxDistance", maxDistance*voxelWidth);
		
		pointShader.setUniform1f("twistFactorX", twistPositionX*twistFactorX);
		pointShader.setUniform1f("twistFactorY", twistPositionY*twistFactorY);
		
		pointShader.setUniform4f("noiseDistort",
								powf(noiseDistort.x, 2.0f),
								powf(noiseDistort.y, 2.0f),
								powf(noiseDistort.z, 2.0f),
								powf(noiseDistort.w, 2.0f));
		pointShader.setUniform1f("noiseDensity", powf(noiseDensity, 2.0f));
		
		pointShader.setUniform1f("noisePosition", noisePosition);
		
		lineShader.setUniform1f("centerDecayMinRadius", centerDecayMinRadius*currentVoxelWidth*numVoxels*.5);
		lineShader.setUniform1f("centerDecayMaxRadius", centerDecayMaxRadius*currentVoxelWidth*numVoxels*.5);

		pointVbo.draw(GL_POINTS, 0, nPointIndices);
		
		pointShader.end();
	}
	ofDisableAlphaBlending();
	ofPopStyle();
	ofPopMatrix();
}

void VoxelMesh::reloadShaders(){
	pointShader.load(GetCloudsDataPath() +  "shaders/background/voxel_points");
	lineShader.load(GetCloudsDataPath() +  "shaders/background/voxel_lines");
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
