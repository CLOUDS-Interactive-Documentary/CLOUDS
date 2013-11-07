//
//  CloudsVisualSystemRandomDigits.cpp
//

#include "CloudsVisualSystemRandomDigits.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemRandomDigits::selfSetupGui(){
    
    
    ofGetRectMode() == OF_RECTMODE_CENTER;
    /*
     customGui = new ofxUISuperCanvas("CUSTOM", gui);
     customGui->copyCanvasStyle(gui);
     customGui->copyCanvasProperties(gui);
     customGui->setName("Custom");
     customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
     
     
     ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemRandomDigits::selfGuiEvent);
     guis.push_back(customGui);
     guimap[customGui->getName()] = customGui;
     */
}

void CloudsVisualSystemRandomDigits::selfGuiEvent(ofxUIEventArgs &e){
    //	if(e.widget->getName() == "Custom Button"){
    //		cout << "Button pressed!" << endl;
    //	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemRandomDigits::selfSetupSystemGui(){
	
}

void CloudsVisualSystemRandomDigits::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemRandomDigits::selfSetupRenderGui(){
    
}

void CloudsVisualSystemRandomDigits::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemRandomDigits::selfSetup(){
	
    glEnable(GL_DEPTH_TEST);
    ofBackground(0);
    //ofSetFrameRate(30);
    Rand::Font.loadFont(getVisualSystemDataPath() + "Courier.ttf", 14, true, true, true);
    
    
    for (int i = 0; i < Rand::rows; i++){
        for (int j = 0; j < Rand::columns; j ++){ //rows
            
            cout << "yoffset = " << yoffset << endl;
            if(j%4==0){
                yoffset = 100;
            }
            else{
                yoffset = 50;
            }
           
            /*
            if(i%2==0){
                xoffset = 300;
            }
            else{
                xoffset = 100;
            }
             */
            grid.push_back(Rand (i*xoffset, j*(yoffset), int(ofRandom(10000, 99999)+.5)));
            
            
            
        }
    }
    
    for (int i = 0; i < grid.size(); i++){
        
        grid[i].changeRandomNumber();
    }
}


// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemRandomDigits::selfPresetLoaded(string presetPath){
    
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemRandomDigits::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemRandomDigits::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemRandomDigits::selfUpdate(){
    
    
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemRandomDigits::selfDraw(){
    
    ofBackground(0);
    
    
    cam.begin();
    
    //ofDrawBox(10);
    //ofDrawAxis(20)
    
    for (int i = 0; i < grid.size(); i++){
        
        ofPushStyle();
        ofPushMatrix();
        
        ofTranslate(-800,-400);
        //noiseRotation
        // if(! (ofGetElapsedTimeMillis()% 8000)){
        cout<<"here"<< " :"<<i<<endl;
        //          for (int k = 0; k < grid.size(); k++){
        //          grid[k].changeRandomNumber();
        //          }
        if(! (ofGetElapsedTimeMillis()% 500)){
            
            grid[ofRandom(grid.size())].changeRandomNumber();
            
        }
        grid[i].drawNumbers();
        //sleep(150);
        ofPopMatrix();
        ofPopStyle();
    }
    
    //int randomIndex = int(ofRandom(grid.size()));
    // Rand[randomIndex].generateRandomNumber();
    
    cam.end();
    
}


// draw any debug stuff here
void CloudsVisualSystemRandomDigits::selfDrawDebug(){
    
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemRandomDigits::selfDrawBackground(){
    
	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemRandomDigits::selfEnd(){
    
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemRandomDigits::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemRandomDigits::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemRandomDigits::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemRandomDigits::selfMouseDragged(ofMouseEventArgs& data){
    
    
	
}


void CloudsVisualSystemRandomDigits::selfMouseMoved(ofMouseEventArgs& data){
    
    float x = ofMap(data.x, 0, ofGetWidth(), -(ofGetWidth()/2.0), (ofGetWidth()/2.0));
    float y = ofMap(data.y, 0, ofGetHeight(), -(ofGetHeight()/2.0), (ofGetHeight()/2.0));
    
    ofPushStyle();
    //ofSetColor(255,255, 255);
    //ofFill();
    ofRect(x, y, 20, 20);
    ofPopStyle();
	
}

void CloudsVisualSystemRandomDigits::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemRandomDigits::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemRandomDigits::sleep(long d){
    
    
    clock_t start=clock();
    while(clock() - start < d); ///loop until time's up
    
    
}
