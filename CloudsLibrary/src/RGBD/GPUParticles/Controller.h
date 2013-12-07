
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
		
		void setShaderDirectory(string directory);
		void setParticleCount(int numParticles);
		void setup();
		void update();
		void draw();
		
		void reloadShaders();
		
		void reset();
		
		void drawDebug();
		
		ofVec3f birthPlace;
		float birthSpread;
		float birthRate;
		
		Points& getPoints();
		
	protected:
		
		string shaderDirectory;
		
		ofShader null;
		State state;
		ForceSet forceSet;
		ofShader gravity;
		Points points;

		int numParticles;
		
		ofFloatImage random;
		
		ofFbo fromAbove;
		ofFbo multiSample;
		
		
		bool updating;
	};
}
