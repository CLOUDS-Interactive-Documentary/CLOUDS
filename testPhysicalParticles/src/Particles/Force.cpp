//
//  Force
//
#include "Force.h"

namespace Particles {
	//--------
	Force::Force(string name) {
		this->name = name;
		load();
	}

	//--------
	void Force::load() {
		this->shader.load("default.vert", "forces/" + this->name + ".frag");
	}

	//--------
	void Force::step(Particles::State & state) {
		this->shader.begin();
		this->shader.setUniformTexture("position", state.getSourcePosition().getTextureReference(), 0);
		this->shader.setUniformTexture("velocity", state.getSourceVelocity().getTextureReference(), 1);
		this->shader.setUniform1f("dt", ofGetLastFrameTime());
		this->shader.setUniform1f("t", ofGetElapsedTimef());
		
		//draw a quad with the right texture coords
		state.drawQuad();
		
		this->shader.end();
	}
}