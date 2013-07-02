//
//  ForceSet.cpp
//

#include "ForceSet.h"

namespace GPUParticles {
	//---------
	void ForceSet::load() {		
		this->clear();
		
		ofDirectory dir;
		dir.listDir("forces");
		dir.sort();
		for (int i=0; i<dir.size(); i++) {
			string name = dir.getFile(i).getBaseName();
			if (name[0] == '~')
				continue;
			this->add(name);
			ofLogNotice() << "Loaded force '" << name << "'";
		}
	}
	
	//---------
	void ForceSet::add(string name) {
		this->add(new Force(name));
	}
	
	//---------
	void ForceSet::add(BaseForce* force) {
		forces.push_back(force);
	}
	
	//---------
	void ForceSet::clear() {
		for (ForceIterator it = this->forces.begin(); it != this->forces.end(); it++) {
			delete *it;
		}
		this->forces.clear();
	}
	
	//---------
	void ForceSet::update(State & state) {
		glPushAttrib(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);
		state.getSourceVelocity().draw(0, 0);
		for (ForceIterator it = this->forces.begin(); it != this->forces.end(); it++) {
			state.getTargetVelocity().begin();
			(*it)->step(state);
			state.getTargetVelocity().end();
			state.update();
		}
		glPopAttrib();
	}
}