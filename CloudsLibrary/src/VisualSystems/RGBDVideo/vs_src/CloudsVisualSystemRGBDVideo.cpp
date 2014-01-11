

#include "CloudsVisualSystemRGBDVideo.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
CloudsVisualSystemRGBDVideo::CloudsVisualSystemRGBDVideo(){
	videoPathField = NULL;
	movieLoaded = false;
}

//--------------------------------------------------------------
string CloudsVisualSystemRGBDVideo::getSystemName(){
	return "RGBDVideo";
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfSetDefaults(){
    pointscale = .25;
    pointShift = ofVec3f(0,0,0);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfSetup(){
	for(int y = 0; y < 480; y++){
		for(int x = 0; x < 640; x++){
			pointCloud.addVertex(ofVec3f(x,y,0));
		}
	}
	
	reloadShader();
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::reloadShader(){
	rgbdPixelToPixelShader.load(getVisualSystemDataPath() + "shaders/rgbdpixeltopixel");
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfBegin(){

}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfSetupGuis(){
	
	ofxUISuperCanvas *g = new ofxUISuperCanvas("VIDEO", gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
	
    g->setName("VideoSettings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);

    videoPathField = g->addTextInput("VideoPath", "");
	g->addButton("Load Video", false);
    
   	g->addSlider("Point Offset X", -400, 400, &pointShift.x);
   	g->addSlider("Point Offset Y", -400, 400, &pointShift.y);
	g->addSlider("Point Offset", -2000, 0, &pointShift.z);
    
//	g->addSlider("Point Scale", .1, 3.0, &pointscale);

    g->autoSizeToFitWidgets();

	ofAddListener(g->newGUIEvent, this, &CloudsVisualSystemRGBDVideo::selfGuiEvent);
	
    guis.push_back(g);
    guimap[g->getName()] = g;
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfUpdate(){
	
	if(loadMoviePath != ""){
		cout << "CloudsVisualSystemRGBDVideo::selfUpdate LOADING MOVIE PATH " << loadMoviePath << endl;
		if( playMovie(loadMoviePath) ){
			videoPathField->setTextString(loadMoviePath);
		}
		loadMoviePath = "";
	}
	
    cloudsCamera.lookTarget = ofVec3f(0,0,0);
    
	if(movieLoaded){
		player.update();
	}
}

void CloudsVisualSystemRGBDVideo::selfDrawBackground(){
}

void CloudsVisualSystemRGBDVideo::selfDrawDebug(){

}

void CloudsVisualSystemRGBDVideo::selfSceneTransformation(){
	
}

void CloudsVisualSystemRGBDVideo::selfDraw(){
	if(movieLoaded && player.isLoaded()){
				
		ofPushMatrix();

        setupRGBDTransforms();
		
		rgbdPixelToPixelShader.begin();
		rgbdPixelToPixelShader.setUniformTexture("texture", player.getTextureReference(), 1);
		rgbdPixelToPixelShader.setUniform2f("depthPP", videoIntrinsics.depthPP.x,videoIntrinsics.depthPP.y );
		rgbdPixelToPixelShader.setUniform2f("depthFOV", videoIntrinsics.depthFOV.x,videoIntrinsics.depthFOV.y );
		rgbdPixelToPixelShader.setUniform1f("minDepth", videoIntrinsics.depthRange.min);
		rgbdPixelToPixelShader.setUniform1f("maxDepth", videoIntrinsics.depthRange.max);
		
		rgbdPixelToPixelShader.setUniform1f("pointoffset", pointShift.z);
		
		rgbdPixelToPixelShader.setUniform1f("scale", 1.0);
		rgbdPixelToPixelShader.setUniform1f("offset", 0.0);
		
		pointCloud.drawVertices();
		
		rgbdPixelToPixelShader.end();
		
		ofPopMatrix();
	}
}

void CloudsVisualSystemRGBDVideo::selfExit(){
	
}

void CloudsVisualSystemRGBDVideo::selfPresetLoaded(string presetPath){
	loadMoviePath = videoPathField->getTextString();
}

bool CloudsVisualSystemRGBDVideo::playMovie(string filePath){
	movieLoaded = false;
	
	if( filePath == ""){
		ofLogError("CloudsVisualSystemRGBDVideo::playMovie") << "Path is blank" << endl;
		return false;
	}
	
	filePath = getVisualSystemDataPath(true) + "videos/" + filePath;
	
	if(! ofFile(filePath).exists() ) {
		ofLogError("CloudsVisualSystemRGBDVideo::playMovie") << "File does not exist" << endl;
		return false;
	}
	
	if(!player.loadMovie(filePath)){
		ofLogError("CloudsVisualSystemRGBDVideo::selfPresetLoaded") << "Video File " << filePath << " Failed to load";
		return false;
	}
	
	ofxXmlSettings intrinsicsXml;
	string xmlFilePath = ofFilePath::removeExt(filePath) + ".xml";
	if(!intrinsicsXml.loadFile(xmlFilePath)){
		ofLogError("CloudsVisualSystemRGBDVideo::selfPresetLoaded") << "XML File " << xmlFilePath << " Failed to load";
		return false;
	}
	
	videoIntrinsics.depthFOV.x = intrinsicsXml.getValue("depth:fovx", 0.);
	videoIntrinsics.depthFOV.y = intrinsicsXml.getValue("depth:fovy", 0.);
	videoIntrinsics.depthRange.min = intrinsicsXml.getValue("depth:minDepth", 0.);
	videoIntrinsics.depthRange.max = intrinsicsXml.getValue("depth:maxDepth", 0.);
	//TODO: read from xml
	videoIntrinsics.depthPP.x = 320;
	videoIntrinsics.depthPP.y = 240;
	player.play();
	movieLoaded = true;
	return true;
}

void CloudsVisualSystemRGBDVideo::selfEnd(){
}

void CloudsVisualSystemRGBDVideo::selfKeyPressed(ofKeyEventArgs & args){

}

void CloudsVisualSystemRGBDVideo::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemRGBDVideo::selfMouseDragged(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfMouseMoved(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfMousePressed(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfMouseReleased(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfSetupGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfGuiEvent(ofxUIEventArgs &e){
	
	if(e.widget->getName() == "Load Video"){
		
		ofxUIButton* button = (ofxUIButton*)e.widget;
		if(button->getValue()){
			ofFileDialogResult f = ofSystemLoadDialog();
			if(f.bSuccess){
				loadMoviePath = f.getName();
			}
		}
	}
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::selfSetupSystemGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::guiSystemEvent(ofxUIEventArgs &e){
}

void CloudsVisualSystemRGBDVideo::selfSetupRenderGui(){
//	rdrGui->addButton("regenerate", false);
//	rdrGui->addSlider("speed", 1, 20, &speed);
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBDVideo::guiRenderEvent(ofxUIEventArgs &e){

}

