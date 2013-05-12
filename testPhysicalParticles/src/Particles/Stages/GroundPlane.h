//
//  GroundPlane
//
#pragma once

#include "Force.h"

namespace Particles {
	class GroundPlane : public BaseForce {
	public:
		GroundPlane() { this->load(); }
		void step(State & );
		void load();
	protected:
		ofShader shader;
		string name;
	};
}