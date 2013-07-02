#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);

	ofBackground(0);
	
	cam.setPosition(40, 40, 40);
	cam.lookAt(ofVec3f());
	cam.setup();
	
	
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
	
	gpuParticles.setup();
	
}

//--------------------------------------------------------------
void testApp::update(){
	
	gpuParticles.update();
	
}

//--------------------------------------------------------------
void testApp::draw(){

	cam.begin();
	gpuParticles.draw();
	cam.end();

	ofDrawBitmapStringHighlight("Framerate: " + ofToString(ofGetFrameRate()),
								ofPoint(20,20));
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if(key == 'r'){
		gpuParticles.reloadShaders();
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