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
		void init(int particleCount, string path);
		void clear();
		void update();
		
		int getWidth() { return this->resolution; }
		int getHeight() { return this->resolution; }
		
		ofFbo & getSourcePosition();
		ofFbo & getSourceVelocity();
		ofFbo & getTargetPosition();
		ofFbo & getTargetVelocity();
		
		string getPath(){ return shaderPath; }
		
		void loadShader();
		void drawQuad();
		
	protected:
		ofFbo sourcePosition, sourceVelocity;
		ofFbo targetPosition, targetVelocity;
		string shaderPath;
		
		int particleCount;
		int resolution;
		
		ofShader null;
		ofShader integrate;
		ofMesh quad;
	};
}