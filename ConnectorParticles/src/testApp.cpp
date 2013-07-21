#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofSetVerticalSync(true);
	ofBackground(0);


	generator.numParticles = 200;
	
	generator.setup();
}

//--------------------------------------------------------------
void testApp::update(){
	generator.setMinDistance(mouseX);
	generator.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	generator.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}