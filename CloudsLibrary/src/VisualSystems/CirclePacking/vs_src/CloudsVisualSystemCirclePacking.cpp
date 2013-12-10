//
//  CloudsVisualSystemCirclePacking.cpp
//



#include "CloudsVisualSystemCirclePacking.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//float CloudsVisualSystemCirclePacking::NCircles = 50;

//These methods let us add custom GUI parameters and respond to their events

void CloudsVisualSystemCirclePacking::selfSetupGui(){

	/*
    customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
	customGui->addButton("Custom Button", false);
	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemCirclePacking::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
     
     */
    
    
    //pack(400,400);
    
    CirclePacker pack(400.0, 400.0);
    
    for(int i = 0; i<50; i++){
      
        Circle newCircle(ofRandom(300.0), ofRandom(300.0), ofRandom(100.0), "blank");
      //  newCircle
        pack.circles.push_back(newCircle);
        
    }
}

void CloudsVisualSystemCirclePacking::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemCirclePacking::selfSetupSystemGui(){
	
}

void CloudsVisualSystemCirclePacking::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemCirclePacking::selfSetupRenderGui(){

}

void CloudsVisualSystemCirclePacking::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemCirclePacking::selfSetup(){
	
	
	
//	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemCirclePacking::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemCirclePacking::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemCirclePacking::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemCirclePacking::selfUpdate(){
    
   pack.update();

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemCirclePacking::selfDraw(){
	
   

	
}

// draw any debug stuff here
void CloudsVisualSystemCirclePacking::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemCirclePacking::selfDrawBackground(){

    //for (int i = 0; i<pack.circles.size(); i++){
       // cout << "circle radius : " << pack.circles[i].r << endl;
   // }
    
    cout << "circle vector size : " << pack.circles.size() << endl;
    pack.draw();
	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemCirclePacking::selfEnd(){
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemCirclePacking::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemCirclePacking::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemCirclePacking::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemCirclePacking::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCirclePacking::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCirclePacking::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCirclePacking::selfMouseReleased(ofMouseEventArgs& data){
	
}