#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	state.init(1e6);
	this->keyPressed(' ');
	cam.setPosition(40, 40, 40);
	cam.lookAt(ofVec3f());

	
	fromAbove.allocate(1024, 768, GL_RGB);
	
	this->updating = true;
	
	birthSpread = 5.0f;
	birthPlace = ofVec3f(0,10,0);
	birthRate = 0.01f;
	debug = false;
	
	rx.setup(12345);
	
	this->windowResized(ofGetWidth(), ofGetHeight());
	
	cam.setup();
	cam.autosavePosition = true;
//	cam.loadCameraPosition();
	
	this->birthRate = .01;
	
//	//setup GUI
//	gui = new ofxUICanvas(0,0,300,ofGetHeight());
//	gui->addWidgetDown(new ofxUILabel("SIMPLE EXAMPLE", OFX_UI_FONT_LARGE));
//	
//    gui->addSpacer(length, 2);
//    gui->addWidgetDown(new ofxUILabel("BACKGROUND CONTROL", OFX_UI_FONT_MEDIUM));

}

//--------------------------------------------------------------
void testApp::update(){
	if (this->updating)
		forceSet.update(state);
	this->drawFromAbove();
	this->keyPressed('m');
	
	while (rx.hasWaitingMessages()) {
		ofxOscMessage m;
		rx.getNextMessage(&m);
		
		if (m.getAddress() == "/birth/x") {
			this->birthPlace.x = m.getArgAsFloat(0);
		} else if (m.getAddress() == "/birth/y") {
			this->birthPlace.y = m.getArgAsFloat(0);
		} else if (m.getAddress() == "/birth/z") {
			this->birthPlace.z = m.getArgAsFloat(0);
		} else if (m.getAddress() == "/birth/rate") {
			this->birthRate = m.getArgAsFloat(0);
		} else if (m.getAddress() == "/birth/spread") {
			this->birthSpread = m.getArgAsFloat(0);
		}
	}
}

//--------------------------------------------------------------
void testApp::drawGrid(){
    ofMesh grid;
    //floor grid (y == 0)
    float size = 300;
    float min = -size*20;
    float max = size*20;
    float step = size;
    for(float x = min; x < max; x+=step){
        grid.addVertex(ofVec3f(x,0,min));
        grid.addVertex(ofVec3f(x,0,max));
    }
    for(float z = min; z < max; z+=step){
        grid.addVertex(ofVec3f(min,0,z));
        grid.addVertex(ofVec3f(max,0,z));
    }
    
    ofPushStyle();
    
	//    GLfloat density = timeline.getValue("Plane Fog");
	//    GLfloat fogColor[4] = {0.0, 0.0, 0.0, 1.0};
	//    glEnable (GL_FOG);
	//    glFogi (GL_FOG_MODE, GL_EXP2);
	//    glFogfv (GL_FOG_COLOR, fogColor);
	//    glFogf (GL_FOG_DENSITY, density);
	//    glHint (GL_FOG_HINT, GL_NICEST);
    
    ofSetColor(200);
    ofEnableSmoothing();
    ofSetLineWidth(2);
    grid.setMode(OF_PRIMITIVE_LINES);
    grid.draw();
    
	//    camTrack.draw3d();
	//    glDisable(GL_FOG);
    
    ofPopStyle();

}

