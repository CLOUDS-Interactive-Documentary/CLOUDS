//
//  CloudsVisualSystemOpenP5Machine.cpp
//

#include "CloudsVisualSystemOpenP5Machine.h"


//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOpenP5Machine::selfSetupGui(){

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

    
//	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
//	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
//	customGui->addButton("Custom Button", false);
//	customGui->addToggle("Custom Toggle", &customToggle);
	
	
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5Machine::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemOpenP5Machine::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5Machine::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5Machine::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5Machine::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5Machine::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemOpenP5Machine::selfSetup(){
	

	
    solidSphereScale = 0.8666;
    solidSphereAlpha = 1.0;
    
    depth = 400;
    
    ofEnableSmoothing();
    ofSetLineWidth(.1);

}


// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5Machine::selfPresetLoaded(string presetPath){

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5Machine::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5Machine::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOpenP5Machine::selfUpdate(){

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOpenP5Machine::selfDraw(){
  
    /*
    
    for(int i=0; i<10; i++) {
        
        float r = random(20);
        
        rotateX(frameCount*PI/1000);
        
        //alt effect
        //rotateY(frameCount*PI/1000);
        
   
        for (int y = -2; y < 2; y++) {
            for (int x = -2; x < 2; x++) {
                for (int z = -2; z < 2; z++) {
                    
                    pushMatrix();
                    translate(400*x, 300*y, 300*z);
                    box(5, 5, 100);
                    popMatrix();
                    
                    pushMatrix();
                    translate(400*x, 300*y, 50*z);
                    box(100, 5, 5);
                    popMatrix();
                    
                    pushMatrix();
                    translate(400*x, 10*y, 50*z);
                    box(50, 5, 5);
                    popMatrix();
                    
                    pushMatrix();
                    rotateY(frameCount*PI/400);
                    translate(100*x, 300*y, 300*z);
                    box(60, 40, 20);
                    popMatrix();
                    
                }
            }
        }
    }
}
    */
    
    ofColor color;
    
    framecount = ofGetElapsedTimeMillis();

    for(int i=0; i<10; i++) {
        
        ofRotateX(framecount*PI/1000);
        
        for (int y = -2; y < 2; y++) {
            for (int x = -2; x < 2; x++) {
                for (int z = -2; z < 2; z++) {

                    ofPushMatrix();
                    ofTranslate(400*x, 300*y, 300*z);
                    //color.setHsb(color1HSB.r,color1HSB.g,color1HSB.b-r/4);
                    //box.setColor(color);
                    ofBox(5, 5, 100);
                    ofPopMatrix();
                    
                    ofPushMatrix();
                    ofTranslate(400*x, 300*y, 50*z);
                    ofBox(100, 10, 10);
                    ofPopMatrix();
                    
                    ofPushMatrix();
                    ofTranslate(400*x, 10*y, 50*z);
                    ofBox(50, 5, 5);
                    ofPopMatrix();
                    
                    ofPushMatrix();
                    ofRotateY(framecount*PI/400);
                    ofTranslate(100*x, 300*y, 300*z);
                    ofBox(60, 40, 20);
                    ofPopMatrix();
                    
                }
            }
        }
    }
}


// draw any debug stuff here
void CloudsVisualSystemOpenP5Machine::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOpenP5Machine::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5Machine::selfEnd(){

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOpenP5Machine::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5Machine::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5Machine::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5Machine::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Machine::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Machine::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Machine::selfMouseReleased(ofMouseEventArgs& data){
	
}