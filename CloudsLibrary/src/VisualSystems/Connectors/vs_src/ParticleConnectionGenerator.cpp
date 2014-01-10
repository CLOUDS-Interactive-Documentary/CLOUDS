//
//  ParticleConnectionGenerator.cpp
//  emptyExample
//
//  Created by James George on 5/29/13.
//
//

#include "ParticleConnectionGenerator.h"
#include "ParticleConnectorBin.h"

ParticleConnectionGenerator::ParticleConnectionGenerator(){
	lastMinDistance = 0;
	minDistance = 50;
	currentBinSize = minDistance*2;
	numParticles = 100;
	searchStartIdx = 0;
	searchStepSize = 1;
	drawConnections = true;
	lastBoundarySize = 0;
	boundarySize = 500;
	bBinMeshDirty = false;
	maxTotalConnections = 40000;
    connectorsSpeed = 1;
}

void ParticleConnectionGenerator::setup(){
	
	
	for(int i = 0; i < maxTotalConnections; i++){
		connectionMesh.addColor(ofFloatColor(0));
		connectionMesh.addVertex(ofVec3f(0,0,0));
		connectionMesh.addColor(ofFloatColor(0));
		connectionMesh.addVertex(ofVec3f(0,0,0));
		freeConnectionIndeces.insert(i);
	}
	connectionMesh.setMode(OF_PRIMITIVE_LINES);
}

void ParticleConnectionGenerator::update(){
	
	checkParticleCounts();
	
	updateConnectors();
	
	if(!drawConnections) return;
	
	updateConnections();
}

void ParticleConnectionGenerator::checkParticleCounts(){
	numParticles = floor(numParticles);
	while(connectors.size() > numParticles){
		
		int deleteIndex = connectors.size() - 1;
		while (deleteIndex >= 0 && connectors[deleteIndex]->connectingLines.size() != 0) {
			deleteIndex--;
		}
		
		if(deleteIndex <= 0) break;
		
		delete connectors[deleteIndex];
		connectors.erase( connectors.begin() + deleteIndex);
		
		pointMesh.getVertices().erase( pointMesh.getVertices().begin() + pointMesh.getVertices().size()-1 );
	}
	
	while(connectors.size() < numParticles){
		
		pointMesh.addVertex(ofVec3f(0,0,0));
		ParticleConnector* connector = new ParticleConnector() ;
		lastBoundarySize = 0;
		connectors.push_back(connector);
	}
	
}

void ParticleConnectionGenerator::updateConnectors(){
	if(lastMinDistance  != minDistance ||
	   lastBoundarySize != boundarySize)
	{
		setBounds(boundarySize, minDistance);
		
		lastMinDistance = minDistance;
		lastBoundarySize = boundarySize;
		
		ofVec3f boundaryPoint = ofVec3f(boundarySize,boundarySize,boundarySize);
		for(int i = 0; i < connectors.size(); i++){
			connectors[i]->upperBounds =  boundaryPoint;
			connectors[i]->lowerBounds = -boundaryPoint;
		}
		for(int i = 0; i < connections.size(); i++){
			connections[i]->minDistance = minDistance;
		}
	}
	
	for(int i = 0; i < connectors.size(); i++){
		connectors[i]->update(connectorsSpeed);
		connectors[i]->binIndex1 = positionToBinIndex(connectors[i]->position);
		connectors[i]->binIndex2 = positionToBinIndex(connectors[i]->position);
	}
	
}

void ParticleConnectionGenerator::updateConnections(){
	
	
	searchStartIdx = (searchStartIdx + 1) % searchStepSize;
	float squaredDistance = pow(minDistance,2);
	
	for(int i = 0; i < connectors.size(); i++){
		int numConnects = 0;
		for(int j = searchStartIdx; j < connectors.size(); j+= searchStepSize){

			if(i != j &&
			   (connectors[i]->binIndex1 == connectors[j]->binIndex1 || connectors[i]->binIndex2 == connectors[j]->binIndex2) &&
			   connectors[i]->connectingParticles.find(connectors[j]) == connectors[i]->connectingParticles.end() &&
			   connectors[j]->connectingParticles.find(connectors[i]) == connectors[j]->connectingParticles.end() &&
			   connectors[i]->position.distanceSquared(connectors[j]->position) < squaredDistance &&
			   !freeConnectionIndeces.empty())
			{
				ParticleConnection* connection = new ParticleConnection();
				
				connection->a = connectors[i];
				connection->b = connectors[j];
				connection->connectionMeshRef = &connectionMesh;
				connection->connectionVboIndex = *freeConnectionIndeces.begin();
				freeConnectionIndeces.erase(freeConnectionIndeces.begin());
				
				connection->minDistance = minDistance;
				connection->createConnections();
				connection->a->connectingParticles.insert(connection->b);
				connection->b->connectingParticles.insert(connection->a);
				
				connections.push_back(connection);
				connection->a->connectingLines.insert( connections[connections.size()-1] );
				connection->b->connectingLines.insert( connections[connections.size()-1] );
				
			}
		}
	}
	
	for(int i = connections.size()-1; i >= 0; i--){
		
		connections[i]->updateConnections();
		
		if(connections[i]->dead){
			deleteConnection(connections.begin() + i);
		}
	}

}

