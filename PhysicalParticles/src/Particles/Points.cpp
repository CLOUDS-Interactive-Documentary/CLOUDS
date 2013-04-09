//
//  Points.cpp
//

#include "Points.h"

namespace Particles {
	//--------
	void Points::setState(State & state) {
		this->state = & state;
		
		mesh.clear();
		for (int i=0; i<state.getWidth(); i++)
			for (int j=0; j<state.getHeight(); j++) {
				mesh.addVertex(ofVec3f(i, j));
				mesh.addTexCoord(ofVec2f(i, j));
			}
		
		displace.load("displace");
	}

	//--------
	void Points::customDraw() {
		glPointSize(2);
		displace.begin();
		displace.setUniformTexture("positions", this->state->getSourcePosition(), 0);
		mesh.drawVertices();
		displace.end();
	}
}