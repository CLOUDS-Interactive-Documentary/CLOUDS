
#pragma once

#include "ofMain.h"

#include "State.h"
#include "ForceSet.h"
#include "Points.h"
#include "GroundPlane.h"

namespace GPUParticles {
	
	class Controller {
  	public:
		Controller();
		
		void setup();
		void update();
		void draw();
		
		void reset();
		
		void drawDebug();
		
	protected:
		
//		void windowResized(ofEventArgs& args);
		
		ofShader null;
		State state;
		ForceSet forceSet;
		ofShader gravity;
		Points points;
		
		ofFloatImage random;
		
		ofFbo fromAbove;
		ofFbo multiSample;
		
		ofVec3f birthPlace;
		float birthSpread;
		float birthRate;
		
		bool updating;
	};
}
