
#include "ParticleConnection.h"

ParticleConnection::ParticleConnection(){
	createFrame = 0;
	a = b = NULL;
}

void ParticleConnection::createConnections(){
	
	if(a == NULL || b == NULL){
		return;
	}
	currentColor = 0;
	connectionLines.clear();
	for(float i = 0; i <= 1.0; i += 1.0/steps){
		//connectionLines.addColor( ofFloatColor(ofMap( abs(i - .5), 0, .5, 0, 1.0)) );
		connectionLines.addColor( ofFloatColor(0) );		
		connectionLines.addVertex( a->position.getInterpolated(b->position, i) );
	}
	
	createFrame = ofGetFrameNum();
	
	connectionLines.setMode(OF_PRIMITIVE_LINE_STRIP);
}

void ParticleConnection::updateConnections(){
	
	currentColor = ofMap(a->position.distanceSquared( b->position ), 0, powf(minDistance,2.), 1.0, 0);
	
	for(int i = 0; i < connectionLines.getNumVertices(); i++){

		connectionLines.getColors()[i].set( ofFloatColor(currentColor) );
		connectionLines.getVertices()[i].set( a->position.getInterpolated(b->position, 1.0 * i / (connectionLines.getNumVertices() - 1)) );
	}
	
}

