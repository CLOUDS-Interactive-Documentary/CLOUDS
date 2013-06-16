
#include "CloudsVisualSystemSeeingDark.h"

CloudsVisualSystemSeeingDark::CloudsVisualSystemSeeingDark(){
	scanlineSimplify = ofVec2f(2,2);
}

string CloudsVisualSystemSeeingDark::getSystemName(){
	return "SeeingDark";
}


void CloudsVisualSystemSeeingDark::selfSetup(){
	renderer.setShaderPath(getDataPath() + "shaders/rgbdcombined");
	generateScanlines();
}

void CloudsVisualSystemSeeingDark::loadMovieForComposition(string videoPath){

	if(renderer.setup(videoPath, ofFilePath::removeExt(videoPath)+".xml")){
		fileNameInput->setTextString(ofFilePath::getFileName(videoPath));
		timeline->getVideoTrack("Video")->setPlayer(renderer.getSharedPlayerPtr());
		timeline->setTimecontrolTrack("Video");
		timeline->setFrameRate(renderer.getPlayer().getTotalNumFrames() / renderer.getPlayer().getDuration());
		timeline->setDurationInSeconds(renderer.getPlayer().getDuration());
//		renderer.getPlayer().play();
	}
}

void CloudsVisualSystemSeeingDark::selfSetupGuis(){
	
    composeGui = new ofxUISuperCanvas("EXAMPLE CUSTOM", gui);
    composeGui->copyCanvasStyle(gui);
    composeGui->copyCanvasProperties(gui);
	
    composeGui->setName("COMPOSE");
    composeGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	//EXAMPLES
	fileNameInput = composeGui->addTextInput("FILENAME", "");
	composeGui->addButton("LOAD", false);
	composeGui->addToggle("BEGINNING", &beginning);
	composeGui->addToggle("MIDDLE", &middle);
	composeGui->addToggle("END", &end);
	composeGui->addSpacer();
	composeGui->addSlider("NEAR PLANE", 100, 4000, &nearPlane);
	composeGui->addSlider("FAR PLANE",  100, 4000, &farPlane);
	composeGui->addSlider("SCALE",  0, .5, &scale);
	composeGui->addSpacer();
	composeGui->addSlider("OFFSET X COURSE", -100, 100, &centerOffsetCourse.x);
	composeGui->addSlider("OFFSET Y COURSE", -100, 100, &centerOffsetCourse.y);
	composeGui->addSlider("OFFSET Z COURSE", -100, 100, &centerOffsetCourse.z);
	composeGui->addSlider("OFFSET X FINE", -10, 10, &centerOffsetFine.x);
	composeGui->addSlider("OFFSET Y FINE", -10, 10, &centerOffsetFine.y);
	composeGui->addSlider("OFFSET Z FINE", -10, 10, &centerOffsetFine.z);
	composeGui->addSpacer();
	composeGui->addSlider("VERTICAL LINE SPACE", .5, 12, &scanlineSimplify.x);
	composeGui->addSlider("HORIZONTAL LINE SPACE", .5, 12, &scanlineSimplify.y);
	
	ofAddListener(composeGui->newGUIEvent, this, &CloudsVisualSystemSeeingDark::selfGuiEvent);
	
    guis.push_back(composeGui);
    guimap[composeGui->getName()] = composeGui;
}

void CloudsVisualSystemSeeingDark::generateScanlines(){
		
	verticalScanLines.clear();
	horizontalScanLines.clear();
	
	if(scanlineSimplify.x <= 0) scanlineSimplify.x = 1;
	if(scanlineSimplify.y <= 0) scanlineSimplify.y = 1;
	
	int height = 480;
	int width = 640;
	
	//VERTICAL
	for (float xstep = 0; xstep <= width-scanlineSimplify.x; xstep += scanlineSimplify.x){
		for (float ystep = 0; ystep <= height-scanlineSimplify.y; ystep += scanlineSimplify.y){
			verticalScanLines.addVertex( ofVec3f(xstep, ystep, 0) );
			verticalScanLines.addVertex( ofVec3f(xstep, ystep+scanlineSimplify.y, 0) );
		}
	}
	
	//HORIZONTAL
	for (float ystep = 0; ystep <= height-scanlineSimplify.y; ystep += scanlineSimplify.y){
		for (float xstep = 0; xstep <= width-scanlineSimplify.x; xstep += scanlineSimplify.x){
			
//			float ystepOffset = ofRandom(-scanlineSimplify.y/4,scanlineSimplify.y/4);
			
			horizontalScanLines.addColor(ofFloatColor(ofRandom(1.)));
			horizontalScanLines.addVertex( ofVec3f(xstep, ystep, 0) );
			
			horizontalScanLines.addColor(ofFloatColor(ofRandom(1.)));
			horizontalScanLines.addVertex( ofVec3f(xstep+scanlineSimplify.x, ystep, 0) );
		}
	}
	
	verticalScanLines.setMode( OF_PRIMITIVE_LINES );
	horizontalScanLines.setMode( OF_PRIMITIVE_LINES );
	
	refreshScanlineMesh = false;
}


