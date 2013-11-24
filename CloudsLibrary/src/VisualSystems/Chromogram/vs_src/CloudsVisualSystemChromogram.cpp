//
//  CloudsVisualSystemChromogram.cpp
//

#include "CloudsVisualSystemChromogram.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemChromogram::selfSetupGui(){
    
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	//customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
    //customGui->addButton("Custom Button", false);
    
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemChromogram::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemChromogram::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemChromogram::selfSetupSystemGui(){
	
}

void CloudsVisualSystemChromogram::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemChromogram::selfSetupRenderGui(){
    
}

void CloudsVisualSystemChromogram::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemChromogram::selfSetup(){
	
    seed =  int(ofRandom(20));
    numRandomColors = 2400;
    ofSetFrameRate(40);
    
    //	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemChromogram::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemChromogram::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemChromogram::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemChromogram::selfUpdate(){
    
    
    ////////////////////////////////////////////////////////////////////////
    // FILL THE VECTOR WITH RANDOM NUMBERS FOR STARTERS
    
    float t = ofGetFrameNum() / 30.0;
    
    
    if (filled == false){
        for (int i = 0; i < numRandomColors; i++){
            n = n+1;
           
            if (int(ofRandom(500)) == 3){
                sporadicColorChanger = (ofRandom(100)-50);
                cout <<  "SPORADICALLY CHANGED BY " << sporadicColorChanger << endl;
            }
            else{
                sporadicColorChanger = 0;
            }
            
            if (int(ofRandom(50)) == 3){
                sporadicSaturationChanger = (ofRandom(50)-25);
                //cout <<  "SPORADICALLY CHANGED BY " << sporadicSaturationChanger << endl;
            }
            else{
                sporadicSaturationChanger = 0;
            }
            
            if (int(ofRandom(10)) == 3){
                sporadicSaturationChanger = (ofRandom(100)-50);
                //cout <<  "SPORADICALLY CHANGED BY " << sporadicSaturationChanger << endl;
            }
            else{
                sporadicBrightnessChanger = 0;
            }
            
            
            noiseHue += (ofNoise( n * .1, t) * 10 - 5) + sporadicColorChanger; //generate noise hue
            noiseSaturation += (ofNoise(t, n * .01) * 10 - 5) + sporadicSaturationChanger; // generation saturation
            noiseBrightness = noiseSaturation/2 + ofRandom(-100, 100) + sporadicBrightnessChanger;
            //noiseBrightness = ofRandom(200, 255);
            float newHue = ofMap(noiseHue,-300, 200, 20, 250, true);
            float newSaturation = ofMap(noiseSaturation,-300, 200, 180, 220, true);
            float newBrightness = ofMap(noiseBrightness,-300, 200, 150, 255, true);
            //cout <<  "raw hues " << noiseHue << "  mapped hue: " << newHue << endl;
            //cout <<  "raw brightness " << noiseBrightness << "  mapped brightness: " << newBrightness << endl;
            color1.setHsb(newHue, newSaturation, newBrightness);
            randomColors.push_back(color1); // noise value
            
            //  cout << "time: " <<  t << "size of vector: " << randomColors.size() << "  noise value " << noiseValue << endl;
        }
        
        if (randomColors.size() >= numRandomColors){
            filled = true;
        }
    }
    
    ////////////////////////////////////////////////////////////////////////
    // DELETE THE FIRST ITEM IN VECTOR, ADD A NEW ONE TO THE BACK
    
    // Generate a new noise value
    n = n+1;
    if (int(ofRandom(500)) == 3){
         sporadicColorChanger = (ofRandom(100)-50);
        //cout <<  "SPORADICALLY CHANGED BY " << sporadicColorChanger << endl;
    }
    else{
         sporadicColorChanger = 0;
    }
    
    if (int(ofRandom(50)) == 3){
        sporadicSaturationChanger = (ofRandom(50)-25);
        //cout <<  "SPORADICALLY CHANGED BY " << sporadicSaturationChanger << endl;
    }
    else{
        sporadicSaturationChanger = 0;
    }
    
    if (int(ofRandom(10)) == 3){
        sporadicSaturationChanger = (ofRandom(100)-50);
        //cout <<  "SPORADICALLY CHANGED BY " << sporadicSaturationChanger << endl;
    }
    else{
        sporadicBrightnessChanger = 0;
    }
    
    
    noiseHue += (ofNoise( n * .1, t) * 10 - 5) + sporadicColorChanger; //generate noise hue
    noiseSaturation += (ofNoise(t, n * .01) * 10 - 5) + sporadicSaturationChanger; // generation saturation
    noiseBrightness = noiseSaturation/2 + ofRandom(-100, 100) + sporadicBrightnessChanger;
    //noiseBrightness = ofRandom(200, 255);
    float newHue = ofMap(noiseHue,-300, 200, 20, 250, true);
    float newSaturation = ofMap(noiseSaturation,-300, 200, 180, 240, true);
    float newBrightness = ofMap(noiseBrightness,-300, 200, 150, 255, true);
    //cout <<  "raw hues " << noiseHue << "  mapped hue: " << newHue << endl;
    cout <<  "raw brightness " << noiseBrightness << "  mapped brightness: " << newBrightness << endl;
    color1.setHsb(newHue, newSaturation, newBrightness);
    
    randomColors.erase(randomColors.begin());
    randomColors.push_back(color1); // noise value
    
    ////////////////////////////////////////////////////////////////////////
    // ADD VERTICES TO THE MESH
    
    
    chromogram.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    chromogram.clear();
    
    int p = 0;
    int r = 0;
    
    for (int j = randomColors.size()-1; j > 0 ; j--){
        
        r++;
        
        if(r%75 == 0){
            p++;
            r = 0;
        }
        
        
        //float k = ofMap(j,0,randomColors.size(),255,80);
      
        //bottom left
        ofPoint a = ofPoint((r * rectWidth), (p * rectHeight));
        //topleft
        ofPoint b = ofPoint((r * rectWidth), (p * rectHeight) + rectHeight);
        //bottom right
        ofPoint c = ofPoint((r * rectWidth) + rectWidth, (p * rectHeight));
        //top right
        ofPoint d = ofPoint((r * rectWidth) + rectWidth, (p * rectHeight) + rectHeight);
        
        ofBeginShape();
        
        color2 = randomColors[j];
        
        chromogram.addColor(color2);
        chromogram.addVertex(a);
        chromogram.addColor(color2);
        chromogram.addVertex(b);
        chromogram.addColor(color2);
        chromogram.addVertex(c);
        chromogram.addColor(color2);
        chromogram.addVertex(d);
        
        ofEndShape();
        
        // cout << "xoffset: " <<  xoffset << endl;
        //cout <<  "index : " << j << "  current number: " << k << endl;
    
        
}
    
   
    
    
    
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemChromogram::selfDraw(){
	
    ofPushMatrix();
    ofPushStyle();
    
    ofTranslate(-150,0);
    chromogram.draw();
	ofPopStyle();	
    ofPopMatrix();

	
}

// draw any debug stuff here
void CloudsVisualSystemChromogram::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemChromogram::selfDrawBackground(){
    
	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemChromogram::selfEnd(){
	
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemChromogram::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemChromogram::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemChromogram::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemChromogram::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemChromogram::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemChromogram::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemChromogram::selfMouseReleased(ofMouseEventArgs& data){
	
}