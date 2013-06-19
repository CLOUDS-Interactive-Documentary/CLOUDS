//
//  CloudsVisualSystemOcean.cpp
//  VSCloudsRezanator
//
//  Created by James George on 5/31/13.
//
//

#include "CloudsVisualSystemOcean.h"

CloudsVisualSystemOcean::CloudsVisualSystemOcean(){
	windSpeed = 32;
    oceanTileSizeX = 200;
    oceanTileSizeY = 200;
	
}

string CloudsVisualSystemOcean::getSystemName(){
	return "Ocean";
}

void CloudsVisualSystemOcean::selfSetup(){
	generateOcean();
	
}

void CloudsVisualSystemOcean::generateOcean(){
	ocean.size = ofVec3f(int(oceanTileSizeX), 1.0, int(oceanTileSizeY));
    ocean.windSpeed = windSpeed;
    ocean.setup();
	
	renderer.shaderLocation = "";
	renderer.setup(&ocean, 9, 9);

}

void CloudsVisualSystemOcean::selfSetupGuis(){
	
	oceanGui = new ofxUISuperCanvas("OCEAN", gui);
    oceanGui->copyCanvasStyle(gui);
    oceanGui->copyCanvasProperties(gui);
	
    oceanGui->setName("OceanSettings");
    oceanGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	oceanGui->addSlider("OCEAN SIZE X", 10, 1000, &oceanTileSizeX);
	oceanGui->addSlider("OCEAN SIZE Y", 10, 1000, &oceanTileSizeY);
	oceanGui->addSlider("WIND SPEED Y", 2, 1000, &windSpeed);
	
	oceanGui->addButton("REGENERATE", &shouldRegenerateOcean);
	
	oceanGui->addSlider("WAVE SPEED", 1, 10, &ocean.waveSpeed);
	oceanGui->addSlider("WAVE SCALE", 0, 100.0, &ocean.waveScale);
	oceanGui->addSlider("WAVE CHOPPINESS", 0, 20, &ocean.choppyScale);
		
	ofAddListener(oceanGui->newGUIEvent, this, &CloudsVisualSystemOcean::selfGuiEvent);
	
    guis.push_back(oceanGui);
    guimap[oceanGui->getName()] = oceanGui;
}

void CloudsVisualSystemOcean::selfUpdate(){

	ocean.setFrameNum(ofGetFrameNum());
    ocean.update();
	renderer.update();
	
}

void CloudsVisualSystemOcean::selfDrawBackground(){
	
}

void CloudsVisualSystemOcean::selfDrawDebug(){
	
}

void CloudsVisualSystemOcean::selfSceneTransformation(){
	
}

void CloudsVisualSystemOcean::selfDraw(){
//	renderer.drawWireframe();
	renderer.draw();
}

void CloudsVisualSystemOcean::selfExit(){
	
}

void CloudsVisualSystemOcean::selfBegin(){
	
}

void CloudsVisualSystemOcean::selfEnd(){
	
}

void CloudsVisualSystemOcean::selfKeyPressed(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOcean::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOcean::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOcean::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOcean::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOcean::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOcean::selfSetupGui(){
	
}

void CloudsVisualSystemOcean::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "REGENERATE" && ((ofxUIButton*)e.widget)->getValue() ){
		generateOcean();
	}
	
}

void CloudsVisualSystemOcean::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOcean::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemOcean::selfSetupRenderGui(){
	
}

void CloudsVisualSystemOcean::guiRenderEvent(ofxUIEventArgs &e){
	
}
