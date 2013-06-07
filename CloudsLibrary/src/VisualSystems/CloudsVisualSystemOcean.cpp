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
	
	ocean.size = ofVec3f(oceanTileSizeX, 1.0, oceanTileSizeY);
    ocean.windSpeed = 32;
    ocean.setup();

	renderer.shaderLocation = "";
	renderer.setup(&ocean, 9, 9);
	
}

void CloudsVisualSystemOcean::selfSetupGuis(){
	
}

void CloudsVisualSystemOcean::selfUpdate(){
    ocean.waveSpeed = 15;
    ocean.waveScale = 10;
    ocean.choppyScale = 7;
	
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
	renderer.drawWireframe();

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
	
}

void CloudsVisualSystemOcean::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOcean::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemOcean::selfSetupRenderGui(){
	
}

void CloudsVisualSystemOcean::guiRenderEvent(ofxUIEventArgs &e){
	
}
