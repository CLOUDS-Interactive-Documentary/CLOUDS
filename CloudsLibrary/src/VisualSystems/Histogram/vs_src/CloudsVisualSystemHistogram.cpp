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
    
	customGui = new ofxUISuperCanvas("HISTOGRAM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Histogram");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addSpacer();
    
    vector<string> modes;
    modes.push_back("BARS");
    modes.push_back("LINES");
    modes.push_back("POINTS");
    customGui->addLabel("MODE");
    customGui->addRadio("MODES", modes);
    
    vector<string> sources;
    sources.push_back("RANDOM");
    sources.push_back("AUDIO");
    customGui->addLabel("SOURCE");
    customGui->addRadio("SOURCES", sources);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemHistogram::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemHistogram::selfGuiEvent(ofxUIEventArgs &e)
{
	if (e.widget->getName() == "BARS" && ((ofxUIToggle *)e.widget)->getValue()) {
        mode = HISTOGRAM_MODE_BARS;
	}
    else if (e.widget->getName() == "LINES" && ((ofxUIToggle *)e.widget)->getValue()) {
        mode = HISTOGRAM_MODE_LINES;
	}
    else if (e.widget->getName() == "POINTS" && ((ofxUIToggle *)e.widget)->getValue()) {
        mode = HISTOGRAM_MODE_POINTS;
	}
    
    else if (e.widget->getName() == "RANDOM" && ((ofxUIToggle *)e.widget)->getValue()) {
        source = HISTOGRAM_SOURCE_RANDOM;
	}
    else if (e.widget->getName() == "AUDIO" && ((ofxUIToggle *)e.widget)->getValue()) {
        source = HISTOGRAM_SOURCE_AUDIO;
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
void CloudsVisualSystemHistogram::selfSetup()
{
    seed = int(ofRandom(20));
    maxNumDataPoints = 3200;
    
    colorClear.set(0, 0, 0, 0);
    
    mode = HISTOGRAM_MODE_BARS;
    source = HISTOGRAM_SOURCE_RANDOM;
    
    maxCols = 100;
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
    
    t = ofGetFrameNum() / 50.0;
    
    while (dataPoints.size() < maxNumDataPoints) {
        addRandomPoint();
    }
    
    ////////////////////////////////////////////////////////////////////////
    // DELETE THE FIRST ITEM IN VECTOR, MOVE EVERY VALUE UP ONE, ADD A NUMBER TO THE END
    
    // Generate a new noise value
    addRandomPoint();
    
    while (dataPoints.size() > maxNumDataPoints) {
        dataPoints.erase(dataPoints.begin());
    }
    
    ////////////////////////////////////////////////////////////////////////
    // ADD VERTICES TO THE MESH
    
    histo.clear();
    if (mode == HISTOGRAM_MODE_BARS) {
        histo.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    }
    else if (mode == HISTOGRAM_MODE_LINES) {
        histo.setMode(OF_PRIMITIVE_LINE_STRIP);
    }
    else {
        histo.setMode(OF_PRIMITIVE_POINTS);
    }
    
    int col = 0;
    int row = 0;
    
    for (int j = dataPoints.size()-1; j > 0 ; j--) {
                
        if (col % maxCols == 0) {
            // next row
            row++;
            col = 0;
            xoffset = 0;
        }
        
        col++;
        xoffset = col * rectWidth;
        
        float k = ofMap(j, 0, dataPoints.size(), 255, 80);
        rectHeight = dataPoints[j];
      
        colorFg.setHsb(k, 180 + ofRandom(25), 225 + ofRandom(25), 225);
       
        if (mode == HISTOGRAM_MODE_BARS) {
            // bottom left
            ofPoint a = ofPoint(xpos + xoffset, ypos, row * zoffset);
            // top left
            ofPoint b = ofPoint(xpos + xoffset, ypos + rectHeight, row * zoffset);
            // bottom right
            ofPoint c = ofPoint(xpos + rectWidth + xoffset, ypos, row * zoffset);
            // top right
            ofPoint d = ofPoint(xpos + rectWidth + xoffset, ypos + rectHeight, row * zoffset);
            
            histo.addColor(colorClear);
            histo.addVertex(a);
            
            histo.addColor(colorFg);
            histo.addVertex(a);
            histo.addColor(colorFg);
            histo.addVertex(b);
            histo.addColor(colorFg);
            histo.addVertex(c);
            histo.addColor(colorFg);
            histo.addVertex(d);
            
            histo.addColor(colorClear);
            histo.addVertex(d);
        }
        else {
            ofPoint a = ofPoint(xpos + xoffset, ypos + rectHeight, row * zoffset);
            
            if (col == 1) {
                histo.addColor(colorClear);
                histo.addVertex(a);
            }
            
            histo.addColor(colorFg);
            histo.addVertex(a);
            
            if (col == maxCols) {
                histo.addColor(colorClear);
                histo.addVertex(a);
            }
        }
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

void CloudsVisualSystemHistogram::addRandomPoint()
{
    n = n+1;
    //randomData.push_back(ofRandom(1,100)); // random float between 1 and 100
    noiseValue += ofNoise( n * .01, t) * 10 - 5; //generate noise value
    noiseValue = noiseValue + ofRandom(-70.0, 70.0); // add randomness
    float newValue = ofMap(noiseValue, -500, 2000, 10, 400, true);
    dataPoints.push_back(newValue); // noise value
    //cout << "time: " <<  t << "size of vector: " << randomData.size() << "  current number: " << randomData.at(i) << endl;
    //  cout << "time: " <<  t << "size of vector: " << randomData.size() << "  noise value " << noiseValue << endl;
}
