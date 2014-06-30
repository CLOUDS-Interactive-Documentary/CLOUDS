#include "testApp.h"

#include "CloudsInputMouse.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);

	gesturePaint.setDrawToScreen(false);
  
	gesturePaint.setup();
	gesturePaint.playSystem();

}


//--------------------------------------------------------------
void testApp::update(){

}


//--------------------------------------------------------------
void testApp::draw(){
	
	gesturePaint.selfPostDraw();

	//#ifdef MOUSE_INPUT
	
	if (gesturePaint.bPromptForInteraction) {
		bool drawBrightText = gesturePaint.getBgColor().getBrightness() < .5;
		ofPtr<CloudsInputMouse> mouseInput = dynamic_pointer_cast<CloudsInputMouse>(GetCloudsInput());
		mouseInput->drawFeedback(
			gesturePaint.getCanvasWidth(), 
			gesturePaint.getCanvasHeight(), drawBrightText);
    }
	
	//#endif
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::exit(){
	gesturePaint.exit();
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
