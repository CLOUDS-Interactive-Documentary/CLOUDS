//
//  CloudsVisualSystemHistogram.cpp
//

#include "CloudsVisualSystemHistogram.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemHistogram::selfSetupGui(){
    
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	//customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
    //customGui->addButton("Custom Button", false);
    
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemHistogram::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemHistogram::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemHistogram::selfSetupSystemGui(){
	
}

void CloudsVisualSystemHistogram::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemHistogram::selfSetupRenderGui(){
    
}

void CloudsVisualSystemHistogram::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemHistogram::selfSetup(){
	
    
    
    seed =  int(ofRandom(20));
    numRandomData = 3200;
    ofSetFrameRate(60);
    
    //	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemHistogram::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemHistogram::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemHistogram::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemHistogram::selfUpdate(){
    
    
    ////////////////////////////////////////////////////////////////////////
    // FILL THE VECTOR WITH RANDOM NUMBERS FOR STARTERS
    
    float t = ofGetFrameNum() / 50.0;
    
    
    if (filled == false){
        for (int i = 0; i < numRandomData; i++){
            n = n+1;
            //randomData.push_back(ofRandom(1,100)); // random float between 1 and 100
            noiseValue += ofNoise( n * .01, t) * 10 - 5; //generate noise value
            noiseValue = noiseValue + ofRandom(-70.0, 70.0); // add randomness
            float newValue = ofMap(noiseValue,-500, 2000, 10, 400, true);
            randomData.push_back(newValue); // noise value
            //cout << "time: " <<  t << "size of vector: " << randomData.size() << "  current number: " << randomData.at(i) << endl;
            //  cout << "time: " <<  t << "size of vector: " << randomData.size() << "  noise value " << noiseValue << endl;
        }
        
        if (randomData.size() >= numRandomData){
            filled = true;
        }
    }
    
    ////////////////////////////////////////////////////////////////////////
    // DELETE THE FIRST ITEM IN VECTOR, MOVE EVERY VALUE UP ONE, ADD A NUMBER TO THE END
    
    // Generate a new noise value
    n = n+1;
    noiseValue += ofNoise( n * .01, t) * 10 - 5; // generate noise value
    noiseValue = noiseValue + ofRandom(-70.0, 70.0); // add randomness
    float newValue = ofMap(noiseValue, -500, 2000, 10, 400, true);
    
    randomData.erase(randomData.begin());
    randomData.push_back(newValue); // noise value
    
    ////////////////////////////////////////////////////////////////////////
    // ADD VERTICES TO THE MESH
    
    
    histo.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    histo.clear();
    
    int p = 0;
    int r = 0;
    
    for (int j = randomData.size()-1; j > 0 ; j--){
                
        if(p%100 == 0){
            r++;
            p = 0;
            xoffset = 0;
            cout << "bang: " <<  xoffset << endl;
            
        }
        
        p++;
        xoffset = p * rectWidth;
        
        float k = ofMap(j,0,randomData.size(),255,80);
//      cout<<randomD
        rectHeight = randomData[j];
      
        //bottom left
        ofPoint a = ofPoint(xpos + xoffset, ypos, r*zoffset);
        //topleft
        ofPoint b = ofPoint(xpos + xoffset, ypos + rectHeight, r*zoffset);
        //bottom right
        ofPoint c = ofPoint(xpos + rectWidth + xoffset, ypos, r*zoffset);
        //top right
        ofPoint d = ofPoint(xpos + rectWidth + xoffset, ypos + rectHeight, r*zoffset);
        
        ofBeginShape();
        
        color1.setHsb(k,180 + ofRandom(25),225 + ofRandom(25), 225);
        color2.setHsb(0,0,0,0);
       
        histo.addColor(color2);
        histo.addVertex(a);
        histo.addColor(color1);
        histo.addVertex(a);
        histo.addColor(color1);
        histo.addVertex(b);
        histo.addColor(color1);
        histo.addVertex(c);
        histo.addColor(color1);
        histo.addVertex(d);
        histo.addColor(color2);
        histo.addVertex(d);
        
        ofEndShape();
        
        // cout << "xoffset: " <<  xoffset << endl;
        //cout <<  "index : " << j << "  current number: " << k << endl;
    
        
    }
    

    
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemHistogram::selfDraw(){
	
    //glDisable(GL_DEPTH_TEST);
    //ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    ofPushMatrix();
    ofPushStyle();
    
    ofTranslate(-150,0);
    histo.draw();
	ofPopStyle();	
    ofPopMatrix();

	
}

// draw any debug stuff here
void CloudsVisualSystemHistogram::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemHistogram::selfDrawBackground(){
    
	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemHistogram::selfEnd(){
	
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemHistogram::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemHistogram::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemHistogram::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemHistogram::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemHistogram::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemHistogram::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemHistogram::selfMouseReleased(ofMouseEventArgs& data){
	
}