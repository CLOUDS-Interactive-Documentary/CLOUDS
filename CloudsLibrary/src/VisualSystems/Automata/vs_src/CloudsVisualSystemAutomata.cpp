//
//  CloudsVisualSystemAutomata.cpp
//

#include "CloudsVisualSystemAutomata.h"


//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemAutomata::selfSetupGui(){

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
	
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemAutomata::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemAutomata::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemAutomata::selfSetupSystemGui(){
	
}

void CloudsVisualSystemAutomata::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemAutomata::selfSetupRenderGui(){

}

void CloudsVisualSystemAutomata::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemAutomata::selfSetup(){
    
    if(!image.loadImage(getVisualSystemDataPath() + "mem.gif")){
        cout << "failed to load " << getVisualSystemDataPath() + "mem.gif" << endl;
    }
    
    conway.allocate(ofGetWidth(), ofGetHeight());
    conway.setPasses(10);
    //
    // Created by kalwalt alias Walter Perdan on 24/12/11
    // Copyright 2011 http://www.kalwaltart.it/ All rights reserved.
    conway.setCode( "#version 120\n \
                   #extension GL_ARB_texture_rectangle : enable\n \
                   \
                   uniform sampler2DRect tex0; \
                   \
                   vec4 dead = vec4(1.0,1.0,1.0,1.0); \
                   vec4 alive = vec4(0.0,0.0,0.0,1.0); \
                   \
                   void main(void) { \
                   vec2  st = gl_TexCoord[0].st;\
                   int sum = 0; \
                   vec4 y = texture2DRect(tex0, st); \
                   \
                   if (texture2DRect(tex0, st + vec2(-1.0, -1.0)) == alive) ++sum; \
                   if (texture2DRect(tex0, st + vec2(0.0, -1.0)) == alive) ++sum; \
                   if (texture2DRect(tex0, st + vec2(1.0, -1.0)) == alive) ++sum; \
                   \
                   if (texture2DRect(tex0, st + vec2(-1.0, 0.0)) == alive) ++sum; \
                   if (texture2DRect(tex0, st + vec2(1.0, 0.0)) == alive) ++sum; \
                   \
                   if (texture2DRect(tex0, st + vec2(-1.0, 1.0)) == alive) ++sum; \
                   if (texture2DRect(tex0, st + vec2(0.0, 1.0)) == alive) ++sum; \
                   if (texture2DRect(tex0, st + vec2(1.0, 1.0)) == alive) ++sum; \
                   \
                   if (sum < 2) gl_FragColor = dead; \
                   else if (sum > 3) gl_FragColor = dead; \
                   else if (sum == 3) gl_FragColor = alive; \
                   else gl_FragColor = y; \
                   }");
    
    conway.setTexture(image.getTextureReference());
	

}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemAutomata::selfPresetLoaded(string presetPath){

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemAutomata::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemAutomata::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemAutomata::selfUpdate(){


    conway.begin();
    ofClear(255, 255);
    conway.draw();
    ofSetColor(0,255);
    ofCircle(ofGetMouseX(), ofGetMouseY(), 5);
    conway.end();
    conway.update();
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemAutomata::selfDraw(){
    
    ofDisableLighting();
    
    ofPushMatrix();
    ofScale(1, -1, 1);
    ofSetColor(255);
    conway.draw(-conway.getWidth() / 2, -conway.getHeight() / 2);
    ofPopMatrix();

}

// draw any debug stuff here
void CloudsVisualSystemAutomata::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemAutomata::selfDrawBackground(){
   // glDisable(GL_DEPTH_TEST);
   // ofSetColor(255);
    
    //image.draw(0, 0);


	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemAutomata::selfEnd(){

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemAutomata::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemAutomata::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemAutomata::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemAutomata::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemAutomata::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemAutomata::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemAutomata::selfMouseReleased(ofMouseEventArgs& data){
	
}