
#include "CloudsVisualSystemRGBD.h"

//--------------------------------------------------------------
string CloudsVisualSystemRGBD::getSystemName(){
	return "RGBD";
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetup(){

	for(int y = 0; y < 480; y++){
		for(int x = 0; x < 640; x++){
			randomPoints.addVertex(ofVec3f(x,y,0));
		}
	}

	generateScanlines();
}

void CloudsVisualSystemRGBD::selfSetupGuis(){
	
}

void CloudsVisualSystemRGBD::selfUpdate(){
	if(refreshScanlineMesh){
		generateScanlines();
	}
}

void CloudsVisualSystemRGBD::generateScanlines(){

	verticalScanLines.clear();
	horizontalScanLines.clear();
	
	if(scanlineSimplify.x <= 0) scanlineSimplify.x = 1;
	if(scanlineSimplify.y <= 0) scanlineSimplify.y = 1;
	
	int height = 480;
	int width = 640;
	
	for (float xstep = 0; xstep < width-scanlineSimplify.x; xstep += scanlineSimplify.x){
		for (float ystep = 0; ystep < height-scanlineSimplify.y; ystep += scanlineSimplify.y){
			verticalScanLines.addVertex( ofVec3f(xstep, ystep, 0) );
			verticalScanLines.addVertex( ofVec3f(xstep, ystep+scanlineSimplify.y, 0) );
		}
	}
	for (float ystep = 0; ystep < height-scanlineSimplify.y; ystep += scanlineSimplify.y){
		for (float xstep = 0; xstep < width-scanlineSimplify.x; xstep += scanlineSimplify.x){
			horizontalScanLines.addVertex( ofVec3f(xstep, ystep, 0) );
			horizontalScanLines.addVertex( ofVec3f(xstep+scanlineSimplify.x, ystep, 0) );
		}
	}

	verticalScanLines.setMode(OF_PRIMITIVE_LINES);
	horizontalScanLines.setMode(OF_PRIMITIVE_LINES);
	
	if(sharedRenderer != NULL){
		sharedRenderer->setSimplification(scanlineSimplify);
	}
	else{
		ofLogError() << "CloudsVisualSystemRGBD::generateScanlines -- renderer not set up!";
	}
	
	refreshScanlineMesh = false;
}

void CloudsVisualSystemRGBD::selfDrawBackground(){
	
}

void CloudsVisualSystemRGBD::selfDrawDebug(){
	
}

void CloudsVisualSystemRGBD::selfSceneTransformation(){

}

void CloudsVisualSystemRGBD::selfDraw(){
	if(sharedRenderer != NULL && hasSpeaker){
		ofPushStyle();
		ofPushMatrix();
		
		glDisable(GL_DEPTH_TEST);
		
		//Enable smooth lines and additive blending
		
		//move the pointcloud
		ofTranslate(0,0,pointcloudOffsetZ);
		ofScale(pointcloudScale, pointcloudScale,pointcloudScale);

		ofEnableSmoothing();
		ofEnableBlendMode(OF_BLENDMODE_SCREEN);

		//set up the renderer so that any geometry within 640x480 space
		//can be prjected onto the pointcloud
		sharedRenderer->bindRenderer();
		
		//draw the points
		randomPoints.drawVertices();
				
		//draw the lines
		ofSetLineWidth(verticalScanlineThickness);
		ofSetColor(255*verticalScanlineAlpha);
		verticalScanLines.draw();
		
		ofSetLineWidth(horizontalScanlineThickness);
		horizontalScanLines.draw();
		ofSetColor(255*horizontalScanlineAlpha);
		
		sharedRenderer->unbindRenderer();
		
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		
		ofPopMatrix();
		ofPopStyle();
	}
}

void CloudsVisualSystemRGBD::selfExit(){
	
}

void CloudsVisualSystemRGBD::selfBegin(){

}

void CloudsVisualSystemRGBD::selfEnd(){
	
}

void CloudsVisualSystemRGBD::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){

	}
}

void CloudsVisualSystemRGBD::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemRGBD::selfMouseDragged(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfMouseMoved(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfMousePressed(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfMouseReleased(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfGuiEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupSystemGui(){
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::guiSystemEvent(ofxUIEventArgs &e){
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupRenderGui(){
	
	rdrGui->addSlider("POINTCLOUD SCALE", .001,  0.5, &pointcloudScale);
	rdrGui->addSlider("POINTCLOUD OFFSET",   0, -100, &pointcloudOffsetZ);

	rdrGui->addSlider("VERTICAL LINE SPACE", .5, 12, &scanlineSimplify.x);
	rdrGui->addSlider("HORIZONTAL LINE SPACE", .5, 12, &scanlineSimplify.y);
	
	rdrGui->addSlider("VERTICAL LINE ALPHA", 0, 1.0, &verticalScanlineAlpha);
	rdrGui->addSlider("HORIZONTAL LINE ALPHA", 0, 1.0, &horizontalScanlineAlpha);

	rdrGui->addSlider("VERTICAL LINE THICKNESS", 0, 2.0, &verticalScanlineThickness);
	rdrGui->addSlider("HORIZONTAL LINE THICKNESS", 0, 2.0, &horizontalScanlineThickness);

}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::guiRenderEvent(ofxUIEventArgs &e){
	
	cout << "GUI EVENT WITH WIDGET " << e.widget->getName();
	
	if(e.widget->getName() == "VERTICAL LINE SPACE"){
		refreshScanlineMesh = true;
	}
	else if(e.widget->getName() == "HORIZONTAL LINE SPACE"){
		refreshScanlineMesh = true;
	}
}

