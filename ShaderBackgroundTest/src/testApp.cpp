#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(0);
	ofSetVerticalSync(true);
	
	vm.setup();
}

//--------------------------------------------------------------
void testApp::update(){
	vm.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	cam.begin();
	vm.draw();
	cam.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'R'){
		vm.reloadShaders();
	}
	if(key == 'G'){
		vm.regenerateGeometry();
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