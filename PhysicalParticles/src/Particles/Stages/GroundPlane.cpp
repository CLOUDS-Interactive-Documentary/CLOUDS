//
//  GroundPlane.cpp
//

#include "GroundPlane.h"

namespace Particles {
	void GroundPlane::step(Particles::State &state) {
		this->shader.begin();
		this->shader.setUniformTexture("position", state.getSourcePosition().getTextureReference(), 0);
		this->shader.setUniformTexture("velocity", state.getSourceVelocity().getTextureReference(), 1);
		this->shader.setUniform1f("dt", ofGetLastFrameTime());
		this->shader.setUniform1f("t", ofGetElapsedTimef());
		
		state.getTargetPosition().begin();
		//draw a quad with the right texture coords
		//state.drawQuad();
		state.getTargetPosition().end();
		
		this->shader.end();
	}
	
	void GroundPlane::load() {
		this->shader.load("default.vert","groundplane.frag");
	}
}