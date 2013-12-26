#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(0);
	ofSetVerticalSync(true);
	portal.cam = &easyCam;
	portal.setup();
}

//--------------------------------------------------------------
void testApp::update(){
	portal.update();
}

//--------------------------------------------------------------
void testApp::draw(){

	easyCam.begin();
	glDisable(GL_DEPTH_TEST);
	portal.draw();
	
	easyCam.end();
	
	ofPushStyle();
	ofSetColor(portal.hovering ? ofColor::yellow : ofColor::white);
	ofCircle(portal.screenPosition, 10);
	ofPopStyle();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::exit(){
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
