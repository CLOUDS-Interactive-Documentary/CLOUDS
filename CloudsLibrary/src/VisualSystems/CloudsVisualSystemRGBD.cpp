
#include "CloudsVisualSystemRGBD.h"

//--------------------------------------------------------------
string CloudsVisualSystemRGBD::getSystemName(){
	return "RGBD";
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetup(){

	generatePointGrid();
	generateScanlines();
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupGuis(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfUpdate(){
	if(refreshScanlineMesh){
		generateScanlines();
	}
	if(refreshPointcloud){
		generatePointGrid();
	}
	if(numRandomPoints != randomPoints.getNumVertices()){
		generateRandomPoints();
	}
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::generatePointGrid(){
	
	pointGrid.clear();
	
	pointVerticalSpace = MAX(pointVerticalSpace, 1);
	pointHorizontalSpace = MAX(pointHorizontalSpace, 1);
	for(float y = 0; y < 480; y+= pointVerticalSpace){
		for(float x = 0; x < 640; x+= pointHorizontalSpace){
			pointGrid.addVertex(ofVec3f(x,y,0));
		}
	}
	pointGrid.setMode(OF_PRIMITIVE_POINTS);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::generateRandomPoints(){
	if(numRandomPoints == 0){
		randomPoints.clear();
	}
	else if(numRandomPoints < randomPoints.getNumVertices() ){
		randomPoints.getVertices().erase(randomPoints.getVertices().begin(), randomPoints.getVertices().begin() + (randomPoints.getNumVertices() - numRandomPoints) );
	}
	while(numRandomPoints > randomPoints.getNumVertices()){
		randomPoints.addVertex( ofVec3f(ofRandom(640),ofRandom(480),0) );
	}
	
	randomPoints.setMode(OF_PRIMITIVE_POINTS);
}

//--------------------------------------------------------------
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

	verticalScanLines.setMode( OF_PRIMITIVE_LINES );
	horizontalScanLines.setMode( OF_PRIMITIVE_LINES );
	
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
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		
		//Enable smooth lines and screen blending
		glDisable(GL_DEPTH_TEST);
		
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		
		ofEnableAlphaBlending();
		ofEnableBlendMode(OF_BLENDMODE_SCREEN);
		
		//move the pointcloud
		ofTranslate(0,0,pointcloudOffsetZ);
		ofScale(pointcloudScale,pointcloudScale,pointcloudScale);

		//set up the renderer so that any geometry within 640x480 space
		//can be prjected onto the pointcloud
		sharedRenderer->bindRenderer();
		
		//draw the points
		glPointSize(pointSizeMin);
		randomPoints.draw();
		pointGrid.draw();
		
		//draw the lines
		ofSetLineWidth(verticalScanlineThickness);
		ofSetColor(255*verticalScanlineAlpha, 255*verticalScanlineAlpha);
		verticalScanLines.draw();
		
		ofSetColor(255*verticalScanlineAlpha, 255*verticalScanlineAlpha);
		ofSetLineWidth(horizontalScanlineThickness);
		horizontalScanLines.draw();
		
		sharedRenderer->unbindRenderer();
		
		glPopAttrib();
		
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
	
	rdrGui->addSlider("CLOUD SCALE", .001,  0.5, &pointcloudScale);
	rdrGui->addSlider("CLOUD OFFSET",   0, -800, &pointcloudOffsetZ);

	rdrGui->addSlider("POINTSIZE MIN", 0, 3, &pointSizeMin);
	rdrGui->addSlider("POINTSIZE MAX", 0, 3, &pointSizeMax);

	rdrGui->addSlider("POINTGRID VERTICAL SPACE", 1, 10, &pointVerticalSpace);
	rdrGui->addSlider("POINTGRID HORIZONTAL SPACE", 1, 10, &pointHorizontalSpace);

	rdrGui->addSlider("NUM RANDOM POINTS", 0, 50000.0f, &numRandomPoints);
	
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
	else if(e.widget->getName() == "POINTGRID VERTICAL SPACE"){
		refreshPointcloud = true;
	}
	else if(e.widget->getName() == "POINTGRID HORIZONTAL SPACE"){
		refreshPointcloud = true;
	}
}

