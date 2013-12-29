//
//  CloudsVisualSystemOpenP5Spaghetti .cpp
//

#include "CloudsVisualSystemOpenP5Spaghetti.h"

float CloudsVisualSystemOpenP5Spaghetti::NWalkers = 100;
bool CloudsVisualSystemOpenP5Spaghetti::smooth = true;
//bool CloudsVisualSystemOpenP5Spaghetti::drawTriangles = false;

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOpenP5Spaghetti::selfSetupGui(){

    // Set defaults.
    currSpin = 0.0f;
    spinSpeed = 0.5f;

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Spaghetti");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

   
    customGui->addSpacer();
    customGui->addLabel("SIMULATION");
    customGui->addButton("REGENERATE", &shouldRegenerate);
    customGui->addIntSlider("Preloads", 0, 50000, &numPreloads);
    vector<string> modes;
    modes.push_back("SMOOTH");
    modes.push_back("GNARLY");
    customGui->addRadio("MODE", modes, OFX_UI_ORIENTATION_HORIZONTAL);
    customGui->addSlider("Number of Walkers", 1, 100, &NWalkers);
    customGui->addSlider("Particles per Walker", 10, 1000, &Walker::nParticles);
    
    customGui->addSlider("SPIN SPEED", 0, 5, &spinSpeed);
    customGui->addMinimalSlider("STEP SIZE X", 0.0, 5.0, &Walker::stepSizex);
    customGui->addMinimalSlider("STEP SIZE Y", 0.0, 5.0, &Walker::stepSizey);
    customGui->addMinimalSlider("STEP SIZE Z", 0.0, 5.0, &Walker::stepSizez);
    
	customGui->addMinimalSlider("NOISE SPEED X", 0.0, 20.0, &Walker::noiseSpeedx);
    customGui->addMinimalSlider("NOISE SPEED Y", 0.0, 20.0, &Walker::noiseSpeedy);
    customGui->addMinimalSlider("NOISE SPEED Z", 0.0, 20.0, &Walker::noiseSpeedz);
    
    customGui->addLabel("COLOR MODES");
    customGui->addSpacer();
   // customGui->addImageSampler("Color 1", &colorMap, colorMap.getWidth()/2., colorMap.getHeight()/2. );
   //customGui->addImageSampler("Color 2", &colorMap, colorMap.getWidth()/2., colorMap.getHeight()/2. );
    
    customGui->addMinimalSlider("Hue1", 0.0, 255.0, &hue1);
    customGui->addMinimalSlider("Saturation1", 0.0, 200.0, &saturation1);
    customGui->addMinimalSlider("Brightness1", 0.0, 255.0, &brightness1);
    customGui->addMinimalSlider("Hue2", 0.0, 255.0, &hue2);
    customGui->addMinimalSlider("Saturation2", 0.0, 200.0, &saturation2);
    customGui->addMinimalSlider("Brightness2", 0.0, 255.0, &brightness2);
    
    customGui->addToggle("Oscillator Mode", &oscillate);
    customGui->addToggle("Rainbow Mode", &rainbow);
    customGui->addToggle("Dichromatic Mode", &dichromatic);
    customGui->addSlider("Oscilator Period (in frames)", 0.0, 1000.0, &period);
    customGui->addMinimalSlider("Saturation", 0.0, 200.0, &saturation);
    customGui->addMinimalSlider("Brightness", 0.0, 255.0, &brightness);
    vector<string> drawModes;
    drawModes.push_back("DRAW POINTS");
    drawModes.push_back("DRAW LINES");
    customGui->addRadio("DRAW MODES", drawModes);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    customGui->addSlider("LINE WIDTH", 0.1, 10, &Walker::lineWidth);

    ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5Spaghetti::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
 
  
}

void CloudsVisualSystemOpenP5Spaghetti::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
    
    if(e.widget->getName() == "REGENERATE" && ((ofxUIButton*)e.widget)->getValue() ){
		regenerate();
	}
    
    else if (e.widget->getName() == "SMOOTH" && ((ofxUIToggle *)e.widget)->getValue()) {
        smooth = true;
    }
    else if (e.widget->getName() == "GNARLY" && ((ofxUIToggle *)e.widget)->getValue()) {
        smooth = false;
    }
    
    else if (e.widget->getName() == "DRAW POINTS" && ((ofxUIToggle *)e.widget)->getValue()) {
        Walker::drawPoints = true;
    }
    else if (e.widget->getName() == "DRAW LINES" && ((ofxUIToggle *)e.widget)->getValue()) {
        Walker::drawPoints = false;
    }
}


