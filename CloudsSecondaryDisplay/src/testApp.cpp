#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	secondaryDisplay.setup();
    
    layoutID = 0;

}

//--------------------------------------------------------------
void testApp::update(){
	secondaryDisplay.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	secondaryDisplay.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if (key == OF_KEY_RIGHT){
        secondaryDisplay.drawNextLayout();
    }
    else if(key == OF_KEY_LEFT){
        secondaryDisplay.drawPrevLayout();
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