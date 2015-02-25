#include "testApp.h"
#include "CloudsGlobal.h"
#include "CloudsIntroSequence.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetWindowPosition(0,0);
#if defined(OCULUS_RIFT) && defined(TARGET_WIN32)
	ofSetWindowShape(1920*2,1080);
#else
	ofSetWindowShape(1920,1080);
#endif
	ofSetVerticalSync(true);
	//ofSetFrameRate(60);
	ofBackground(0);
	//ofToggleFullscreen();

#ifdef CLOUDS_RELEASE
	ofHideCursor();
#endif

	firstFrame = true;
	playerSetup = false;
	shouldSetupPlayer = false;
#ifdef OCULUS_RIFT
//	loader.loadImage("OCULUS_BG_HD.png");
	loader.loadImage("CLOUDS_HD_BG_DOUBLE.png");
#else
	loader.loadImage("SCREEN_BG.png");
#endif

	//ofSetWindowShape(1080*2,1920);
	//ofSetWindowPosition(0,0);

}

//--------------------------------------------------------------
void testApp::update(){
	if(ofGetFrameNum() == 10){
		ofToggleFullscreen();
	}
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
		ofSaveImage(p,"OCULUS_BG_HD.png");
#endif
	}

	//if(key == '1'){
	//	int* crashme = NULL;
	//	*crashme = 0;
	//}

	//if(key == '2'){
	//	for(;;){}
	//}
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