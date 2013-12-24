#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	secondaryDisplay.setup();

}

//--------------------------------------------------------------
void testApp::update(){
	secondaryDisplay.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	secondaryDisplay.drawOverlay();
    secondaryDisplay.draw();
    secondaryDisplay.drawSDLayer();
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key==' '){
        secondaryDisplay.animateOn();
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