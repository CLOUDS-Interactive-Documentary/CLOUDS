
#include "CloudsVisualSystemYellowTail.h"

CloudsVisualSystemYellowTail::CloudsVisualSystemYellowTail(){

}

string CloudsVisualSystemYellowTail::getSystemName(){
	return "YellowTail";
}

void CloudsVisualSystemYellowTail::selfSetup(){
	//normal set up
}

void CloudsVisualSystemYellowTail::selfSetupGuis(){
	
    customGui = new ofxUISuperCanvas("CUSTOM", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
    customGui->setName("CustomSettings");
    customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("SOME PARAM", 0, 100, &someParam);
	customGui->addButton("SOME BUTTON", false);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemYellowTail::selfGuiEvent);
	
    guis.push_back(customGui);
    guimap[customGui->getName()] = customGui;
	
}

void CloudsVisualSystemYellowTail::selfPresetLoaded(string presetPath){
	
}

void CloudsVisualSystemYellowTail::selfUpdate(){


}

void CloudsVisualSystemYellowTail::selfDrawBackground(){

}

void CloudsVisualSystemYellowTail::selfDrawDebug(){
	
}

void CloudsVisualSystemYellowTail::selfSceneTransformation(){
	
}

void CloudsVisualSystemYellowTail::selfDraw(){


}

void CloudsVisualSystemYellowTail::selfExit(){
	
}

void CloudsVisualSystemYellowTail::selfBegin(){
	//This shoudl be quick, just means it started laying
}

void CloudsVisualSystemYellowTail::selfEnd(){
	//CLEAR ALL MEMORY
}

void CloudsVisualSystemYellowTail::selfKeyPressed(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemYellowTail::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemYellowTail::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemYellowTail::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemYellowTail::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemYellowTail::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemYellowTail::selfSetupGui(){

	
}

void CloudsVisualSystemYellowTail::selfGuiEvent(ofxUIEventArgs &e){

	if(e.widget->getName() == "SOME BUTTON" && ((ofxUIButton*)e.widget)->getValue() ){
		//do buttony stuff
	}
}

void CloudsVisualSystemYellowTail::selfSetupSystemGui(){
	
}

void CloudsVisualSystemYellowTail::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemYellowTail::selfSetupRenderGui(){
	
}

void CloudsVisualSystemYellowTail::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemYellowTail::selfSetupTimeline(){
	
}

void CloudsVisualSystemYellowTail::selfSetupTimelineGui(){
	
}

void CloudsVisualSystemYellowTail::selfTimelineGuiEvent(ofxUIEventArgs &e){
	
}
