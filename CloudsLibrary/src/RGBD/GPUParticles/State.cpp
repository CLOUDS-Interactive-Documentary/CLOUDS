//
//  State.cpp
//

#include "State.h"

namespace GPUParticles {
	//--------
	void allocate(ofFbo & fbo, int resolution) {
		fbo.allocate(resolution, resolution, GL_RGB32F);
	}
	
	//--------
	void State::init(int particleCount, string path) {
		

		shaderPath = path;
		float root = sqrt( (float) particleCount);
		this->resolution = 1 << (int) floor(log2(root));
		this->particleCount = resolution * resolution;
		this->loadShader();
		
		quad.clear();
		quad.addVertex(ofVec3f(0,0,0));
		quad.addVertex(ofVec3f(resolution, 0, 0));
		quad.addVertex(ofVec3f(0, resolution, 0));
		quad.addVertex(ofVec3f(resolution, resolution, 0));
		quad.addTexCoord(ofVec3f(0,0,0));
		quad.addTexCoord(ofVec3f(resolution, 0, 0));
		quad.addTexCoord(ofVec3f(0, resolution, 0));
		quad.addTexCoord(ofVec3f(resolution, resolution, 0));
		quad.addIndex(1);
		quad.addIndex(0);
		quad.addIndex(2);
		quad.addIndex(2);
		quad.addIndex(1);
		quad.addIndex(3);
		
		this->clear();
	}
	
	//--------
	void wipe(ofFbo & fbo) {
		fbo.bind();
		ofClear(0,0,0);
		fbo.unbind();
	}
	
	//--------
	void State::clear() {
		allocate(sourcePosition, resolution);
		allocate(sourceVelocity, resolution);
		allocate(targetPosition, resolution);
		allocate(targetVelocity, resolution);
		
		wipe(this->targetPosition);
		wipe(this->targetVelocity);
		wipe(this->sourcePosition);
		wipe(this->sourceVelocity);
		
		this->update();
	}
	
	//--------
	void State::update() {
		
		//perform integration
		sourcePosition.begin();
		ofClear(0);
		integrate.begin();
		
		integrate.setUniformTexture("position", this->getTargetPosition().getTextureReference(), 0);
		integrate.setUniformTexture("velocity", this->getTargetVelocity().getTextureReference(), 1);
		integrate.setUniform1f("dt", ofGetLastFrameTime());
		
		glPushAttrib(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);
		this->drawQuad();
		
		integrate.end();
		
		glPopAttrib();
		
		sourcePosition.end();
	
		std::swap(sourcePosition, targetPosition);
		std::swap(sourceVelocity, targetVelocity);
	}
	
	//--------
	ofFbo & State::getSourcePosition() {
		return this->sourcePosition;
	}
	
	//--------
	ofFbo & State::getSourceVelocity() {
		return this->sourceVelocity;
	}
	
	//--------
	ofFbo & State::getTargetPosition() {
		return this->targetPosition;
	}
	
	//--------
	ofFbo & State::getTargetVelocity() {
		return this->targetVelocity;
	}
	
	//--------
	void State::loadShader() {
		null.load(shaderPath + "default.vert", shaderPath + "null.frag");
		integrate.load(shaderPath + "default.vert", shaderPath + "integrate.frag");
	}
	
	//--------
	void State::drawQuad() {
		this->quad.drawFaces();
	}
}