void CloudsVisualSystemSeeingDark::selfUpdate(){
	if(renderer.getPlayer().isLoaded()){
		renderer.farClip = farPlane;
		renderer.nearClip = nearPlane;
		renderer.update();
	}
	if(refreshScanlineMesh){
		generateScanlines();
	}
}

void CloudsVisualSystemSeeingDark::selfDrawBackground(){

}

void CloudsVisualSystemSeeingDark::selfDrawDebug(){
	ofPushStyle();
	ofSetColor(255, 255, 0, 100);
	ofDrawSphere(0, 0, 2);
	ofPopStyle();
}

void CloudsVisualSystemSeeingDark::selfSceneTransformation(){

}

void CloudsVisualSystemSeeingDark::selfDraw(){
	//draw the lines
	if(renderer.getPlayer().isLoaded()){
		
		ofPushMatrix();
		
		ofTranslate(centerOffsetCourse + centerOffsetFine);
		ofScale(scale,scale,scale);
		
		renderer.setSimplification(scanlineSimplify);
		renderer.bindRenderer();
		
		renderer.getShader().setUniform1f("flowPosition", 0);
		renderer.getShader().setUniform1f("baseMultiplier", 1.0);
		
		ofSetColor(255);
		ofSetLineWidth(2);
		horizontalScanLines.draw();
		
		ofSetLineWidth(2);
		ofSetColor(255);
		verticalScanLines.draw();
		
		renderer.unbindRenderer();
		
		ofPopMatrix();
	}

}

void CloudsVisualSystemSeeingDark::selfExit(){

}

void CloudsVisualSystemSeeingDark::selfBegin(){

}

void CloudsVisualSystemSeeingDark::selfEnd(){

}


void CloudsVisualSystemSeeingDark::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == ' ' && renderer.getPlayer().isLoaded()) renderer.getPlayer().play();
}

void CloudsVisualSystemSeeingDark::selfKeyReleased(ofKeyEventArgs & args){

}

void CloudsVisualSystemSeeingDark::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystemSeeingDark::selfMouseMoved(ofMouseEventArgs& data){

}

void CloudsVisualSystemSeeingDark::selfMousePressed(ofMouseEventArgs& data){

}

void CloudsVisualSystemSeeingDark::selfMouseReleased(ofMouseEventArgs& data){

}

void CloudsVisualSystemSeeingDark::selfSetupGui(){

}

void CloudsVisualSystemSeeingDark::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "VERTICAL LINE SPACE"){
		refreshScanlineMesh = true;
	}
	else if(e.widget->getName() == "HORIZONTAL LINE SPACE"){
		refreshScanlineMesh = true;
	}
	else if(e.widget->getName() == "LOAD" && ((ofxUIButton*)e.widget)->getValue()){
		loadMovieForComposition( getVisualSystemDataPath() + "_videos/" + fileNameInput->getTextString() );
//		if(renderer.setup(,
//						  ofFilePath::removeExt(fileNameInput->getTextString())+".xml")){
//			renderer.getPlayer().play();
//		}
	}
}


void CloudsVisualSystemSeeingDark::selfSetupSystemGui(){

}

void CloudsVisualSystemSeeingDark::guiSystemEvent(ofxUIEventArgs &e){

}

void CloudsVisualSystemSeeingDark::selfSetupRenderGui(){

}

void CloudsVisualSystemSeeingDark::selfSetupTimeline(){
	timeline->addVideoTrack("Video");
}

void CloudsVisualSystemSeeingDark::guiRenderEvent(ofxUIEventArgs &e){
	
}