void ParticleConnectionGenerator::deleteConnection(vector<ParticleConnection*>::iterator it){

	ParticleConnection* connection = *it;

	set<ParticleConnector*>& setA = connection->a->connectingParticles;
	set<ParticleConnector*>& setB = connection->b->connectingParticles;

	setA.erase( setA.find( connection->b) );
	setB.erase( setB.find( connection->a) );

	connection->a->connectingLines.erase( connection->a->connectingLines.find(connection) );
	connection->b->connectingLines.erase( connection->b->connectingLines.find(connection) );

	//reclaim connection
	freeConnectionIndeces.insert( connection->connectionVboIndex );
	delete connection;
	connections.erase(it);
}

void ParticleConnectionGenerator::setBounds(float bounds, float minDist){
	currentBinSize = minDist * 2;
	currentSubdivisions = ceil(bounds*2 / currentBinSize);
	
	//minDistance = bounds / currentBinSize / 2.0;
	
	bBinMeshDirty = true;
	//boundarySize = currentSubdivisions * currentBinSize / 2.;
}

int ParticleConnectionGenerator::positionToBinIndex(ofVec3f pos){

	ofVec3f quantized = (pos - ofVec3f(boundarySize, boundarySize, boundarySize)) / currentBinSize;
	return floor(quantized.x) +
		   floor(quantized.y) * currentSubdivisions +
		   floor(quantized.z) * currentSubdivisions * currentSubdivisions;
}

void ParticleConnectionGenerator::drawBins(){

	if(bBinMeshDirty){
		binMesh.clear();
		binMesh.setMode(OF_PRIMITIVE_LINES);
		
		//layer one
		ofVec3f lowerBounds(-boundarySize,-boundarySize,-boundarySize);
		
		for(int z = 0; z < currentSubdivisions; z++){
			for(int y = 0; y < currentSubdivisions; y++){
				for(int x = 0; x < currentSubdivisions; x++){
					ofVec3f binMin = lowerBounds + currentBinSize * ofVec3f(x,y,z);
					
					binMesh.addVertex( binMin );
					binMesh.addVertex( binMin + ofVec3f(currentBinSize,0,0) );
					
					binMesh.addVertex( binMin );
					binMesh.addVertex( binMin + ofVec3f(0,currentBinSize,0) );
					
					binMesh.addVertex( binMin );
					binMesh.addVertex( binMin + ofVec3f(0,0,currentBinSize) );

				}
			}
		}
		
		for(int z = 0; z <= currentSubdivisions; z++){
			for(int y = 0; y <= currentSubdivisions; y++){
				for(int x = 0; x <= currentSubdivisions; x++){
					ofVec3f binMin = lowerBounds + currentBinSize * ofVec3f(x,y,z) - currentBinSize/2;
					
					binMesh.addVertex( binMin );
					binMesh.addVertex( binMin + ofVec3f(currentBinSize,0,0) );

					binMesh.addVertex( binMin );
					binMesh.addVertex( binMin + ofVec3f(0,currentBinSize,0) );
					
					binMesh.addVertex( binMin );
					binMesh.addVertex( binMin + ofVec3f(0,0,currentBinSize) );
					
				}
			}
		}
		bBinMeshDirty = false;
	}

	ofPushStyle();
	
	binMesh.draw();

	ofPopStyle();
}

void ParticleConnectionGenerator::draw(){
	
	ofPushStyle();
//	ofNoFill();
	
	for(int i = 0; i < connectors.size(); i++){
		pointMesh.setVertex(i, connectors[i]->position);
	}
	
	pointMesh.drawVertices();
	connectionMesh.draw();
	
	ofPopStyle();
}

