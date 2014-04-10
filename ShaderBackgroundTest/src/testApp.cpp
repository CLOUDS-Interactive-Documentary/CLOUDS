#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(0);
	ofSetVerticalSync(true);
	
	
	gui = new ofxUISuperCanvas("GUI");
	gui->addIntSlider("num voxels", 10, 100, &vm.numVoxels);
	gui->addSlider("voxel spacing", 10, 100, &vm.voxelWidth);
	
	gui->addSlider("sphere radius",  100, 800, &vm.sphereRadius);
	gui->addSlider("sphere percent", -2.0, 2.0, &vm.spherePercent);
	
	gui->addRangeSlider("distance range", 0.0, 2.0, &vm.minDistance, &vm.maxDistance);

	gui->addSlider("twist factor",  0, .005, &vm.twistFactor);
	gui->addSlider("noise distort", 0, 1.0, &vm.noiseDistort);
	gui->addSlider("noise density", 0, 0.2, &vm.noiseDensity);
	gui->addSlider("noise speed",   0, 0.1, &vm.noiseSpeed);


	gui->loadSettings("settings.xml");
	
	vm.setup();
	
}

//--------------------------------------------------------------
void testApp::update(){
	vm.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	cam.begin();
	ofPushMatrix();
//	ofRotate(ofGetElapsedTimef()*50, 0, 1, 0);
	vm.draw();
	ofPopMatrix();
	cam.end();
}

//--------------------------------------------------------------
void testApp::exit(){
	gui->saveSettings("settings.xml");
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