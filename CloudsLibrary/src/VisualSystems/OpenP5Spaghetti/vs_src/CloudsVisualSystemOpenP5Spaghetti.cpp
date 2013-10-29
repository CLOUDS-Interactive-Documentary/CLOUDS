//
//  CloudsVisualSystemOpenP5Spaghetti .cpp
//

#include "CloudsVisualSystemOpenP5Spaghetti.h"


//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOpenP5Spaghetti::selfSetupGui(){

    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Spaghetti");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);


    customGui->addSpacer(length-xInit, 2);
    customGui->addLabel("SIMULATION");
    customGui->addLabel("click to reset");
    customGui->addToggle("SMOOTH", &smooth);
    customGui->addToggle("GNARLY", &gnarly);
    customGui->addSlider("Number of Walkers", 1, 50, &NWalkers);
    customGui->addSlider("Particles per Walker", 10, 1000, &Walker::nParticles);
  //  customGui->addSlider("STEP SIZE", 0.0, 5.0, &Walker::stepSize);
//	customGui->addSlider("NOISE SPEED", 0.0, 1.0, &Walker::noiseSpeed1);

    
    customGui->addLabel("RENDERING");
	customGui->addToggle("DRAW POINTS", &Walker::drawPoints);
    customGui->addToggle("DRAW LINES", &Walker::drawLines);
 

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
    
    NWalkers = 10;
    smooth = true; 
    Walker::drawLines = true; 
    
    for(int i = 0; i<NWalkers; i++){
   
        walkers.push_back( Walker() );
		walkers[i].init(40, ofFloatColor::white);
    
//        walkers[i] = *new Walker();
    }
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
void CloudsVisualSystemOpenP5Spaghetti ::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOpenP5Spaghetti ::selfUpdate(){
    
    if (smooth){
        gnarly = false;
    for(int i = 0; i < NWalkers; i++){
        walkers[i].smoothTrails();
        
        }
    }
    if (gnarly){
        smooth = false;
    for(int i = 0; i < NWalkers; i++){
        walkers[i].gnarlyTrails();
    }
    }
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOpenP5Spaghetti ::selfDraw(){
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	glDisable(GL_DEPTH_TEST);
	
    for(int i = 0; i<NWalkers; i++){
		
        walkers[i].draw();
    }
}

// draw any debug stuff here
void CloudsVisualSystemOpenP5Spaghetti ::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOpenP5Spaghetti ::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5Spaghetti ::selfEnd(){

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOpenP5Spaghetti ::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5Spaghetti ::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5Spaghetti ::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5Spaghetti ::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Spaghetti ::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Spaghetti ::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Spaghetti ::selfMouseReleased(ofMouseEventArgs& data){
	
}