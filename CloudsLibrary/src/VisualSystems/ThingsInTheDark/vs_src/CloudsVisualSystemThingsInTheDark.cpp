
#include "CloudsVisualSystemThingsInTheDark.h"
#include "CloudsGlobal.h"

CloudsVisualSystemThingsInTheDark::CloudsVisualSystemThingsInTheDark(){
	scanlineSimplify = ofVec2f(2,2);
}

string CloudsVisualSystemThingsInTheDark::getSystemName(){
	return "ThingsInTheDark";
}

void CloudsVisualSystemThingsInTheDark::selfSetup(){
	//renderer.setShaderPath(getDataPath() + "shaders/rgbdcombined");
	generateScanlines();
}

void CloudsVisualSystemThingsInTheDark::loadMovieForComposition(string videoPath){

	if(renderer.setup(videoPath, ofFilePath::removeExt(videoPath)+".xml")){
		fileNameInput->setTextString(ofFilePath::getFileName(videoPath));
		timeline->getVideoTrack("Video")->setPlayer(renderer.getSharedPlayerPtr());
		timeline->setTimecontrolTrack("Video");
		timeline->setFrameRate(renderer.getPlayer().getTotalNumFrames() / renderer.getPlayer().getDuration());
		timeline->setDurationInSeconds(renderer.getPlayer().getDuration());
//		renderer.getPlayer().play();
	}
}

void CloudsVisualSystemThingsInTheDark::selfSetupGuis(){
	
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
	
	ofAddListener(composeGui->newGUIEvent, this, &CloudsVisualSystemThingsInTheDark::selfGuiEvent);
	
    guis.push_back(composeGui);
    guimap[composeGui->getName()] = composeGui;
}

void CloudsVisualSystemThingsInTheDark::generateScanlines(){
		
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


void CloudsVisualSystemThingsInTheDark::selfUpdate(){
	if(renderer.getPlayer().isLoaded()){
		renderer.farClip = farPlane;
		renderer.nearClip = nearPlane;
		//COMMENTED
//		renderer.update();
	}
	if(refreshScanlineMesh){
		generateScanlines();
	}
}

void CloudsVisualSystemThingsInTheDark::selfDrawBackground(){

}

void CloudsVisualSystemThingsInTheDark::selfDrawDebug(){
	ofPushStyle();
	ofSetColor(255, 255, 0, 100);
	ofSphere(0, 0, 2);
	ofPopStyle();
}

void CloudsVisualSystemThingsInTheDark::selfSceneTransformation(){

}

void CloudsVisualSystemThingsInTheDark::selfDraw(){
	//draw the lines
	if(renderer.getPlayer().isLoaded()){
		
		ofPushMatrix();
		
		ofTranslate(centerOffsetCourse + centerOffsetFine);
		ofScale(scale,scale,scale);
		
//JG COMMENTED
//		renderer.setSimplification(scanlineSimplify);
//		renderer.bindRenderer();
//JG COMMENTED
//		renderer.getShader().setUniform1f("flowPosition", 0);
//		renderer.getShader().setUniform1f("baseMultiplier", 1.0);
		
		ofSetColor(255);
		ofSetLineWidth(2);
		horizontalScanLines.draw();
		
		ofSetLineWidth(2);
		ofSetColor(255);
		verticalScanLines.draw();
//JG COMMENTED		
//		renderer.unbindRenderer();
		
		ofPopMatrix();
	}

}

void CloudsVisualSystemThingsInTheDark::selfExit(){

}

void CloudsVisualSystemThingsInTheDark::selfBegin(){

}

void CloudsVisualSystemThingsInTheDark::selfEnd(){

}


void CloudsVisualSystemThingsInTheDark::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == ' ' && renderer.getPlayer().isLoaded()) renderer.getPlayer().play();
}

void CloudsVisualSystemThingsInTheDark::selfKeyReleased(ofKeyEventArgs & args){

}

void CloudsVisualSystemThingsInTheDark::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystemThingsInTheDark::selfMouseMoved(ofMouseEventArgs& data){

}

void CloudsVisualSystemThingsInTheDark::selfMousePressed(ofMouseEventArgs& data){

}

void CloudsVisualSystemThingsInTheDark::selfMouseReleased(ofMouseEventArgs& data){

}

void CloudsVisualSystemThingsInTheDark::selfSetupGui(){

}

void CloudsVisualSystemThingsInTheDark::selfGuiEvent(ofxUIEventArgs &e){
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


void CloudsVisualSystemThingsInTheDark::selfSetupSystemGui(){

}

void CloudsVisualSystemThingsInTheDark::guiSystemEvent(ofxUIEventArgs &e){

}

void CloudsVisualSystemThingsInTheDark::selfSetupRenderGui(){

}

void CloudsVisualSystemThingsInTheDark::selfSetupTimeline(){
	timeline->addVideoTrack("Video");
}

void CloudsVisualSystemThingsInTheDark::guiRenderEvent(ofxUIEventArgs &e){
	
}