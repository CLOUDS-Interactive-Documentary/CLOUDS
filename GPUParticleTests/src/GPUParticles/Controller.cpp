//
//  GPUParticles.cpp
//  gpuParticles_remakeProject
//
//  Created by James George on 7/1/13.
//
//

#include "Controller.h"

namespace GPUParticles {

	Controller::Controller(){
		updating = true;
		numParticles = 50000;
	}
	
	void Controller::setup(){
		
		state.init(numParticles);
		
		reloadShaders();
		
		birthSpread = 500.0f;
		birthPlace = ofVec3f(0,10,0);
		birthRate = 0.01f;
	}
	
	void Controller::reloadShaders(){
		null.load("default.vert","null.frag");
		forceSet.load();
		state.loadShader();
		points.setState(state);
	}
	
	void Controller::update() {
		
		forceSet.update(state);
		
		int resolution = state.getWidth();
		random.allocate(resolution, resolution, OF_IMAGE_COLOR_ALPHA);
		float* x = random.getPixels();
		for (int i = 0; i < resolution*resolution; i++) {
			*x++ = ofRandomf() * birthSpread + birthPlace.x;
			*x++ = ofRandomf() * birthSpread + birthPlace.y;
			*x++ = ofRandomf() * birthSpread + birthPlace.z;
			
			*x++ = ofRandomuf() < birthRate;
		}

		ofEnableAlphaBlending();
		
		random.update();
		state.getTargetPosition().begin();
		random.draw(0,0);
		state.getTargetPosition().end();

		ofPushStyle();
		state.getTargetVelocity().begin();
		random.draw(0,state.getHeight(), state.getWidth(), -state.getHeight());
		state.getTargetVelocity().end();
		ofPopStyle();
		
		ofDisableAlphaBlending();
	}
	
	void Controller::draw(){

		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
	
		points.draw();

		ofPopStyle();
		
	}
	
	void Controller::reset(){
		state.clear();
	}
	
	void Controller::drawDebug(){

		state.getTargetPosition().draw(0, 0, 128, 128);
		state.getTargetVelocity().draw(128, 0, 128, 128);
		state.getSourcePosition().draw(128*2, 0, 128, 128);
		state.getSourceVelocity().draw(123*3, 0, 128, 128);
		
		ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, ofGetHeight() - 10);
		
		if (ofGetMouseX() < 512 && ofGetMouseY() < 128) {
			ofFbo * fbo;
			switch (ofGetMouseX() / 128) {
				case 0:
					fbo = & state.getSourcePosition();
					break;
				case 1:
					fbo = & state.getSourceVelocity();
					break;
				case 2:
					fbo = & state.getTargetPosition();
					break;
				case 3:
					fbo = & state.getTargetVelocity();
					break;
					
				default:
					break;
			}
			
			float mousex = ofGetMouseX() % 128;
			float mousey = ofGetMouseY();
			
			mousex = mousex / 128.0f * state.getWidth();
			mousey = mousey / 128.0f * state.getHeight();
			
			ofFloatPixels pix;
			fbo->readToPixels(pix);
			ofVec3f value = *(ofVec3f*)(pix.getPixels() + int(3*(mousex + mousey * state.getWidth())));
			ofDrawBitmapString(ofToString(value), ofGetMouseX(), ofGetMouseY());
		}
		
		if (!this->updating){
			ofDrawBitmapString("not updating", 10, ofGetHeight() - 30);
		}
	}
	
};