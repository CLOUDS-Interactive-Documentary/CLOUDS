//
//  Force
//
#pragma once

#include "ofShader.h"
#include "State.h"

namespace GPUParticles {
	class BaseForce {
	public:
		virtual ~BaseForce() { }
		virtual void step(State & ) = 0;
		virtual void load() { };
	};
	
	class Force : public BaseForce {
	public:
		Force(string name);
		void step(State & );
		void load();
	protected:
		ofShader shader;
		string name;
	};
}