void CloudsVisualSystemOpenP5Spaghetti::regenerate(){    
	
    color1.setHsb(hue1,saturation1,brightness1);
    // cout << "regenerated color1 = "<< color1 << endl;
    color2.setHsb(hue2,saturation2,brightness2);
    //  cout << "regenerated color2 = "<< color2 << endl;
    
    
    
    for(int i = 0; i<NWalkers; i++){
        //clear all meshes
        walkers[i].mesh.getVertices().clear();
        walkers[i].mesh.getColors().clear();
        
        //set positions to zero
        walkers[i].position.x = 0;
        walkers[i].position.y = 0;
        walkers[i].position.z = 0;
        
        // COLOR MODES
        
        if(rainbow){ //if rainbow mode is selected, choose a random hue between 0 and 255
            dichromatic = false; oscillate = false;
            newColor.setHsb(ofRandom(255),saturation, brightness);
            walkers[i].setColor(newColor);
        }
        else if(dichromatic){ //if rainbow mode is not selected, choose a random color between color1 and color2
            rainbow = false;
            randomColor = color1.getLerped(color2, ofRandom(1.0));
            //newColor.setHsb(randomColor.getHue(),saturation, brightness);
            newColor = randomColor;
            //  cout << "new color = " << i << " " << newColor << endl;
            walkers[i].setColor(newColor);
        }
        else if(oscillate){
            rainbow = false;
            newColor = color1;
            walkers[i].setColor(newColor);
        }
    }
    
    //shouldn't PRELOADS should happen in regenerate...?
    for (int j = 0; j<numPreloads; j++){
        
        selfUpdate();
        cout << "preloaded " << j << "times" << endl;
        //selfDraw();
    }
    
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

  //  colorMap.loadImage( getVisualSystemDataPath() + "GUI/defaultColorPalette.png" );
    
    color1.setHsb(hue1,saturation1, brightness1);
    //cout << "color1 = "<< color1 << endl;
    
    color2.setHsb(hue2,saturation2, brightness2);
   // cout << "color2 = "<< color2 << endl;
    
    for(int i = 0; i<NWalkers; i++){
        newColor = color1; 
        walkers.push_back( Walker() );
        walkers[i].init(NWalkers, newColor); //  walkers[i] = *new Walker(); << wrong syntax

    }
   
    
}


// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5Spaghetti::selfPresetLoaded(string presetPath){
	regenerate();
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5Spaghetti::selfBegin(){
	regenerate();
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5Spaghetti ::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOpenP5Spaghetti ::selfUpdate(){
    
    //gets approximate framecount
    float frameCount = ofGetElapsedTimeMillis()/33.0; 
   
    //cout << "new color " << newColor << endl;
    
    //update speed
    currSpin += spinSpeed;
    
        
    for(int i = 0; i < NWalkers; i++){
        
        
        if(oscillate && !rainbow){ float osc = amplitude * cos(TWO_PI * frameCount / period); // oscilates between -100 and 100 ever 120 frames
        newColor = color1.getLerped(color2, (ofMap(osc, -100.0, 100.0, 0.0, 1.0)));  //oscillates between first color and second color
             walkers[i].setColor(newColor);
        }

        if (smooth){
            walkers[i].smoothTrails();
        }
        else {
            walkers[i].gnarlyTrails();
        }
     //   if (drawTriangles){ smooth = false;  walkers[i].doubleTrails(); }
        }
       
    }


// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOpenP5Spaghetti::selfDraw(){

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glPointSize(1);

    ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofPushMatrix();

	ofRotate(currSpin, 0, 1, 0);
    ofSetLineWidth(Walker::lineWidth);
    for(int i = 0; i < NWalkers; i++){
		walkers[i].draw();
	}
    ofSetLineWidth(1.0f);
	
	ofPopStyle();
    ofPopMatrix();
	glPopAttrib();
}

// draw any debug stuff here
void CloudsVisualSystemOpenP5Spaghetti::selfDrawDebug(){

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