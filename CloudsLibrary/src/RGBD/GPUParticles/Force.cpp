//
//  Force
//
#include "Force.h"

namespace GPUParticles {
	//--------
	Force::Force(string name, string path) {
		this->name = name;
		this->path = path;
		load();
	}

	//--------
	void Force::load() {
		this->shader.load(path + "default.vert", path + "forces/" + this->name + ".frag");
	}

	//--------
	void Force::step(State& state) {
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