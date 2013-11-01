//
//  CloudsVisualSystemOpenP5DrawingMachine10.cpp
//

#include "CloudsVisualSystemOpenP5DrawingMachine10.h"


//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("ORBITAL", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Orbital");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5DrawingMachine10::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfGuiEvent(ofxUIEventArgs &e)
{
    
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5DrawingMachine10::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5DrawingMachine10::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5DrawingMachine10::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5DrawingMachine10::guiRenderEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfSetup()
{
    canvas.allocate(ofGetWidth(), ofGetHeight());
    canvas.begin();
    {
        ofClear(0, 0);
    }
    canvas.end();
    
    // Set defaults.
    numAttractors = 200;
    bSleeping = false;
    bDrawPoints = true;
    bUseLimits = true;
    
    // Init the system.
    bunch = new DM10Bunch(5, 5, 20);
    for (int i = 0; i < numAttractors; i++) {
        attractors.push_back(ofVec2f(ofRandomWidth(), ofRandomHeight()));
    }
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5DrawingMachine10::selfPresetLoaded(string presetPath){

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5DrawingMachine10::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5DrawingMachine10::selfSceneTransformation(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfUpdate()
{
    if (!bSleeping) {
        if (bUseLimits) {
            bunch->update(attractors, 150);
        }
        else {
            bunch->update(attractors);
        }
    }
    
    cout << ofGetFrameRate() << endl;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfDraw()
{

}

// draw any debug stuff here
void CloudsVisualSystemOpenP5DrawingMachine10::selfDrawDebug(){

}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfDrawBackground()
{
    canvas.begin();
    {
        //    ofBackground(255);
        ofSetColor(0, 128);
        ofNoFill();
        
        if (bDrawPoints) {
            bunch->drawPoints();
        }
        else {
            bunch->drawLines();
        }
    }
    canvas.end();
    
    canvas.draw(0, 0);
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5DrawingMachine10::selfEnd(){

	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfExit()
{
    delete bunch;
    attractors.clear();
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5DrawingMachine10::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5DrawingMachine10::selfKeyReleased(ofKeyEventArgs & args){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfMouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemOpenP5DrawingMachine10::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5DrawingMachine10::selfMousePressed(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfMouseReleased(ofMouseEventArgs& data)
{

}