//
//  State.h
//
//
#pragma once
#include "ofMain.h"

namespace GPUParticles {
	//--------
	class State {
	public:
		void init(int particleCount);
		void clear();
		void update();
		
		int getWidth() { return this->resolution; }
		int getHeight() { return this->resolution; }
		
		ofFbo & getSourcePosition();
		ofFbo & getSourceVelocity();
		ofFbo & getTargetPosition();
		ofFbo & getTargetVelocity();
		
		void loadShader();
		void drawQuad();
		
	protected:
		ofFbo sourcePosition, sourceVelocity;
		ofFbo targetPosition, targetVelocity;
		
		int particleCount;
		int resolution;
		
		ofShader null;
		ofShader integrate;
		ofMesh quad;
	};
}