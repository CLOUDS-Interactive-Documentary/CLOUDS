#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofSetVerticalSync(true);
	vs.setup();
	vs.playSystem();
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == '1'){
        vs.selectSystem("LaplacianTunnel");
    }
    else if(key == '2'){
        vs.selectSystem("Replicator");
    }
    else if(key == '3'){
        vs.selectSystem("Swim");
    }
}

//--------------------------------------------------------------
void testApp::exit()
{
    vs.exit();
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