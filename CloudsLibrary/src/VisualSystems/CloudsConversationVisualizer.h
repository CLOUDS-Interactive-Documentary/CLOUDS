//
//  CloudsConversationVisualizer.h
//  ConversationNetwork
//
//  Created by James George on 4/17/13.
//
//

#pragma once

#include "ofMain.h"
#include "CloudsFCPParser.h"
#include "MSAPhysics2D.h"

using namespace msa::physics;

class CloudsConversationVisualizer {
  public:
	CloudsConversationVisualizer();
	~CloudsConversationVisualizer();
	
	void setup(CloudsFCPParser& parser);
	void update();
	void draw();
	
	float springStrength;
	float springRestLength;
	float repulsionForce;

  protected:
	
	CloudsFCPParser* parser;
	msa::physics::World2D physics;
	map<string, Particle2D*> nameToParticle;
	map<Particle2D*, string> particleToName;
	map<pair<string, string>, Spring2D*> namesToSpring;

	ofRectangle totalRectangle;
	float currentScale;
	ofVec2f currentTop;

};