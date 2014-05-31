#include "testApp.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	ofToggleFullscreen();

	firstFrame = true;
	playerSetup = false;
	shouldSetupPlayer = false;
#ifdef OCULUS_RIFT
	loader.loadImage("OCULUS_BG.png");
#else
	loader.loadImage("SCREEN_BG.png");
#endif

}

//--------------------------------------------------------------
void testApp::update(){
	if(shouldSetupPlayer){
		player.setup();
		shouldSetupPlayer = false;
		playerSetup = true;
	}
	else if(firstFrame){
		shouldSetupPlayer = true;
	}

}

//--------------------------------------------------------------
void testApp::draw(){
	if(loader.isAllocated() && !playerSetup){
		loader.draw(0,0);
	}
	firstFrame = false;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	if(key == 'L'){
#ifdef OCULUS_RIFT
		ofPixels p;
		CloudsVisualSystem::getOculusRift().getRenderTarget().readToPixels(p);
		ofSaveImage(p,"OCULUS_BG.png");
#endif
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