#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);

	
	this->keyPressed(' ');
	cam.setPosition(40, 40, 40);
	cam.lookAt(ofVec3f());
	cam.setup();
	
	gpuParticles.setup();
	
}

//--------------------------------------------------------------
void testApp::update(){
	
	gpuParticles.update();
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
//	gravity.begin();
//	gravity.setUniform1f("t", ofGetElapsedTimef());
//	gravity.setUniform1f("dt", ofGetLastreeeFrameTime());
//	state.getSourcePosition().draw(0,512);
//	gravity.end();
	
	//multiSample.begin();
	ofClear(0);
	//ofBackgroundGradient(ofColor(220,230,255), ofColor(100,100,200));

	cam.begin();
	
	gpuParticles.draw();
	
	cam.end();
	
	
	//multiSample.end();
	//multiSample.draw(0, ofGetHeight(), ofGetWidth(), -ofGetHeight());

}

//--------------------------------------------------------------
void testApp::drawFromAbove() {
//	fromAbove.bind();
//
//	ofViewport(0,0,fromAbove.getWidth(), fromAbove.getHeight());
//	ofBackgroundGradient(ofColor(100), ofColor(0,0,0,0));
//	
//	ofPushView();
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glMatrixMode(GL_MODELVIEW);
//	ofRotate(90, 1, 0, 0);
//	ofScale(0.01, 0.01, 0.01);
//	
//	//ofColor(0);
//	points.draw();
//	
//	ofPopView();
//	
//	fromAbove.unbind();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == ' ') {
	
	}
	
	
	if (key == 'm') {
	}
	
//	if (key == 'u')
//		updating ^= true;
//	
//	if (key == 'd')
//		debug ^= true;
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