//
//  Points.cpp
//

#include "Points.h"

namespace GPUParticles {
	//--------
	void Points::setState(State & state) {
		this->state = & state;
		sizeThreshold = .001;
		
		mesh.clear();
		for (int i=0; i<state.getWidth(); i++){
			for (int j=0; j<state.getHeight(); j++) {
				mesh.addVertex( ofVec3f(i, j) );
				mesh.addTexCoord( ofVec2f(i, j) );
				mesh.addNormal( ofVec3f(0,0, ofRandomuf()) );
			}
		}
		
		displace.load(state.getPath() + "displace");
	}

	//--------
	void Points::customDraw() {
		
		ofPushStyle();
		ofSetColor(color);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
		
		displace.begin();
		displace.setUniformTexture("positions", this->state->getSourcePosition(), 0);
		displace.setUniform1f("sizeThreshold", sizeThreshold);
		
		mesh.drawVertices();
		
		displace.end();
		
		ofPopStyle();
		
	}
}