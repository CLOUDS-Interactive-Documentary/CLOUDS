#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(0);
	ofSetVerticalSync(true);
	portal.setup();
}


//--------------------------------------------------------------
void testApp::update(){
	portal.update();
}


//--------------------------------------------------------------
void testApp::draw(){
	ofEnableAlphaBlending();
	portal.draw();	
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
