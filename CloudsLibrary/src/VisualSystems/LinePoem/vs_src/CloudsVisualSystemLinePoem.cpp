//
//  CloudsVisualSystemLinePoem.cpp
//

#include "CloudsVisualSystemLinePoem.h"


//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemLinePoem::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
	customGui->addButton("Custom Button", false);
	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemLinePoem::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemLinePoem::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

void CloudsVisualSystemLinePoem::setupFbo(){
    //ZACH: instantiate your FBO here
    fbo.allocate(getCanvasWidth(), getCanvasHeight(), GL_RGB);
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemLinePoem::selfSetupSystemGui(){
	
}

void CloudsVisualSystemLinePoem::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemLinePoem::selfSetupRenderGui(){

}

void CloudsVisualSystemLinePoem::guiRenderEvent(ofxUIEventArgs &e){
	
}

//This is called whenever a new preset is loaded, before selfSetup()
//use it to ensure all your simple variables are initialized to an
//acceptable default state
void CloudsVisualSystemLinePoem::selfSetDefaults(){

}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemLinePoem::selfSetup(){
	
//    APP.SYS = this;
//    APP.setup();
    //setupFbo();
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemLinePoem::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemLinePoem::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemLinePoem::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemLinePoem::selfUpdate(){
//    if(fbo.getWidth() != getCanvasWidth() ||
//       fbo.getHeight() != getCanvasHeight())
//    {
//        setupFbo();
//    }
    
    
 //   APP.update();

    
    
    
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemLinePoem::selfDraw(){
	
}

// draw any debug stuff here
void CloudsVisualSystemLinePoem::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemLinePoem::selfDrawBackground(){

//    APP.draw();
	
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemLinePoem::selfEnd(){	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemLinePoem::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemLinePoem::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemLinePoem::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemLinePoem::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemLinePoem::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemLinePoem::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemLinePoem::selfMouseReleased(ofMouseEventArgs& data){
	
}