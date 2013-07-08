#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	
	ofBackground(0);
	ofSetSphereResolution(25);
	glEnable(GL_DEPTH_TEST);

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	
	cam.begin();
//	ofNoFill();
	
	backlight.begin();
	ofDrawSphere(0, 0, 150);
	backlight.end();
	
	cam.end();
}

//--------------------------------------------------------------
void testApp::reloadShader(){
	backlight.load("backlight");
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'R'){
		reloadShader();
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