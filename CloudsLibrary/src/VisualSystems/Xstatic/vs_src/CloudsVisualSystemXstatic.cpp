//
//  CloudsVisualSystemXstatic.cpp
//

#include "CloudsVisualSystemXstatic.h"
#include "CloudsRGBDVideoPlayer.h"
#include "ofxBillboard.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

float CloudsVisualSystemXstatic::nParticles = 1000;

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemXstatic::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addIntSlider("Max Brightness", 0, 255, &maxBrightness);
    customGui->addIntSlider("Min Brightness", 0, 255, &minBrightness);
    
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemXstatic::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemXstatic::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemXstatic::selfSetupSystemGui(){
	
}

void CloudsVisualSystemXstatic::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemXstatic::selfSetupRenderGui(){

}

void CloudsVisualSystemXstatic::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemXstatic::selfSetup(){
	
    for(int i = 0; i<nParticles; i++){

       // particles.push_back( Particle(0, ofRandom(-.55,-.35), 0)); // falling particles
        
       // particles.push_back( Particle(ofRandom(1,2), ofRandom(1,2), ofRandom(1,2))); // ricocheting particles
        
        particles.push_back( Particle(0, ofRandom(.40,.80), 0, minBrightness, maxBrightness)); // rising particles
    }
  	
//	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemXstatic::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemXstatic::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemXstatic::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemXstatic::selfUpdate(){

  
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemXstatic::selfDraw(){
	
    glDisable(GL_DEPTH_TEST);
    

    for(int i = 0; i < nParticles; i++){
        particles[i].display();
        //particles[i].checkEdges();
        particles[i].verticalWraparound();
        //ofxBillboardBeginSphericalObvious(ofVec3f::zero(), particles[i].location);
    
    }

    
    ofNoFill();
	ofSetColor(255);
	ofBox(0,0,0, 300);

	
}

// draw any debug stuff here
void CloudsVisualSystemXstatic::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemXstatic::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemXstatic::selfEnd(){
	

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemXstatic::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemXstatic::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemXstatic::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemXstatic::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemXstatic::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemXstatic::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemXstatic::selfMouseReleased(ofMouseEventArgs& data){
	
}