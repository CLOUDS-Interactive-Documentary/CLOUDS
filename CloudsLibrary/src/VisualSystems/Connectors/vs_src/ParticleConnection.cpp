
#include "ParticleConnection.h"

ParticleConnection::ParticleConnection(){
	createFrame = 0;
	a = b = NULL;
	dead = false;
}

void ParticleConnection::createConnections(){
	
	if(a == NULL || b == NULL){
		return;
	}
	currentColor = 0;
	createFrame = ofGetFrameNum();

}

void ParticleConnection::updateConnections(){
	
	
	currentColor = ofMap(a->position.distanceSquared( b->position ), 0, powf(minDistance,2.), 1.0, 0);
	
	ofFloatColor color =  ofFloatColor::azure.getLerped(ofFloatColor::azure, currentColor);
	color.a = currentColor;

	connectionMeshRef->setColor(connectionVboIndex*2, ofFloatColor(currentColor, currentColor));
//	connectionMeshRef->setColor(connectionVboIndex*2, color);
	connectionMeshRef->setVertex(connectionVboIndex*2, a->position);

	connectionMeshRef->setColor(connectionVboIndex*2+1, ofFloatColor(currentColor,currentColor));
//	connectionMeshRef->setColor(connectionVboIndex*2+1, color);
	connectionMeshRef->setVertex(connectionVboIndex*2+1, b->position);

	if(currentColor < 0){
		dead = true;
	}
}

