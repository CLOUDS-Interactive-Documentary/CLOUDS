#include "testApp.h"
//#include "CloudsGlobal.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofBackground(0);
    
//#ifdef TONIC_SOUNDS
//    mixer.setup();
//#endif

#if defined(OCULUS_RIFT) && defined(TARGET_WIN32)
//    ofSetWindowPosition(0,0);
//    ofSetWindowShape(1920*2,1080);
#else
	#ifdef CLOUDS_RELEASE
//    ofSetWindowPosition(1920 + 1920*.5,1080*.5);
//    ofSetWindowPosition(0,0);
	#endif
//    ofSetWindowShape(1920,1080);
#endif

#ifdef VHX_MEDIA
    ofSetWindowShape(1280, 720);
    ofSetWindowPosition(ofGetScreenWidth()/2 - 1280/2, ofGetScreenHeight()/2 - 720/2);
#endif
    
#ifdef CLOUDS_RELEASE
	ofHideCursor();
#endif
		
	firstFrame = true;
	playerSetup = false;
	shouldSetupPlayer = false;
#ifdef OCULUS_RIFT
	loader.loadImage("CLOUDS_HD_BG_DOUBLE.png");
#else
	loader.loadImage("SCREEN_BG.png");
#endif


}

//--------------------------------------------------------------
void testApp::update(){
	if(ofGetFrameNum() == 10){
        #ifndef VHX_MEDIA
		ofToggleFullscreen();
        #endif
	}
    
	if(shouldSetupPlayer){
		player.setup();
		shouldSetupPlayer = false;
		playerSetup = true;
	}
	else if(firstFrame){
		shouldSetupPlayer = true;
	}

    if(ofGetFrameNum() == 100){
//        cout<< "SET UP MIXER" << endl;
//        player.mixer.setup();
    }
}

//--------------------------------------------------------------
void testApp::draw(){
	if(loader.isAllocated() && !playerSetup){
		loader.draw(0,0, ofGetWidth(), ofGetHeight());
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