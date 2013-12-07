//
//  ForceSet.h
//
#pragma once

#include "Force.h"
#include "State.h"

namespace GPUParticles {
	
	//----------
	class ForceSet {
	public:
		typedef vector<BaseForce*>::iterator ForceIterator;
		void load(string forceShaderDirectory);
		void add(string name);
		void add(BaseForce*);
		void clear();
		void update(State &);
		vector<BaseForce*> & getForces() { return this->forces; }
	protected:
		string path;
		vector<BaseForce*> forces;
	};
	
}