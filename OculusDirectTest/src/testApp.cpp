#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	showOverlay = true;
    ofSetVerticalSync(false);
	ofSetBackgroundAuto(false);

	font.loadFont("GUI/NewMedia Fett.ttf",40);
	oculus.baseCamera = &cam;
    oculus.setup();

	ofDisableAlphaBlending();

	oculus.setFadeColor( ofFloatColor(0.,0.,0.,1.) );
	oculus.setPlayerScale(1000);
	oculus.beginBackground();

	oculus.endBackground();

	createLookTargets();

}

//--------------------------------------------------------------
void testApp::createLookTargets(){
	for(int i = 0; i < 1; i++){
		LookTarget t;
		t.pos = ofVec3f(
			ofRandom(-100,100),
			ofRandom(-100,100),
			ofRandom(-100,100));

		lookTargets.push_back(t);
	}
}

//--------------------------------------------------------------
void testApp::update(){
	cam.begin();
	cam.end();

	if(oculus.getButtonClicked(ovrButton_Enter)){
		cout << "ENTER CLICKED!" << endl;
	}

	if(oculus.getButtonClicked(ovrButton_Left)){
		cout << "LEFT CLICKED!" << endl;
	}

	if(oculus.getButtonClicked(ovrButton_Back)){
		cout << "BACK CLICKED!" << endl;
	}

	if(oculus.getButtonClicked(ovrButton_Home)){
		cout << "HOME CLICKED!" << endl;
	}

	ofVec2f screencenter(oculus.getViewport().width/2, oculus.getViewport().height/2);

	for(int i = 0; i < lookTargets.size(); i++){
		
		lookTargets[i].screenPos = oculus.worldToScreen(lookTargets[i].pos);
		lookTargets[i].distance = ofVec2f(lookTargets[i].screenPos.x, lookTargets[i].screenPos.y).distance(screencenter);
		lookTargets[i].lookedAt = lookTargets[i].distance < 50;
		//cout << "distance is\t" << distance << " center\t" << screencenter << " SCREEN POS\t" << lookTargets[i].screenPos << " WORLD POS\t" << lookTargets[i].pos << endl;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	if (oculus.isSetup()) {

		showOverlay = true;
        if (showOverlay) {

			oculus.beginOverlay(-(float)ofGetMouseY() / (float)ofGetHeight(), 1., 1920, 1080);
            ofRectangle overlayRect = oculus.getOverlayRectangle();

            ofPushStyle();
			
            ofEnableAlphaBlending();
            ofFill();
            ofSetColor(255, 40, 10, 50);
			ofRect( overlayRect );

            ofSetColor(255, 255);
            ofFill();
			ofSetColor(255);
			ofVec2f screencenter(oculus.getViewport().width/2, oculus.getViewport().height/2);
			font.drawString( ofToString(lookTargets[0].screenPos.x - screencenter.x) + "\n" + 
							 ofToString(lookTargets[0].screenPos.y - screencenter.y), 
							 overlayRect.getCenter().x,overlayRect.getCenter().y);
            ofSetColor(0, 255, 0);
            ofNoFill();
			
			ofSetRectMode(OF_RECTMODE_CENTER);
			ofCircle(overlayRect.getCenter(), 20);
			
			ofLine(0, overlayRect.getCenter().y,
				overlayRect.width, overlayRect.getCenter().y);
			ofLine(overlayRect.getCenter().x, 0,
				   overlayRect.getCenter().x, overlayRect.height);

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
		ofPushStyle();
		ofCircle(
			lookTargets[0].screenPos.x/oculus.getViewport().width  * ofGetWidth(),
			lookTargets[0].screenPos.y/oculus.getViewport().height * ofGetHeight(), 20);
		ofNoFill();
		ofCircle(ofGetWidth()/2, ofGetHeight()/2, 20);
		ofPopStyle();

	} else {
		//cout << "NOT SETUP " << endl;
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

	for(int i = 0; i < lookTargets.size(); i++){
		ofPushStyle();
		ofNoFill();
		if(lookTargets[i].lookedAt){
			ofSetColor(255,0,0);
		}
		else{
			ofSetColor(255);
		}
		ofDrawSphere(lookTargets[i].pos, 4);
		ofDisableDepthTest();
		ofSetColor(255,0,0);
		//ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
		ofDrawBitmapString(ofToString(lookTargets[i].screenPos.x) + ", " + ofToString(lookTargets[i].screenPos.y), lookTargets[i].pos);
		ofEnableDepthTest();
		ofPopStyle();
	}
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