//
//  CloudsVisualSystemOpenP53DIntro.cpp
//

#include "CloudsVisualSystemOpenP53DIntro.h"


//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOpenP53DIntro::selfSetupGui(){

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
	
	
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP53DIntro::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemOpenP53DIntro::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP53DIntro::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP53DIntro::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP53DIntro::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP53DIntro::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemOpenP53DIntro::selfSetup(){
	

	radius = 75;
	for (int i=0; i<count; i++) {
		list.push_back( Hair(radius) );
	}
    
	noisePosition = 0;
	noiseSpeed = 0;
	noiseScale = 1;
	
    wireSphereScale = 0.9333;
    solidSphereScale = 0.8666;
    wireSphereAlpha = 0.0784;
    solidSphereAlpha = 1.0;
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP53DIntro::selfPresetLoaded(string presetPath){

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP53DIntro::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP53DIntro::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOpenP53DIntro::selfUpdate(){

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOpenP53DIntro::selfDraw(){
    
	ofPushStyle();
	
    ofFill();
    ofSetColor(20,solidSphereAlpha*255.0);
	ofSphere(0, 0, solidSphereScale*300 );
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	
	float rxp = ((ofGetMouseX()-(ofGetWidth()/2))*0.005);
	float ryp = ((ofGetMouseY()-(ofGetHeight()/2))*0.005);
	rx = (rx*0.9)+(rxp*0.1);
	ry = (ry*0.9)+(ryp*0.1);
	ofRotateY(rx);
	ofRotateX(ry);
	//sphere(radio);
	
	ofMesh mesh;
	noisePosition += noiseSpeed;
	for (int i = 0;i < count; i++) {
		list[i].draw(mesh, noisePosition, noiseScale, solidSphereAlpha);
	}
	mesh.setMode(OF_PRIMITIVE_LINES);
	mesh.draw();

	ofPopStyle();
}

// draw any debug stuff here
void CloudsVisualSystemOpenP53DIntro::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOpenP53DIntro::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP53DIntro::selfEnd(){

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOpenP53DIntro::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP53DIntro::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP53DIntro::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP53DIntro::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP53DIntro::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP53DIntro::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP53DIntro::selfMouseReleased(ofMouseEventArgs& data){
	
}