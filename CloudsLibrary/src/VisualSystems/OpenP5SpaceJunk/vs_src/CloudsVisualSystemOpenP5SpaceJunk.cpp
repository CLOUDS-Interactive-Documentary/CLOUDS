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
		list.push_back( Cube(ofRandom(4, 20), ofRandom(4, 20), ofRandom(2, 20),
                             ofRandom(-140, 140), ofRandom(-140, 140), ofRandom(-140, 140),
                             ofRandom(0, 0), ofRandom(0, 0), ofRandom(0, 0) ) );
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
    
    /*
     
    background(0);
    fill(200);
    
    // Set up some different colored lights
    pointLight(51, 102, 255, 65, 60, 100);
    pointLight(200, 40, 60, -65, -60, -150);
    
    // Raise overall light in scene
    ambientLight(70, 70, 10);
    
    // Center geometry in display windwow.
    // you can change 3rd argument ('0')
    // to move block group closer(+)/further(-)
    translate(width/2, height/2, -200 + mouseX * 0.65);
    
    // Rotate around y and x axes
    rotateY(radians(ang));
    rotateX(radians(ang));
    
    // Draw cubes
    for (int i = 0; i < cubes.length; i++){
        cubes[i].drawCube();
    }
    
    // Used in rotate function calls above
    ang += 0.2;
     
    */
    
    ofPushStyle();
    glEnable(GL_DEPTH_TEST);
    ofBackground(0);
    
    ofRotateX(ofRadToDeg(ang));
    ofRotateY(ofRadToDeg(ang));
    mat->begin();
    for (int i = 0;i < limit; i++) {
		list[i].draw();
	}
    mat->end();
	
    ofPopStyle();
    
    ang += 0.002;
    
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