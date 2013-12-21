#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(0);
	ofSetVerticalSync(true);
	lineAnimate.load("shaders/line_animate");
	mesh.addNormal(ofVec3f(0,0,0));
	mesh.addVertex(ofVec3f(0, ofGetHeight()/2,0));
	mesh.addNormal(ofVec3f(1.,0,0));
	mesh.addVertex(ofVec3f(ofGetWidth(), ofGetHeight()/2,0));
	
	mesh.setMode(OF_PRIMITIVE_LINES);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

	lineAnimate.begin();
	lineAnimate.setUniform1f("percent", fmod(ofGetElapsedTimef(),1.0f) );
	mesh.draw();
	lineAnimate.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'R'){
		lineAnimate.load("shaders/line_animate");
	}
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