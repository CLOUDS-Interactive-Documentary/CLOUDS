#include "testApp.h"
//#include "CloudsInputKinectOSC.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
    
    //SetCloudsInputKinect();
    verlet.setNumSamples(4);
	verlet.forceScreenResolution(1920*2, 1080*2);
	verlet.setDrawToScreen(false);
	verlet.setup();
	verlet.playSystem();
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofRectangle screenRect(0,0,ofGetWidth(), ofGetHeight());
	ofRectangle videoRect(0,0,1920*2,1080*2);
	
	videoRect.scaleTo(screenRect);
	verlet.getSharedRenderTarget().getTextureReference().draw(videoRect);
	
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		ofPixels p;
		verlet.getSharedRenderTarget().readToPixels(p);
		
		char screenshot[1024];

		sprintf(screenshot, "screencapture_%d_%d_%d_%d.png",
				ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds());
		ofSaveImage(p, screenshot);
	}
}

//--------------------------------------------------------------
void testApp::exit(){
    verlet.exit();
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