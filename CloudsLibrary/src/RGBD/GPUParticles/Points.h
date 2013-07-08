//
//  Points.h
//

#pragma once

#include "ofMain.h"

#include "State.h"

namespace GPUParticles {
	class Points : public ofNode {
	public:
		void setState(State &);
		void customDraw();
		
		ofFloatColor color;
		float sizeThreshold;

	protected:
		
		State * state;
		ofMesh mesh;
		ofShader displace;
	};
}