
//
//  CloudsVisualSystemCandyMountains.cpp
//

#include "CloudsVisualSystemCandyMountains.h"
#include "CloudsRGBDVideoPlayer.h"

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemCandyMountains::selfSetupGui(){
    
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
	customGui->addButton("Custom Button", false);
	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemCandyMountains::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemCandyMountains::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemCandyMountains::selfSetupSystemGui(){
	
}

void CloudsVisualSystemCandyMountains::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemCandyMountains::selfSetupRenderGui(){
    
}

void CloudsVisualSystemCandyMountains::guiRenderEvent(ofxUIEventArgs &e){
	
}

//This is called whenever a new preset is loaded, before selfSetup()
//use it to ensure all your simple variables are initialized to an
//acceptable default state
void CloudsVisualSystemCandyMountains::selfSetDefaults(){
    
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemCandyMountains::selfSetup(){
    ofSetFrameRate(30);
//    ofSetDepthTest(true);
    
    state = 3;
    
    aa.playStems(0);
    cs.setup();
    cs.gui->setVisible(false);
    
    //    vector<ofFloatColor> colors;
    //    for (int i = 0; i < 13; i++) {
    //        colors.push_back(ofFloatColor(ofRandomuf()));
    //    }
    
    //    for (int i = 0; i < 13; i++) {
    //        cs.addColorRef(&colors[i]);
    //    }
    
    cs.assignRandom(true);
    
    tm.setup(&aa, &cs);
    cm.setup(&tm);
    cm.cam = &getCameraRef();
    lm.setup(&cs);
    pm.setup();
    
    useLights = true;
    
    cm.cam->setupPerspective(false);
    updateCam = false;
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemCandyMountains::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemCandyMountains::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemCandyMountains::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemCandyMountains::selfUpdate(){
    aa.updateAnalytics();
    
    //    cs.setHue(ofMap(aa.kurtosisSmoothed.getMean(), 0, aa.maxKurtosis[0], 0.0, 1.0));
    cs.setSaturation(ofMap(aa.centroidSmoothed.getMean(), 30, 55, 0.0, 1.0));
    cs.setBrightness(aa.ampSmoothed.getMean());
    cs.setDistance(ofMap(aa.pitchSmoothed.getMedian(), 50, 90, 0.0, 0.5));
    
    tm.update();
    cm.update();
    lm.update();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemCandyMountains::selfDraw(){
    ofEnableAlphaBlending();
        
    ofFloatColor gradientStart = cs.colorScheme[0][0];
    float complementHue = gradientStart.getHue() + 0.5;
    if (complementHue > 1.0) complementHue-=1.0;
    ofFloatColor gradientEnd;
    gradientEnd.setHue(complementHue);
    gradientEnd.setSaturation(gradientStart.getSaturation() - 0.2);
    
    ofBackgroundGradient(gradientEnd, gradientStart);
    
    cm.begin();
    lm.begin();
    tm.draw();
    lm.end();
    cm.end();
    
    ofDisableAlphaBlending();
}

// draw any debug stuff here
void CloudsVisualSystemCandyMountains::selfDrawDebug(){
    if (state == 1) {
        cs.draw();
    }
    else if (state == 2) {
        aa.drawAnalytics();
    }
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemCandyMountains::selfDrawBackground(){
    
	//we are using this to draw what keywords are missing content
	if(mainKeyword != ""){
		string keystodraw = "PICKED RANDOM PRESET\n";
		keystodraw += mainKeyword + "\n" + ofJoinString(keywords, ",");
		ofPushMatrix();
		ofScale(5,5);
		ofDrawBitmapString(keystodraw, 20,20);
		ofPopMatrix();
	}
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemCandyMountains::selfEnd(){
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemCandyMountains::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemCandyMountains::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemCandyMountains::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemCandyMountains::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCandyMountains::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCandyMountains::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemCandyMountains::selfMouseReleased(ofMouseEventArgs& data){
	
}


















/*




//my stuff

#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

    ofSetFrameRate(30);
    ofSetDepthTest(true);
    state = 3;
    
    aa.playStems(0);
    cs.setup();
    cs.gui->setVisible(false);
    
//    vector<ofFloatColor> colors;
//    for (int i = 0; i < 13; i++) {
//        colors.push_back(ofFloatColor(ofRandomuf()));
//    }
    
//    for (int i = 0; i < 13; i++) {
//        cs.addColorRef(&colors[i]);
//    }
    
    cs.assignRandom(true);
    
    tm.setup(&aa, &cs);
    cm.setup(&tm);
    lm.setup(&cs);
    pm.setup();
    
    useLights = true;
    
    setupGUI();

    cm.cam.setupPerspective(false);
    updateCam = false;



}

//--------------------------------------------------------------
void testApp::update(){
    aa.updateAnalytics();
    
//    cs.setHue(ofMap(aa.kurtosisSmoothed.getMean(), 0, aa.maxKurtosis[0], 0.0, 1.0));
    cs.setSaturation(ofMap(aa.centroidSmoothed.getMean(), 30, 55, 0.0, 1.0));
    cs.setBrightness(aa.ampSmoothed.getMean());
    cs.setDistance(ofMap(aa.pitchSmoothed.getMedian(), 50, 90, 0.0, 0.5));

    tm.update();
    cm.update();
    lm.update();
    
    gui->update();
    
}

//--------------------------------------------------------------
void testApp::draw(){

    ofEnableAlphaBlending();
    
    if (state == 0) {
        pm.post.begin(cm.cam);
        if (useLights) lm.begin();
        
        ofFloatColor bgColor = cs.colorScheme[0][0];
        float complementHue = bgColor.getHue() + 0.5;
        if (complementHue > 1.0) complementHue-=1.0;
        bgColor.setHue(complementHue);
        bgColor.setSaturation(bgColor.getSaturation() - 0.2);

        ofBackground(bgColor);
        tm.draw();
        
        if (useLights) lm.end();
        pm.post.end();
    }
    else if (state == 1) {
        cs.draw();
    }
    else if (state == 2) {
        aa.drawAnalytics();
    }
    else if (state == 3) {

        ofFloatColor gradientStart = cs.colorScheme[0][0];
        float complementHue = gradientStart.getHue() + 0.5;
        if (complementHue > 1.0) complementHue-=1.0;
        ofFloatColor gradientEnd;
        gradientEnd.setHue(complementHue);
        gradientEnd.setSaturation(gradientStart.getSaturation() - 0.2);
        
        ofBackgroundGradient(gradientEnd, gradientStart);
        
        cm.cam.begin();
        lm.begin();
        tm.draw();
        lm.end();
        cm.cam.end();

    }

    ofDisableAlphaBlending();
}



//--------------------------------------------------------------
void testApp::setupGUI(){
    
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 60);
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("camX", -260, 260, &cm.camX, length-xInit, dim);
    gui->addSlider("camY", 0.0, 500, &cm.camY, length-xInit, dim);
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("lookatX", -260, 260, &cm.lookatX, length-xInit, dim);
    gui->addSlider("lookatY", -500, 500, &cm.lookatY, length-xInit, dim);
    
    gui->toggleVisible();
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}


//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    int kind = e.widget->getKind();

}



//--------------------------------------------------------------
void testApp::keyPressed(int key){
    pm.keyPressed(key);
    
    switch (key) {

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            aa.setMode(key - 48);
            break;
            
            
        case 'f':
            ofToggleFullscreen();
            break;
            
        case 'g':
            gui->toggleVisible();
            cs.gui->toggleVisible();
            break;
            
            
        case 's':
            state = (state + 1) % 4;
            if (state == 0){
                cs.gui->setVisible(false);
            }
            else if (state == 1) {
                gui->setVisible(false);
                cs.gui->setVisible(true);
            }
            else if (state == 2) {
                gui->setVisible(false);
                cs.gui->setVisible(false);
            }
            break;
            
        case 'p':
            aa.playStems(0);
            break;
        
        case 'r':
            cs.assignRandom(true);
            break;
            
        case 'k':
            
            cout << "kurtosis " << aa.audioFeatures[0]->spectralFeatures["kurtosis"] << endl;
            break;
            
        case 'l':
            useLights = !useLights;
            break;
        
//        case 'n':
//			shouldRenderNormals ^= true;
//			break;
//		case 'q':
//			shouldUseFlatShading ^= true;
//			break;
            
        case 'c':
            updateCam = !updateCam;
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}*/
