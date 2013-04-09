//
//  ForceSet.h
//
#pragma once

#include "Force.h"
#include "State.h"

namespace Particles {
	
	//----------
	class ForceSet {
	public:
		typedef vector<BaseForce*>::iterator ForceIterator;
		void load();
		void add(string name);
		void add(BaseForce*);
		void clear();
		void update(State &);
		vector<BaseForce*> & getForces() { return this->forces; }
	protected:
		vector<BaseForce*> forces;
	};
	
}