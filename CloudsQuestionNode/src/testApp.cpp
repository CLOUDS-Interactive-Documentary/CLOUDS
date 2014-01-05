#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(0);
	ofSetVerticalSync(true);
	portal.cam = &easyCam;
	portal.setup();
    
#ifdef OCULUS_RIFT
    oculusRift.baseCamera = &easyCam;
    oculusRift.setup();
#endif
}

//--------------------------------------------------------------
void testApp::update(){
	portal.update();
    
#ifdef OCULUS_RIFT
    ofRectangle viewport = oculusRift.getOculusViewport();
    ofVec3f screenPos = oculusRift.worldToScreen(portal.hoverPosition, true);
    float gazeDist = ofDist(screenPos.x, screenPos.y,
                            viewport.getCenter().x, viewport.getCenter().y);
    if (gazeDist < portal.minSelectDistance) {
        if(!portal.hovering){
            portal.hoverStartTime = ofGetElapsedTimef();
        }
        portal.hovering = true;
        portal.hoverPercentComplete = ofClamp((ofGetElapsedTimef() - portal.hoverStartTime) / portal.maxHoverTime, 0,1.0);
    }
    else{
        portal.hovering = false;
        portal.hoverPercentComplete = 0;
    }
#endif
}

//--------------------------------------------------------------
void testApp::draw(){
    glDisable(GL_DEPTH_TEST);

#ifdef OCULUS_RIFT
    ofRectangle viewport = oculusRift.getOculusViewport();
    
    oculusRift.beginOverlay();
    ofPushStyle();
    ofNoFill();
    ofSetColor(255, 0, 0);
    ofCircle(oculusRift.getOverlayRectangle().getCenter(), 10);
    ofPopStyle();
    oculusRift.endOverlay();

    oculusRift.beginLeftEye();
	portal.draw();
    oculusRift.endLeftEye();
    
    oculusRift.beginRightEye();
    portal.draw();
    oculusRift.endRightEye();
    
    oculusRift.draw();
#else
	easyCam.begin();
	portal.draw();
	easyCam.end();
    
    ofPushStyle();
	ofSetColor(portal.hovering ? ofColor::yellow : ofColor::white);
	ofCircle(portal.screenPosition, 10);
	ofPopStyle();
#endif
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'R'){
		portal.reloadShader();
	}
    else if(key == 'f'){
        ofToggleFullscreen();
    }
#ifdef OCULUS_RIFT
    else if(key == ' '){
        oculusRift.reset();
    }
#endif
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
