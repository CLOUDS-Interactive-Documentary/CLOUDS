#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
	
	showOverlay = false;

	oculus.baseCamera = &cam;
    oculus.setup();

    //cam.setAutoDistance(false);
    cam.begin();
    cam.end();
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
  if (oculus.isSetup()) {
        if (showOverlay) {

            oculus.beginOverlay(-230, 320, 240);
            ofRectangle overlayRect = oculus.getOverlayRectangle();

            ofPushStyle();
            ofEnableAlphaBlending();
            ofFill();
            ofSetColor(255, 40, 10, 200);

            ofRect(overlayRect);

            ofSetColor(255, 255);
            ofFill();

            ofSetColor(0, 255, 0);
            ofNoFill();
            ofCircle(overlayRect.getCenter(), 20);

            ofPopStyle();
            oculus.endOverlay();
        }

        oculus.beginLeftEye();
        drawScene();
        oculus.endLeftEye();

        oculus.beginRightEye();
        drawScene();
        oculus.endRightEye();

        oculus.draw();
    } else {
		cout << "NOT SETUP " << endl;
        cam.begin();
        //drawScene();
        cam.end();
    }
}

void testApp::drawScene() {

    ofPushMatrix();
    ofRotate(90, 0, 0, -1);
    ofSetColor(30);
    ofDrawGridPlane(12.0f, 8.0f, false);
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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

}