//
//  CloudsVisualSystemOpenP5SpaceJunk.cpp
//

#include "CloudsVisualSystemOpenP5SpaceJunk.h"


//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOpenP5SpaceJunk::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    customGui->addButton("REGENERATE", &shouldRegenerate);
	
    customGui->addSlider("Rotation Speed", 0, 25, &speed_);
    customGui->addSlider("X Scale Min", 0, 50, &XrandMin);
    customGui->addSlider("X Scale Max", 0, 50, &XrandMax);
    customGui->addSlider("Y Scale Min", 0, 50, &YrandMin);
    customGui->addSlider("Y Scale Max", 0, 50, &YrandMax);
    customGui->addSlider("Z Scale Min", 0, 50, &ZrandMin);
    customGui->addSlider("Z Scale Max", 0, 50, &ZrandMax);
    customGui->addSlider("X Rotate Min", 0, 90, &XrotMin);
    customGui->addSlider("X Rotate Max", 0, 90, &XrotMax);
    customGui->addSlider("Y Rotate Min", 0, 90, &YrotMin);
    customGui->addSlider("Y Rotate Max", 0, 90, &YrotMax);
    customGui->addSlider("Z Rotate Min", 0, 90, &ZrotMin);
    customGui->addSlider("Z Rotate Max", 0, 90, &ZrotMax);
    
	customGui->addSlider("Color 1 Hue", 0, 255, &color1HSB.r);
	customGui->addSlider("Color 1 Sat", 0, 255, &color1HSB.g);
	customGui->addSlider("Color 1 Bri", 0, 255, &color1HSB.b);

	customGui->addSlider("Color 2 Hue", 0, 255, &color2HSB.r);
	customGui->addSlider("Color 2 Sat", 0, 255, &color2HSB.g);
	customGui->addSlider("Color 2 Bri", 0, 255, &color2HSB.b);

    
//	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
//	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
//	customGui->addButton("Custom Button", false);
//	customGui->addToggle("Custom Toggle", &customToggle);
	
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5SpaceJunk::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemOpenP5SpaceJunk::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
    if(e.widget->getName() == "REGENERATE" && ((ofxUIButton*)e.widget)->getValue() ){
		selfBegin();
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5SpaceJunk::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5SpaceJunk::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5SpaceJunk::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5SpaceJunk::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemOpenP5SpaceJunk::selfSetup(){
    
    
    ang = 0;
    
    
    
	for (int i=0; i<limit; i++) {
     //   Cube.ofColor(color1);
		list.push_back( Cube(ofRandom(XrandMin, XrandMax), ofRandom(YrandMin, YrandMax), ofRandom(ZrandMin, ZrandMax),  // 4,20 4,20, 2,20
                             ofRandom(-140, 140), ofRandom(-140, 140), ofRandom(-140, 140),
                             ofRandom(XrotMin, XrotMax), ofRandom(YrotMin, YrotMax), ofRandom(ZrotMin, ZrotMax) ) );
	}
	


}


// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5SpaceJunk::selfPresetLoaded(string presetPath){

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5SpaceJunk::selfBegin(){
    
    list.clear();
    
    for (int i=0; i<limit; i++) {
        //Cube.ofColor(color1);
		list.push_back( Cube(ofRandom(XrandMin, XrandMax), ofRandom(YrandMin, YrandMax), ofRandom(ZrandMin, ZrandMax),  // 4,20 4,20, 2,20
                             ofRandom(-140, 140), ofRandom(-140, 140), ofRandom(-140, 140),
                             ofRandom(XrotMin, XrotMax), ofRandom(YrotMin, YrotMax), ofRandom(ZrotMin, ZrotMax) ) );
	}
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5SpaceJunk::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOpenP5SpaceJunk::selfUpdate(){

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOpenP5SpaceJunk::selfDraw(){
    
    /* Lighting Advice 
     
    background(0);
    fill(200);
    
    // Set up some different colored lights
    pointLight(51, 102, 255, 65, 60, 100);
    pointLight(200, 40, 60, -65, -60, -150);
    
    // Raise overall light in scene
    ambientLight(70, 70, 10);
    */ 
    
    ofPushStyle();
    glEnable(GL_DEPTH_TEST);
    ofBackground(0);
    
    ofRotateX(ofRadToDeg(ang));     //X rotation - converts radians to degrees
    ofRotateY(ofRadToDeg(ang));     //Y rotation
    mat->begin();
    for (int i = 0;i < limit; i++) {
		list[i].draw();
	}
    mat->end(); //enables material
	
    ofPopStyle();
    
    ang += (speed_ / 10000); //angular rotation speed
    
}

// draw any debug stuff here
void CloudsVisualSystemOpenP5SpaceJunk::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOpenP5SpaceJunk::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5SpaceJunk::selfEnd(){

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOpenP5SpaceJunk::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5SpaceJunk::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5SpaceJunk::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5SpaceJunk::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5SpaceJunk::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5SpaceJunk::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5SpaceJunk::selfMouseReleased(ofMouseEventArgs& data){
	
}