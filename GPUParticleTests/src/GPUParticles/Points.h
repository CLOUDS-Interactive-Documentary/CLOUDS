//
//  Points.h
//

#include "ofMain.h"

#include "State.h"

namespace GPUParticles {
	class Points : public ofNode {
	public:
		void setState(State &);
		void customDraw();
	protected:
		State * state;
		ofMesh mesh;
		ofShader displace;
	};
}