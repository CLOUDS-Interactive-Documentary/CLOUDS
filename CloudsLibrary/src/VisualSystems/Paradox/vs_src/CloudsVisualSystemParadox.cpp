#include "CloudsVisualSystemParadox.h"

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemParadox::selfSetupGui(){

	paradoxGui = new ofxUISuperCanvas("VOXELS", gui);
	paradoxGui->copyCanvasStyle(gui);
	paradoxGui->copyCanvasProperties(gui);
	paradoxGui->setName("Voxels");
	paradoxGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

	vm.addGui(paradoxGui);
	
//	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
//	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
//	customGui->addButton("Custom Button", false);
//	customGui->addToggle("Custom Toggle", &customToggle);

	ofAddListener(paradoxGui->newGUIEvent, this, &CloudsVisualSystemParadox::selfGuiEvent);
	guis.push_back(paradoxGui);
	guimap[paradoxGui->getName()] = paradoxGui;
	
}

void CloudsVisualSystemParadox::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemParadox::selfSetupSystemGui(){
	
}

void CloudsVisualSystemParadox::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemParadox::selfSetupRenderGui(){

}

void CloudsVisualSystemParadox::guiRenderEvent(ofxUIEventArgs &e){
	
}

//This is called whenever a new preset is loaded, before selfSetup()
//use it to ensure all your simple variables are initialized to an
//acceptable default state
void CloudsVisualSystemParadox::selfSetDefaults(){
	vm.setDefaults();
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemParadox::selfSetup(){
	vm.setup();
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemParadox::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemParadox::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemParadox::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemParadox::selfUpdate(){
	vm.center = ofVec3f();
	vm.update();

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemParadox::selfDraw(){
	vm.draw();
}

// draw any debug stuff here
void CloudsVisualSystemParadox::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemParadox::selfDrawBackground(){
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemParadox::selfEnd(){	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemParadox::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemParadox::selfKeyPressed(ofKeyEventArgs & args){
	vm.reloadShaders();
}
void CloudsVisualSystemParadox::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemParadox::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemParadox::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemParadox::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemParadox::selfMouseReleased(ofMouseEventArgs& data){
	
}