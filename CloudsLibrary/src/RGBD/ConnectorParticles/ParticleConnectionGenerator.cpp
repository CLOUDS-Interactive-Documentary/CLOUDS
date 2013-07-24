//
//  ParticleConnectionGenerator.cpp
//  emptyExample
//
//  Created by James George on 5/29/13.
//
//

#include "ParticleConnectionGenerator.h"

ParticleConnectionGenerator::ParticleConnectionGenerator(){
	minDistance = 50;
	numParticles = 100;
	searchStartIdx = 0;
	searchStepSize = 1;
}

void ParticleConnectionGenerator::setup(){
	
	for(int i = 0; i < numParticles; i++){
		
		pointMesh.addVertex( ofVec3f(ofRandom(640), ofRandom(480), 0) );
		connectors.push_back(new ParticleConnector( pointMesh.getVertices()[i] ));
		
	}
}

void ParticleConnectionGenerator::setMinDistance(int distance){
	for(int i = 0; i < connections.size(); i++){
		connections[i].minDistance = distance;
	}
	minDistance = distance;
}

void ParticleConnectionGenerator::update(){
	
	for(int i = 0; i < connectors.size(); i++){
		connectors[i]->update();
	}
	
	searchStartIdx = (searchStartIdx + 1) % searchStepSize;
	float squaredDistance = pow(minDistance,2);
	for(int i = 0; i < connectors.size(); i++){
		for(int j = searchStartIdx; j < connectors.size(); j+= searchStepSize){
			if(i != j &&
			  (connectors[i]->connections.find(connectors[j]) == connectors[i]->connections.end()  ||
			   connectors[j]->connections.find(connectors[i]) == connectors[j]->connections.end()) &&
			   connectors[i]->position.distanceSquared(connectors[j]->position) < squaredDistance)
			{
				ParticleConnection connection;
				
				connection.a = connectors[i];
				connection.b = connectors[j];
				connection.steps = 2;
				connection.minDistance = minDistance;
				connection.createConnections();
				connection.a->connections.insert(connection.b);
				connection.b->connections.insert(connection.a);
				
				connections.push_back(connection);
			}
		}
	}
	
	for(int i = connections.size()-1; i >= 0; i--){
		if(connections[i].currentColor < 0){
			
			set<ParticleConnector*>& setA = connections[i].a->connections;
			set<ParticleConnector*>& setB = connections[i].b->connections;
			
			setA.erase( setA.find( connections[i].b) );
			setB.erase( setB.find( connections[i].a) );
			
			connections.erase(connections.begin() + i);
		}
		else{
			connections[i].updateConnections();
		}
	}
}

void ParticleConnectionGenerator::draw(){
	
	ofDrawBitmapString("fps:" + ofToString(ofGetFrameRate()), ofPoint(10,10));
	
	ofPushStyle();
	ofNoFill();
	
	for(int i = 0; i < connectors.size(); i++){
		pointMesh.setVertex(i, connectors[i]->position);
		
	}
	pointMesh.drawVertices();
	
	for(int i = 0; i < connections.size(); i++){
		connections[i].connectionLines.draw();
	}
}
