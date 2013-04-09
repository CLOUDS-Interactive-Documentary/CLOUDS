#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	

	bool one = false;
	debug = false;
	float turnoverRate = .01;
	width  = 1280;
	height = 720;
	for(int x = 0; x < width; x++){
		for(int y = 0; y < height; y++){

			mesh.addColor( ofFloatColor( one ? 1 : 0) );
			mesh.addVertex(ofVec3f(x,y,0));
			if(ofRandomuf() > 1.0 - turnoverRate ) one = !one;
			if(ofRandomuf() > .05) turnoverRate = ofRandomf() * .2;
			
		}
	}
	mesh.setMode(OF_PRIMITIVE_POINTS);
	
	shiftTexture.allocate(width, height, OF_IMAGE_COLOR);
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			if(y == 0){
				shiftTexture.setColor(x, y, ofColor(128));
			}
			else{
				ofColor current = shiftTexture.getColor(x, y-1);
				float random = ofRandomf();
				if(current.r >= 128 && random > .9){
					shiftTexture.setColor(x, y, current+1);
				}
				else if(current.r <= 128 && random < -.9){
					shiftTexture.setColor(x, y, current-1);
				}
				else{
					shiftTexture.setColor(x, y, current);
				}
			}
		}
	}
	shiftTexture.update();
	
	fbo.allocate(width, height, GL_RGB);
	
	speedTexture.allocate(width, 1, OF_IMAGE_COLOR);
	for(int i = 0; i < width; i++){
		speedTexture.setColor(i, 0, ofRandom(10,255) );
	}
	speedTexture.reloadTexture();
	
	sourceOffset.allocate(width, 1, GL_RGB32F);
	targetOffset.allocate(width, 1, GL_RGB32F);

	offsetMesh.addVertex(ofVec3f(0,0,0));
	offsetMesh.addVertex(ofVec3f(0,1,0));
	offsetMesh.addVertex(ofVec3f(width,0,0));
	offsetMesh.addVertex(ofVec3f(width,1,0));
	offsetMesh.addTexCoord(ofVec3f(0,0));
	offsetMesh.addTexCoord(ofVec3f(0,1));
	offsetMesh.addTexCoord(ofVec2f(width,0));
	offsetMesh.addTexCoord(ofVec2f(width,1));
	
	offsetMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	
	reloadShaders();
}

//--------------------------------------------------------------
void testApp::update(){
	
	ofPushStyle();
	

//	glPushAttrib(GL_BLEND);
//	glBlendFunc(GL_ONE, GL_ZERO);
	ofDisableAlphaBlending();
	sourceOffset.begin();
	updateShader.begin();
	updateShader.setUniformTexture("image", targetOffset.getTextureReference(), 0);
	updateShader.setUniformTexture("speed", speedTexture.getTextureReference(), 1);
	
	offsetMesh.draw();

	updateShader.end();
	sourceOffset.end();

//	glPopAttrib();
	
	ofPopStyle();
	
	swap(sourceOffset,targetOffset);
}

//--------------------------------------------------------------
void testApp::reloadShaders(){
	
	sourceOffset.begin();
	ofClear(0);
	sourceOffset.end();
	
	targetOffset.begin();
	ofClear(0);
	targetOffset.end();
	
	drawShader.load("display");
	updateShader.load("update");
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(0);
	
	ofRectangle screenRect(0,0,ofGetWidth(), ofGetHeight());
	ofRectangle videoRect(0,0,fbo.getWidth(), fbo.getHeight());
	videoRect.scaleTo(screenRect);

	if(debug){
		//speedTexture.getTextureReference().draw(videoRect);
		//targetOffset.getTextureReference().draw(videoRect);
		shiftTexture.draw(videoRect);
	}
	else {
		fbo.begin();
		ofClear(128);
		
//		cam.begin();
		
		drawShader.begin();
		drawShader.setUniformTexture("image", targetOffset.getTextureReference(), 0);
		drawShader.setUniformTexture("shift", shiftTexture.getTextureReference(), 1);
		drawShader.setUniform1f("height", height);
		
		ofPushMatrix();
		ofSetLineWidth(10);
		glPointSize(2);
		ofScale(mouseX, mouseX);
		mesh.draw();
		
		ofPopMatrix();
		
		drawShader.end();
		
		fbo.end();
		
//		cam.end();
		
		fbo.getTextureReference().draw(videoRect);
	}
	
	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(0, 0, 0, 100);
	ofRect(10,10,200,20);
	ofSetColor(255);
	ofDrawBitmapString(ofToString( ofGetFrameRate() ), 25, 20);
	ofPopStyle();
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if(key == 'r'){
		reloadShaders();
	}
	
	if(key == 'D'){
		debug = !debug;
	}
	
	if(key == 'f'){
		ofToggleFullscreen();
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