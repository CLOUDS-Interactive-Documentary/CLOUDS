#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	showOverlay = false;
    ofSetVerticalSync(false);
	ofSetBackgroundAuto(false);

	oculus.baseCamera = &cam;
    oculus.setup();
	ofDisableAlphaBlending();

}

//--------------------------------------------------------------
void testApp::update(){
	cam.begin();
	cam.end();
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
	}

	if (oculus.isSetup()) {
		oculus.draw();
	} else {
		cout << "NOT SETUP " << endl;
		cam.begin();
		drawScene();
		cam.end();
	}
}

void testApp::drawScene() {

	ofPushStyle();
	ofEnableDepthTest();

    ofPushMatrix();    
	ofTranslate(0, 0, 10);
	ofRotate(90, 0, 1, 0);
    ofSetColor(0,200,0);
    ofDrawGridPlane(12.0f, 8.0f, false);
	ofPopMatrix();

    ofPushMatrix();    
	ofTranslate(0, 0, -3);
	ofRotate(90, 0, 1, 0);    
	ofSetColor(200,0,0);
    ofDrawGridPlane(12.0f, 8.0f, false);
	ofPopMatrix();

    ofPushMatrix();    
	ofTranslate(0, 0, -6);
	ofRotate(90, 0, 1, 0);    
	ofSetColor(100,100,0);
    ofDrawGridPlane(12.0f, 8.0f, false);
	ofPopMatrix();

	ofDisableDepthTest();

	ofPopStyle();
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