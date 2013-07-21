
#pragma once

#include "ofMain.h"

#include "State.h"
#include "ForceSet.h"
#include "Points.h"
//#include "GroundPlane.h"

namespace GPUParticles {
	
	class Controller {
  	public:
		Controller();
		
		void setParticleCount(int numParticles);
		void setup();
		void update();
		void draw();
		
		void reloadShaders();
		
		void reset();
		
		void drawDebug();
		
	protected:
		
		ofShader null;
		State state;
		ForceSet forceSet;
		ofShader gravity;
		Points points;

		int numParticles;
		
		ofFloatImage random;
		
		ofFbo fromAbove;
		ofFbo multiSample;
		
		ofVec3f birthPlace;
		float birthSpread;
		float birthRate;
		
		bool updating;
	};
}
