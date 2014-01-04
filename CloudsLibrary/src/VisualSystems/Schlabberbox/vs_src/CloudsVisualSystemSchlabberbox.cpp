//
//  CloudsVisualSystemSchlabberbox.cpp
//

#include "CloudsVisualSystemSchlabberbox.h"

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemSchlabberbox::selfSetupGui(){

	inForceDrawMode = false;

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

	//customGui->addSlider("Viscosity", 0.001, .01, &fluids.viscosity);
	customGui->addToggle("Apply Force", &inForceDrawMode);
	customGui->addSlider("Force At Y", 1, fluids.size-1, &drawingHeight);
	customGui->addSlider("Force Amount", .1, 3, &drawingForce);

	customGui->addSlider("Particle Speed", 30, 100, &fluids.particleSpeed);
	customGui->addSlider("Particles Return", .001, .3, &fluids.particlesMoveBack);

	//customGui->addButton("Custom Button", false);
	customGui->addToggle("Render Forces", &fluids.drawForceField);
	customGui->addToggle("Render Particles", &fluids.drawParticles);
	customGui->addSlider("Box Size", .1, 3, &fluids.boxSize);
	customGui->addToggle("Render Lines", &fluids.drawLines);
	customGui->addSlider("Box Connect Distance", .5, 3, &fluids.particleConnectDistance);

	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemSchlabberbox::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemSchlabberbox::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		//cout << "Button pressed!" << endl;
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
	fluids.setup(17);
	int numParticles = 9;
	fluids.addParticles(numParticles, numParticles, numParticles, 0, 0, 0, 20, 20, 20);
	fluids.drawParticles = true;
	fluids.drawLines = true;
	fluids.drawForceField = false;
	fluids.particleConnectDistance = 2;

	drawingForce = .3;
	drawingHeight = 1;
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
	//glEnable(GL_POINT_SMOOTH);
	//glPointSize(15);

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

}
void CloudsVisualSystemSchlabberbox::selfKeyReleased(ofKeyEventArgs & args){

}

void CloudsVisualSystemSchlabberbox::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystemSchlabberbox::selfMouseMoved(ofMouseEventArgs& data){
	if(inForceDrawMode) {
		fluids.addVelocity(data.x/float(getCanvasWidth()) * fluids.size, int(drawingHeight),
						   data.y/float(getCanvasHeight()) * fluids.size,
						   (data.x - GetCloudsPreviousInputX())*.3, drawingForce, (data.y - GetCloudsPreviousInputY())*.3);
	}
}

void CloudsVisualSystemSchlabberbox::selfMousePressed(ofMouseEventArgs& data){

}

void CloudsVisualSystemSchlabberbox::selfMouseReleased(ofMouseEventArgs& data){

}