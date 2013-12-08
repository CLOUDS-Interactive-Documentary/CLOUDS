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
    
    customGui->addSpacer();
    vector<string> sources;
    sources.push_back("RANDOM");
    sources.push_back("AUDIO");
    customGui->addLabel("SOURCE");
    customGui->addRadio("SOURCES", sources);
    
    customGui->addSpacer();
    customGui->addLabel("SOUND");
    vector<string> soundNames;
    for (int i = 0; i < soundsDir.size(); i++) {
        soundNames.push_back(soundsDir.getName(i));
    }
    customGui->addRadio("SOUNDS", soundNames);
    
    customGui->addSlider("LEVEL ADJUST", 0.0f, 10.0f, &levelAdjust);
    
    customGui->addSpacer();
    customGui->addLabel("COLOR");
    customGui->addRangeSlider("HUE RANGE", 0, 255, &hueMin, &hueMax);
    customGui->addSlider("SAT BASE", 0, 255, &satBase);
    customGui->addSlider("SAT RANGE", 0, 255, &satRange);
    customGui->addSlider("BRI BASE", 0, 255, &briBase);
    customGui->addSlider("BRI RANGE", 0, 255, &briRange);
    customGui->addSlider("ALPHA", 0, 255, &alpha);
    
    customGui->addSpacer();
    customGui->addLabel("DIMENSIONS");
    customGui->addIntSlider("COLS PER ROW", 1, 100, &colsPerRow);
    customGui->addSlider("ROW SPACER", 1, 200, &rowSpacer);
    customGui->addSlider("COL SPACER", 0, 50, &colSpacer);
    customGui->addSlider("COL WIDTH", 0, 100, &colWidth);
    customGui->addRangeSlider("COL HEIGHT", 0, 500, &colHeightMin, &colHeightMax);
    customGui->addSlider("LINE WIDTH", 0.1, 10, &lineWidth);
	
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
    
    else {
        // Let's look through the files dropdown for a match.
        string name = e.widget->getName();
        for (int i = 0; i < soundsDir.numFiles(); i++) {
            if (name == soundsDir.getName(i) && ((ofxUIToggle *)e.widget)->getValue()) {
                selectedSoundsIdx = i;
                reloadSound();
                break;
            }
        }
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
    
    mode = HISTOGRAM_MODE_BARS;
    source = HISTOGRAM_SOURCE_RANDOM;
    
    colorClear.set(0, 0, 0, 0);
    
    hueMax   = 255;
    hueMin   =  80;
    satBase  = 180;
    satRange =  25;
    briBase  = 225;
    briRange =  25;
    alpha    = 225;
    
    rowSpacer = 150;
    colSpacer = 0;
    colWidth = 40;
    colHeightMin = 10;
    colHeightMax = 400;
    colsPerRow = 100;
    lineWidth = 1.0f;

    soundsDir.listDir(getVisualSystemDataPath() + "sounds");
    soundsDir.sort();
    selectedSoundsIdx = 0;
    
    levelAdjust = 1.0f;
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
void CloudsVisualSystemHistogram::selfUpdate()
{
    ////////////////////////////////////////////////////////////////////////
    // FILL THE VECTOR WITH DATA

    t = ofGetFrameNum() / 50.0;
    
    if (source == HISTOGRAM_SOURCE_RANDOM) {
        while (dataPoints.size() < maxNumDataPoints) {
            addRandomPoint();
        }
    
        // Generate a new noise value
        addRandomPoint();
    }
    else {
        addSoundPoint();
    }
    
    while (dataPoints.size() > maxNumDataPoints) {
        dataPoints.erase(dataPoints.begin());
    }
    
    ////////////////////////////////////////////////////////////////////////
    // ADD VERTICES TO THE MESH
    
    histoMesh.clear();
    if (mode == HISTOGRAM_MODE_BARS) {
        histoMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    }
    else if (mode == HISTOGRAM_MODE_LINES) {
        histoMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    }
    else {
        histoMesh.setMode(OF_PRIMITIVE_POINTS);
    }
    
    int col = 0;
    int row = 0;
    
    for (int j = dataPoints.size()-1; j > 0 ; j--) {
        float offsetX = col * (colWidth + colSpacer);
        
        float hue = ofMap(j, 0, dataPoints.size(), hueMax, hueMin);
        colorFg.setHsb(hue, satBase + ofRandom(satRange), briBase + ofRandom(briRange), alpha);
       
        if (mode == HISTOGRAM_MODE_BARS) {
            // bottom left
            ofPoint a = ofPoint(offsetX, 0, row * -rowSpacer);
            // top left
            ofPoint b = ofPoint(offsetX, dataPoints[j], row * -rowSpacer);
            // bottom right
            ofPoint c = ofPoint(colWidth + offsetX, 0, row * -rowSpacer);
            // top right
            ofPoint d = ofPoint(colWidth + offsetX, dataPoints[j], row * -rowSpacer);
            
            histoMesh.addColor(colorClear);
            histoMesh.addVertex(a);
            
            histoMesh.addColor(colorFg);
            histoMesh.addVertex(a);
            histoMesh.addColor(colorFg);
            histoMesh.addVertex(b);
            histoMesh.addColor(colorFg);
            histoMesh.addVertex(c);
            histoMesh.addColor(colorFg);
            histoMesh.addVertex(d);
            
            histoMesh.addColor(colorClear);
            histoMesh.addVertex(d);
        }
        else {
            ofPoint a = ofPoint(offsetX, dataPoints[j], row * -rowSpacer);
            
            if (col == 0) {
                histoMesh.addColor(colorClear);
                histoMesh.addVertex(a);
            }
            
            histoMesh.addColor(colorFg);
            histoMesh.addVertex(a);
            
            if (col == colsPerRow - 1) {
                histoMesh.addColor(colorClear);
                histoMesh.addVertex(a);
            }
        }
        
        col++;
        if (col % colsPerRow == 0) {
            // next row
            row++;
            col = 0;
        }
    }
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemHistogram::selfDraw()
{
    ofPushMatrix();
    ofPushStyle();
    {
        ofEnableAlphaBlending();
        ofTranslate((colsPerRow * (colWidth + colSpacer)) * -0.5, 0);
 
        if (mode == HISTOGRAM_MODE_LINES) {
            ofSetLineWidth(lineWidth);
        }
        else if (mode == HISTOGRAM_MODE_POINTS) {
            glPointSize(lineWidth);
        }
        histoMesh.draw();
    }
	ofPopStyle();
    ofPopMatrix();	
}

// draw any debug stuff here
void CloudsVisualSystemHistogram::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemHistogram::selfDrawBackground(){

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

void CloudsVisualSystemHistogram::reloadSound()
{
    // close whatever sound was previously open
    soundPlayer.stop();
    soundPlayer.unloadSound();
    
    ofFile file = soundsDir.getFile(selectedSoundsIdx);
    soundPlayer.loadSound(file.getAbsolutePath());
    soundPlayer.play();
    soundPlayer.setLoop(true);
}

void CloudsVisualSystemHistogram::addRandomPoint()
{
    n = n+1;
    //randomData.push_back(ofRandom(1,100)); // random float between 1 and 100
    noiseValue += ofNoise(n * .01, t) * 10 - 5; //generate noise value
    noiseValue = noiseValue + ofRandom(-70.0, 70.0); // add randomness
    float newValue = ofMap(noiseValue, -500, 2000, colHeightMin, colHeightMax, true);
    dataPoints.push_back(newValue); // noise value
    //cout << "time: " <<  t << "size of vector: " << randomData.size() << "  current number: " << randomData.at(i) << endl;
    //  cout << "time: " <<  t << "size of vector: " << randomData.size() << "  noise value " << noiseValue << endl;
}

void CloudsVisualSystemHistogram::addSoundPoint()
{
    ofSoundUpdate();
    float currLevel = ofSoundGetSpectrum(1)[0] * levelAdjust;
    float newValue = ofMap(currLevel, 0, 1, colHeightMin, colHeightMax, true);
    dataPoints.push_back(newValue);
}
