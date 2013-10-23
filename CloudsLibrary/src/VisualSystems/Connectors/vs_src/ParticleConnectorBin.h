//
//  ParticleConnectorBin.h
//  Connectors
//
//  Created by James George on 7/30/13.
//
//

#pragma once

#include "ofMain.h"

class ParticleConnector;
class ParticleConnectorBin {
  public:
	ParticleConnectorBin();

	void setup();
	vector<ParticleConnector*> members;
	
	ofVec3f minBound;
	ofVec3f maxBound;
	ofVec3f size;
};
