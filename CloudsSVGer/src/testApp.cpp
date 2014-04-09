#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(0);
	ofSetVerticalSync(true);
    
	parser.loadFromFiles();
	svgReader.parser = &parser;
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	svgReader.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if(key == 'E'){
		svgReader.exportFile("textlayer.svg");
	}
	
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
	svgReader.loadFile( dragInfo.files[0] );
}
