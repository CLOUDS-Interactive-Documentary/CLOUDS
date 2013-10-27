//
//  CloudsVisualSystemOpenP5Spaghetti .cpp
//

#include "CloudsVisualSystemOpenP5Spaghetti.h"


//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOpenP5Spaghetti::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
//	customGui->addSlider("Color 1 Hue", 0, 255, &color1HSB.r);
//	customGui->addSlider("Color 1 Sat", 0, 255, &color1HSB.g);
//	customGui->addSlider("Color 1 Bri", 0, 255, &color1HSB.b);
//
//	customGui->addSlider("Color 2 Hue", 0, 255, &color2HSB.r);
//	customGui->addSlider("Color 2 Sat", 0, 255, &color2HSB.g);
//	customGui->addSlider("Color 2 Bri", 0, 255, &color2HSB.b);
    
    customGui->addLabel("Solid Sphere");
    customGui->addSlider("Solid_Sphere_Scale", 0.0, .25, &solidSphereScale);
    customGui->addSlider("Solid_Sphere_Alpha", 0.0, 1.0, &solidSphereAlpha);
    customGui->addSlider("Noise Speed", 0.0, 10.0, &noiseSpeed);
    customGui->addSlider("Noise Scale", 0.0, 4.0, &noiseScale);
    
//	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
//	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
//	customGui->addButton("Custom Button", false);
//	customGui->addToggle("Custom Toggle", &customToggle);
	
	
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5Spaghetti::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemOpenP5Spaghetti::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5Spaghetti::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5Spaghetti::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5Spaghetti::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5Spaghetti::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemOpenP5Spaghetti::selfSetup(){

}


// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5Spaghetti::selfPresetLoaded(string presetPath){

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5Spaghetti::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5Spaghetti::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOpenP5Spaghetti::selfUpdate(){
    
    for(int i = 0; i<NWALKERS; i++){
        
        walkers[i].step();
        
    }

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOpenP5Spaghetti::selfDraw(){
    int idx = 0;
    
    for(int i = 0; i<NWALKERS; i++){
        walkers[i].draw();
    }
}

// draw any debug stuff here
void CloudsVisualSystemOpenP5Spaghetti::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOpenP5Spaghetti::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5Spaghetti::selfEnd(){

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOpenP5Spaghetti::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5Spaghetti::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5Spaghetti::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5Spaghetti::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Spaghetti::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Spaghetti::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Spaghetti::selfMouseReleased(ofMouseEventArgs& data){
	
}