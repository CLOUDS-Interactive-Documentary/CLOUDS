
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
}

void CloudsVisualSystemRGBD::selfSetupGuis(){
	
}

void CloudsVisualSystemRGBD::selfUpdate(){
	cout << "Updating..." << endl;
}

void CloudsVisualSystemRGBD::selfDrawBackground(){
	
}

void CloudsVisualSystemRGBD::selfDrawDebug(){
	
}

void CloudsVisualSystemRGBD::selfSceneTransformation(){

}

void CloudsVisualSystemRGBD::selfDraw(){
	if(sharedRenderer != NULL && hasSpeaker){
		cout << "binding and drawing " << randomPoints.getNumVertices() << endl;
		sharedRenderer->bindRenderer();
		randomPoints.drawVertices();
		sharedRenderer->unbindRenderer();
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

void CloudsVisualSystemRGBD::selfSetupRenderGui(){
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::guiRenderEvent(ofxUIEventArgs &e){
	
}