//--------------------------------------------------------------
void testApp::draw(){
	
//	gravity.begin();
//	gravity.setUniform1f("t", ofGetElapsedTimef());
//	gravity.setUniform1f("dt", ofGetLastreeeFrameTime());
//	state.getSourcePosition().draw(0,512);
//	gravity.end();
	
	//multiSample.begin();
	ofClear(0);
	//ofBackgroundGradient(ofColor(220,230,255), ofColor(100,100,200));

	cam.begin();
	glEnable(GL_DEPTH_TEST);

	//ofDrawGrid(10, 10, true);
	//ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	ofPushStyle();
	ofSetColor(255, 255, 255, 50);
	ofPopStyle();
	//glEnable(GL_POINT_SMOOTH);
	points.draw();
	//glDisable(GL_POINT_SMOOTH);
	
	ofPushMatrix();
	ofRotate(90, 1, 0, 0);
	fromAbove.draw(-1000, -1000, 2000, 2000);
	ofPopMatrix();
	
	drawGrid();
	
	glDisable(GL_DEPTH_TEST);
	cam.end();
	
	
	//multiSample.end();
	//multiSample.draw(0, ofGetHeight(), ofGetWidth(), -ofGetHeight());

	if (debug) {
		state.getTargetPosition().draw(0, 0, 128, 128);
		state.getTargetVelocity().draw(128, 0, 128, 128);
		state.getSourcePosition().draw(128*2, 0, 128, 128);
		state.getSourceVelocity().draw(123*3, 0, 128, 128);
		
		ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, ofGetHeight() - 10);
		
		if (ofGetMouseX() < 512 && ofGetMouseY() < 128) {
			ofFbo * fbo;
			switch (ofGetMouseX() / 128) {
				case 0:
					fbo = & state.getSourcePosition();
					break;
				case 1:
					fbo = & state.getSourceVelocity();
					break;
				case 2:
					fbo = & state.getTargetPosition();
					break;
				case 3:
					fbo = & state.getTargetVelocity();
					break;
					
				default:
					break;
			}
			
			float mousex = ofGetMouseX() % 128;
			float mousey = ofGetMouseY();
			
			mousex = mousex / 128.0f * state.getWidth();
			mousey = mousey / 128.0f * state.getHeight();
			
			ofFloatPixels pix;
			fbo->readToPixels(pix);
			ofVec3f value = *(ofVec3f*)(pix.getPixels() + int(3*(mousex + mousey * state.getWidth())));
			ofDrawBitmapString(ofToString(value), ofGetMouseX(), ofGetMouseY());
		}
		
		if (!this->updating)
			ofDrawBitmapString("not updating", 10, ofGetHeight() - 30);
	}
}

//--------------------------------------------------------------
void testApp::reload(){
	
}

//--------------------------------------------------------------
void testApp::drawFromAbove() {
	fromAbove.bind();

	ofViewport(0,0,fromAbove.getWidth(), fromAbove.getHeight());
	ofBackgroundGradient(ofColor(100), ofColor(0,0,0,0));
	
	ofPushView();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	ofRotate(90, 1, 0, 0);
	ofScale(0.01, 0.01, 0.01);
	
	ofSetColor(0);
	points.draw();
	
	ofPopView();
	
	fromAbove.unbind();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == ' ') {
		null.load("default.vert","null.frag");
		forceSet.load();
		//forceSet.add(new GroundPlane());
		state.loadShader();
		points.setState(state);
	}
	
	if (key == 'C')
		state.clear();
	if (key == 'm') {
		int resolution = state.getWidth();
		random.allocate(resolution, resolution, OF_IMAGE_COLOR_ALPHA);
		float* x = random.getPixels();
		for (int i=0; i<resolution*resolution; i++) {
			*x++ = ofRandomf() * birthSpread + birthPlace.x;
			*x++ = ofRandomf() * birthSpread + birthPlace.y;
			*x++ = ofRandomf() * birthSpread + birthPlace.z;
			*x++ = ofRandomuf() < birthRate;
		}
		
		ofEnableAlphaBlending();
		random.update();
		state.getTargetPosition().begin();
		random.draw(0,0);
		state.getTargetPosition().end();
		
		ofPushStyle();
		state.getTargetVelocity().begin();
		random.draw(0,state.getHeight(), state.getWidth(), -state.getHeight());
		state.getTargetVelocity().end();
		ofPopStyle();
		ofDisableAlphaBlending();
	}
	
	if (key == 'u')
		updating ^= true;
	
	if (key == 'D')
		debug ^= true;
	
	if(key == 'R'){
		
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
	ofFbo::Settings settings;
	settings.width = w;
	settings.height = h;
	settings.internalformat = GL_RGBA;
	settings.depthStencilInternalFormat = GL_DEPTH24_STENCIL8;
	multiSample.allocate(settings);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}