//
//  CloudsVisualSystemOpenP5Hackpact.cpp
//

#include "CloudsVisualSystemOpenP5Hackpact.h"


//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOpenP5Hackpact::selfSetupGui(){

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
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5Hackpact::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemOpenP5Hackpact::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5Hackpact::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5Hackpact::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5Hackpact::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5Hackpact::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemOpenP5Hackpact::selfSetup(){
	
	
	//some of these could become params
	N = 1000;
	tetha = 10;
	ang = TWO_PI/N;
	beta = 0;
	rotation = 0;
	
	mesh.clear();
	
	//cache
	for( int r = 10; r < 1000; r += 50){
		for (int j = 0; j < N; j += 1) {
			
			mesh.addColor(ofColor());
			mesh.addVertex(ofVec3f());
			
			mesh.addColor(ofColor());
			mesh.addVertex(ofVec3f());
			
			mesh.addColor(ofColor());
			mesh.addVertex(ofVec3f());
			
			mesh.addColor(ofColor());
			mesh.addVertex(ofVec3f());
		}
	}
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5Hackpact::selfPresetLoaded(string presetPath){
	beta = 0;
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5Hackpact::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5Hackpact::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOpenP5Hackpact::selfUpdate(){

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOpenP5Hackpact::selfDraw(){
	
	ofColor color;
	ofVec3f point;
	ofMatrix4x4 transform;
	int idx = 0;
	for( int r = 10; r < 1000; r += 50){
		for (int j = 0; j < N; j += 1) {
			
			//P5
			//			float x = r*cos(radians(j));
			//			float y = r*sin(radians(j));
			//			rotateY(radians(beta));
			//
			//			stroke(60,200,255-r/4);
			//			point(x, y, r);
			//			point(-x, -y, r);
			//
			//			stroke(95,200,255-r/4);
			//			point(y, x, -r);
			//			point(-y, -x, -r);
			
			//OF
			float x = r*cos(ofDegToRad(j));
			float y = r*sin(ofDegToRad(j));
			//ofRotateY(ofDegToRad(beta));
			transform.rotate(beta, 0, 1, 0);
			
			color.setHsb(color1HSB.r,color1HSB.g,color1HSB.b-r/4);
			point.set(x, y, r);
			mesh.setColor(idx, color);
			mesh.setVertex(idx++,transform*point);
			
			point.set(-x, -y, r);
			mesh.setColor(idx,color);
			mesh.setVertex(idx++,transform*point);
			
			color.setHsb(color2HSB.r,color2HSB.g,color2HSB.b-r/4);
			point.set(y, x, -r);
			mesh.setColor(idx,color);
			mesh.setVertex(idx++,transform*point);
			
			point.set(-y, -x, -r);
			mesh.setColor(idx,color);
			mesh.setVertex(idx++,transform*point);
		}
	}
	
	beta += 0.0001;
	if( beta >= TWO_PI || beta <= -TWO_PI){
        beta += -1;
	}
	
	mesh.drawVertices();

}

// draw any debug stuff here
void CloudsVisualSystemOpenP5Hackpact::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOpenP5Hackpact::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5Hackpact::selfEnd(){

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOpenP5Hackpact::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5Hackpact::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5Hackpact::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5Hackpact::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Hackpact::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Hackpact::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Hackpact::selfMouseReleased(ofMouseEventArgs& data){
	
}