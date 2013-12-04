//
//  CloudsVisualSystemSwim.cpp
//

#include "CloudsVisualSystemSwim.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemSwim::selfSetup()
{
    bubbles.init(getVisualSystemDataPath());
    creatures.init(getVisualSystemDataPath());
    
    post.init(ofGetWidth(), ofGetHeight(), true);
    post.createPass<FxaaPass>();
    post.createPass<BloomPass>();
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemSwim::selfBegin()
{
    // adding this here as custom gui data is loaded after setup
	creatures.generate();
}

//normal update call
void CloudsVisualSystemSwim::selfUpdate()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
    bubbles.update();
    creatures.update();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemSwim::selfDraw()
{
    creatures.draw();
    bubbles.draw();
}

void CloudsVisualSystemSwim::selfPostDraw()
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    post.process(CloudsVisualSystem::getSharedRenderTarget(), false);
    post.getProcessedTextureReference().draw(0, ofGetHeight(), ofGetWidth(), -ofGetHeight());
    glPopAttrib();
}

//use render gui for display settings, like changing colors
void CloudsVisualSystemSwim::selfSetupRenderGui()
{
    rdrGui->addToggle("regenerate", false);
    
    rdrGui->addLabel("Flocking");
    rdrGui->addSpacer();
    rdrGui->addSlider("zoneRadius", 50.f, 2000.f, &creatures.zoneRadius);
    rdrGui->addSlider("alignmentLower", 0.f, 1.f, &creatures.alignmentLower);
    rdrGui->addSlider("alignmentUpper", 0.f, 1.f, &creatures.alignmentUpper);
    rdrGui->addSlider("repelStrength", 0.f, 1.f, &creatures.repelStrength);
    rdrGui->addSlider("alignStrength", 0.f, 1.f, &creatures.alignStrength);
    rdrGui->addSlider("attractStrength", 0.f, 1.f, &creatures.attractStrength);
    rdrGui->addSlider("maxDistFromCentre", 500.f, 4000.f, &creatures.maxDistFromCentre);
    
    rdrGui->addLabel("Creatures");
    rdrGui->addSpacer();
    rdrGui->addSlider("numJellyOne", 20, 300, &creatures.numJellyOne);
    rdrGui->addSlider("numJellyTwo", 20, 300, &creatures.numJellyTwo);
    rdrGui->addSlider("numGreyFish", 20, 300, &creatures.numGreyFish);
    rdrGui->addSlider("numYellowFish", 20, 300, &creatures.numYellowFish);
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemSwim::selfSetupGui()
{
	jellyOneGui = createCustomGui("Jellyus Oneus");
    addSliders(jellyOneGui, creatures.jellyOneParams, " 1");
    
    jellyTwoGui = createCustomGui("Jellyus Twous");
    addSliders(jellyTwoGui, creatures.jellyTwoParams, " 2");
}

void CloudsVisualSystemSwim::addSliders(ofxUISuperCanvas* gui, JellyParams& params, const string& suffix)
{
    gui->addSpacer();
    
    gui->addMinimalSlider("bell h" + suffix, 0.f, 1.f, &params.bellHsb.x);
    gui->addMinimalSlider("bell s" + suffix, 0.f, 1.f, &params.bellHsb.y);
    gui->addMinimalSlider("bell b" + suffix, 0.f, 1.f, &params.bellHsb.z);
    
    gui->addMinimalSlider("tentacles h" + suffix, 0.f, 1.f, &params.tentacleHsb.x);
    gui->addMinimalSlider("tentacles s" + suffix, 0.f, 1.f, &params.tentacleHsb.y);
    gui->addMinimalSlider("tentacles b" + suffix, 0.f, 1.f, &params.tentacleHsb.z);
    
    gui->addMinimalSlider("width average" + suffix, 10, 200, &params.widthAverage);
    gui->addMinimalSlider("width std dev" + suffix, 0, 200, &params.widthStdDeviation);
    
    gui->addMinimalSlider("length average" + suffix, 10, 200, &params.lengthAverage);
    gui->addMinimalSlider("length std dev" + suffix, 0, 200, &params.lengthStdDeviation);
    
    gui->addRangeSlider("spherical segment" + suffix, HALF_PI, TWO_PI, &params.segmentMin, &params.segmentMax);

    gui->addRangeSlider("superformula m1" + suffix, 2, 20, &params.m1Min, &params.m1Max);
    gui->addRangeSlider("superformula m2" + suffix, 2, 20, &params.m2Min, &params.m2Max);
}

ofxUISuperCanvas* CloudsVisualSystemSwim::createCustomGui(const string& name)
{
    ofxUISuperCanvas* newGui = new ofxUISuperCanvas(name, gui);
	newGui->copyCanvasStyle(gui);
	newGui->copyCanvasProperties(gui);
	newGui->setName("Jellyus Oneus");
	newGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    guis.push_back(newGui);
	guimap[newGui->getName()] = newGui;
    
    return newGui;
}

void CloudsVisualSystemSwim::guiRenderEvent(ofxUIEventArgs &e)
{
	if (e.widget->getName() == "regenerate")
    {
        ofxUIToggle* toggle = static_cast<ofxUIToggle*>(e.widget);
        if (toggle->getValue())
        {
            creatures.generate();
            toggle->setValue(false);
        }
    }
}

void CloudsVisualSystemSwim::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemSwim::selfSetupSystemGui(){
	
}

void CloudsVisualSystemSwim::guiSystemEvent(ofxUIEventArgs &e){
	
}


// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemSwim::selfPresetLoaded(string presetPath) {
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemSwim::selfSceneTransformation(){
	
}





// draw any debug stuff here
void CloudsVisualSystemSwim::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemSwim::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemSwim::selfEnd(){
	
	simplePointcloud.clear();
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemSwim::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemSwim::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemSwim::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemSwim::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSwim::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSwim::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSwim::selfMouseReleased(ofMouseEventArgs& data){
	
}