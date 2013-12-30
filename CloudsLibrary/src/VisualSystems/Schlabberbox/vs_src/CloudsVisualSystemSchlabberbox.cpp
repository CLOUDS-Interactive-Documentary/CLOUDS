//
//  CloudsVisualSystemSchlabberbox.cpp
//

#include "CloudsVisualSystemSchlabberbox.h"

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemSchlabberbox::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
	customGui->addButton("Custom Button", false);
	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemSchlabberbox::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemSchlabberbox::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemSchlabberbox::selfSetupSystemGui(){
	
}

void CloudsVisualSystemSchlabberbox::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemSchlabberbox::selfSetupRenderGui(){

}

void CloudsVisualSystemSchlabberbox::guiRenderEvent(ofxUIEventArgs &e){
	
}

//This is called whenever a new preset is loaded, before selfSetup()
//use it to ensure all your simple variables are initialized to an
//acceptable default state
void CloudsVisualSystemSchlabberbox::selfSetDefaults(){

}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemSchlabberbox::selfSetup(){
	fluids.setup(18);
	int numParticles = 9;
	fluids.addParticles(numParticles, numParticles, numParticles, 0, 0, 0, 20, 20, 20);
	fluids.drawCorners = true;
	fluids.drawLines = true;
	fluids.drawForceField = false;
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemSchlabberbox::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemSchlabberbox::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemSchlabberbox::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemSchlabberbox::selfUpdate(){
	fluids.update();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemSchlabberbox::selfDraw(){
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	ofEnableAlphaBlending();
	
//	glEnable (GL_BLEND);
//	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(15);
	
	ofNoFill();
	//ofDrawBox(fluids.x, fluids.y, fluids.z);
	ofTranslate(-ofVec3f(fluids.width, fluids.height, fluids.depth)*.5);
	fluids.draw();
	
	glPopAttrib();
}

// draw any debug stuff here
void CloudsVisualSystemSchlabberbox::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemSchlabberbox::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemSchlabberbox::selfEnd(){

}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemSchlabberbox::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemSchlabberbox::selfKeyPressed(ofKeyEventArgs & args){
	//JG -> Underdoeg: need some way to make this part of the presets or work automatically
	int key = args.key;
	if(key == 'F') {
		cam.disableMouseInput();
		inForceDrawMode = true;
	}else if(key == OF_KEY_UP){
		fluids.viscosity += .001;
		cout << fluids.viscosity << endl;
	}else if(key == OF_KEY_DOWN){
		fluids.viscosity -= .001;
		if(fluids.viscosity < .001)
			fluids.viscosity = .001;
	}else if(key == OF_KEY_LEFT){
		fluids.particleSpeed += 1;
	}else if(key == OF_KEY_RIGHT){
		fluids.particleSpeed -= 1;
	}
}
void CloudsVisualSystemSchlabberbox::selfKeyReleased(ofKeyEventArgs & args){
	//JG -> Underdoeg: need some way to make this part of the presets or work automatically
	//I also made these capitols to avoid other hot key conflicts
	int key = args.key;
	if(key == 'F') {
		cam.enableMouseInput();
		inForceDrawMode = false;
	}else if(key == 'Q'){
		fluids.drawForceField = !fluids.drawForceField;
	}else if(key == 'W'){
		fluids.drawLines = !fluids.drawLines;
	}else if(key == 'E'){
		fluids.drawCorners = !fluids.drawCorners;
	}
	
}

void CloudsVisualSystemSchlabberbox::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSchlabberbox::selfMouseMoved(ofMouseEventArgs& data){
	if(inForceDrawMode) {
		fluids.addVelocity(data.x/float(getCanvasWidth()) * fluids.size, 5,
						   data.y/float(getCanvasHeight()) * fluids.size,
						   (data.x - GetCloudsPreviousInputX())*.2, .1, (data.y - GetCloudsPreviousInputY())*.2);
	}

}

void CloudsVisualSystemSchlabberbox::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSchlabberbox::selfMouseReleased(ofMouseEventArgs& data){
	
}