
#include "CloudsVisualSystemComputationTicker.h"

//--------------------------------------------------------------
string CloudsVisualSystemComputationTicker::getSystemName(){
	return "ComputerTicker";
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::selfSetup(){
	regenerateParticles();
	
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
		
	regenerate = true;
	
	//choose a random preset
}


//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::reloadShaders(){
	
	sourceOffset.begin();
	ofClear(0);
	sourceOffset.end();
	
	targetOffset.begin();
	ofClear(0);
	targetOffset.end();
	
	drawShader.load( getDataPath() + "/shaders/VisualSystems/ComputationTicker/display");
	updateShader.load( getDataPath() + "/shaders/VisualSystems/ComputationTicker/update");
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::regenerateParticles(){
	
	float currentColorFlip = powf(colorflip,2.0);
	bool one = false;
	debug = false;
	width  = 1280;
	height = 720;
	mesh.clear();
	for(int x = 0; x < width; x++){
		for(int y = 0; y < height; y++){
			mesh.addColor( ofFloatColor( one ? 1 : 0) );
			mesh.addVertex(ofVec3f(x,y,0));
			if(ofRandomuf() > 1.0 - currentColorFlip ) one = !one;
			if(ofRandomuf() > .05) currentColorFlip = ofRandomf() * powf(colorflip, 2.0);
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
				if(current.r >= 128 && random > 1.0 - pathDeviation){
					shiftTexture.setColor(x, y, current+1);
				}
				else if(current.r <= 128 && random < -(1.0 - pathDeviation)){
					shiftTexture.setColor(x, y, current-1);
				}
				else{
					shiftTexture.setColor(x, y, current);
				}
			}
		}
	}
	
	shiftTexture.update();
	regenerate = false;
}


void CloudsVisualSystemComputationTicker::selfSetupGuis(){
	
}

void CloudsVisualSystemComputationTicker::selfUpdate(){
	if(regenerate){
		regenerateParticles();
	}
	
	ofPushStyle();
	
	ofDisableAlphaBlending();
	glDisable(GL_DEPTH_TEST);

	sourceOffset.begin();
	updateShader.begin();
	updateShader.setUniformTexture("image", targetOffset.getTextureReference(), 0);
	updateShader.setUniformTexture("speed", speedTexture.getTextureReference(), 1);
	updateShader.setUniform1f("speedMultiplier", powf(speed, 2.0));
	
	offsetMesh.draw();
	
	updateShader.end();
	sourceOffset.end();
	
	ofPopStyle();
	
	swap(sourceOffset,targetOffset);	
}

void CloudsVisualSystemComputationTicker::selfDrawBackground(){
	
}

void CloudsVisualSystemComputationTicker::selfDrawDebug(){
	
}

void CloudsVisualSystemComputationTicker::selfSceneTransformation(){

}

void CloudsVisualSystemComputationTicker::selfDraw(){
	
	drawShader.begin();
	drawShader.setUniformTexture("image", targetOffset.getTextureReference(), 0);
	drawShader.setUniformTexture("shift", shiftTexture.getTextureReference(), 1);
	drawShader.setUniform1f("height", height);
	drawShader.setUniform1f("deviation", deviation);
	
	ofPushStyle();
	ofEnableAlphaBlending();	
	glPushAttrib(GL_POINT_BIT);
	
	ofPushMatrix();
	ofSetLineWidth(10);
	glPointSize(pointSize);
	float scaleexp = powf(scale,2);
	ofTranslate(ofGetWidth()/2,ofGetHeight()/2 );
	ofScale(scaleexp, scaleexp);
	ofTranslate(-ofGetWidth()/2,-ofGetHeight()/2 );
	
	mesh.draw();
	
	ofPopMatrix();
	
	glPopAttrib();
	ofPopStyle();
	
	drawShader.end();
	
}

void CloudsVisualSystemComputationTicker::selfExit(){
	
}

void CloudsVisualSystemComputationTicker::selfBegin(){
	sourceOffset.begin();
	ofClear(0);
	sourceOffset.end();
	
	targetOffset.begin();
	ofClear(0);
	targetOffset.end();
		
}

void CloudsVisualSystemComputationTicker::selfEnd(){
	
}

void CloudsVisualSystemComputationTicker::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		reloadShaders();
	}
}

void CloudsVisualSystemComputationTicker::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemComputationTicker::selfMouseDragged(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::selfMouseMoved(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::selfMousePressed(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::selfMouseReleased(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::selfSetupGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::selfGuiEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::selfSetupSystemGui(){
	sysGui->addSlider("path color flip chance", 0, 1.0, &colorflip);
	sysGui->addSlider("path deviation", 0, 1000, &deviation);
	sysGui->addSlider("path deviation chance", 0, .2, &pathDeviation);
	sysGui->addLabelButton("regenerate particles", &regenerate);
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::guiSystemEvent(ofxUIEventArgs &e){
}

void CloudsVisualSystemComputationTicker::selfSetupRenderGui(){
	rdrGui->addSlider("speed", 0., 2., &speed);
	rdrGui->addSlider("point size", 1., 25., &pointSize);
	rdrGui->addSlider("scale", 1., 10., &scale);
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::guiRenderEvent(ofxUIEventArgs &e){
	
}

