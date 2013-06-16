
#include "CloudsVisualSystemSeeingDark.h"

CloudsVisualSystemSeeingDark::CloudsVisualSystemSeeingDark(){
	scanlineSimplify = ofVec2f(2,2);
}

string CloudsVisualSystemSeeingDark::getSystemName(){
	return "SeeingDark";
}


void CloudsVisualSystemSeeingDark::selfSetup(){
	renderer.setShaderPath("../../../CloudsData/shaders/rgbdcombined");
	generateScanlines();
}

void CloudsVisualSystemSeeingDark::loadMovieForComposition(string videoPath){
//	if(timeline->getVideoTrack("Video")->load(videoPath)){
//		fileNameInput->setTextString(videoPath);
//	}

	if(renderer.setup(videoPath, ofFilePath::removeExt(videoPath)+".xml")){
		fileNameInput->setTextString(videoPath);
		renderer.getPlayer().play();
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
	composeGui->addSpacer();
	composeGui->addSlider("OFFSET X COURSE", -1000, 1000, &centerOffsetCourse.x);
	composeGui->addSlider("OFFSET Y COURSE", -1000, 1000, &centerOffsetCourse.y);
	composeGui->addSlider("OFFSET Z COURSE", -1000, 1000, &centerOffsetCourse.z);
	composeGui->addSlider("OFFSET X FINE", -100, 100, &centerOffsetFine.x);
	composeGui->addSlider("OFFSET Y FINE", -100, 100, &centerOffsetFine.y);
	composeGui->addSlider("OFFSET Z FINE", -100, 100, &centerOffsetFine.z);
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
			
			float ystepOffset = ofRandom(-scanlineSimplify.y/4,scanlineSimplify.y/4);
			
			horizontalScanLines.addColor(ofFloatColor(ofRandom(1.)));
			horizontalScanLines.addVertex( ofVec3f(xstep, ystep+ystepOffset, 0) );
			
			horizontalScanLines.addColor(ofFloatColor(ofRandom(1.)));
			horizontalScanLines.addVertex( ofVec3f(xstep+scanlineSimplify.x, ystep+ystepOffset, 0) );
		}
	}
	
	verticalScanLines.setMode( OF_PRIMITIVE_LINES );
	horizontalScanLines.setMode( OF_PRIMITIVE_LINES );
	
	refreshScanlineMesh = false;
}


void CloudsVisualSystemSeeingDark::selfUpdate(){
	if(renderer.getPlayer().isLoaded()){
		renderer.update();
	}
	if(refreshScanlineMesh){
		generateScanlines();
	}
}

void CloudsVisualSystemSeeingDark::selfDrawBackground(){

}

void CloudsVisualSystemSeeingDark::selfDrawDebug(){

}

void CloudsVisualSystemSeeingDark::selfSceneTransformation(){

}

void CloudsVisualSystemSeeingDark::selfDraw(){
	//draw the lines
	if(renderer.getPlayer().isLoaded()){
		
		ofPushMatrix();
		
		ofTranslate(0,0,-224);
		ofScale(.25,.25,.25);
		
		renderer.setSimplification(scanlineSimplify);
		renderer.bindRenderer();
		
		renderer.getShader().setUniform1f("flowPosition", 0);
		
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
		if(renderer.setup(fileNameInput->getTextString(),
						  ofFilePath::removeExt(fileNameInput->getTextString())+".xml")){
			renderer.getPlayer().play();
		}
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