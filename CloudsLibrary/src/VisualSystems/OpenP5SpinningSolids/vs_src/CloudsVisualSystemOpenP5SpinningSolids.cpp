//
//  CloudsVisualSystemOpenP5SpinningSolids.cpp
//

#include "CloudsVisualSystemOpenP5SpinningSolids.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOpenP5SpinningSolids::selfSetupGui(){

    
    ofGetRectMode() == OF_RECTMODE_CORNER;
    /*
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5SpinningSolids::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
     */
}

void CloudsVisualSystemOpenP5SpinningSolids::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5SpinningSolids::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5SpinningSolids::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5SpinningSolids::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5SpinningSolids::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemOpenP5SpinningSolids::selfSetup(){
	
    glEnable(GL_DEPTH_TEST);
    ofBackground(0);
    //ofSetFrameRate(30);

    //int IDnumber = 0;
    
    for (int i = 0; i < num-1; i++){
        for (int j = 0; j < num-1; j ++){ //rows
			grid.push_back(Tetra(j*xoffset, i*yoffset, 0, 10));
          //  grid.back().ID = IDnumber + 1;
        }
    }

}


// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5SpinningSolids::selfPresetLoaded(string presetPath){

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5SpinningSolids::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5SpinningSolids::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOpenP5SpinningSolids::selfUpdate(){
    

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOpenP5SpinningSolids::selfDraw(){
    
    ofBackground(0);
    
    
    cam.begin();

            //ofDrawBox(10);
            //ofDrawAxis(20)

    for (int i = 0; i < grid.size(); i++){

            ofPushStyle();
            ofPushMatrix();
        
            //noiseRotation
             grid[i].drawShape();
           
       
            ofPopMatrix();
            ofPopStyle();
    }
    cam.end();

}

 
// draw any debug stuff here
void CloudsVisualSystemOpenP5SpinningSolids::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOpenP5SpinningSolids::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5SpinningSolids::selfEnd(){

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOpenP5SpinningSolids::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5SpinningSolids::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5SpinningSolids::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5SpinningSolids::selfMouseDragged(ofMouseEventArgs& data){
    
    
	
}


void CloudsVisualSystemOpenP5SpinningSolids::selfMouseMoved(ofMouseEventArgs& data){
    
    float x = ofMap(data.x, 0, ofGetWidth(), -(ofGetWidth()/2.0), (ofGetWidth()/2.0));
    float y = ofMap(data.y, 0, ofGetHeight(), -(ofGetHeight()/2.0), (ofGetHeight()/2.0));
    
    ofPushStyle();
    //ofSetColor(255,255, 255);
    //ofFill();
    ofRect(x, y, 20, 20);
    ofPopStyle();
	
}

void CloudsVisualSystemOpenP5SpinningSolids::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5SpinningSolids::selfMouseReleased(ofMouseEventArgs& data){
	
}


