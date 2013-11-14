//
//  CloudsVisualSystemOpenP5Caustics.cpp
//

#include "CloudsVisualSystemOpenP5Caustics.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Caustics::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("CAUSTICS", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Caustics");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addSpacer();
    customGui->addButton("CLEAR", &bClear);
    customGui->addButton("RESET", &bReset);
    
    customGui->addSpacer();
    customGui->addSlider("R INTENSITY", 0, 100, &castManager->cR->intensity);
    customGui->addSlider("G INTENSITY", 0, 100, &castManager->cG->intensity);
    customGui->addSlider("B INTENSITY", 0, 100, &castManager->cB->intensity);
    
    customGui->addSpacer();
    customGui->addSlider("NOISE SCALE", 0, 50, castManager->noiseScale * 1500.0f);
    customGui->addSlider("BOX WIDTH", 0, MAX(ofGetScreenWidth(), ofGetScreenHeight()), &castManager->boxW);
    customGui->addSlider("BOX HEIGHT", 0, MAX(ofGetScreenWidth(), ofGetScreenHeight()), &castManager->boxH);
    customGui->addSlider("DEPTH", 1, 200, 100);
    customGui->addSlider("IOR", 1.0f, 3.0f, 1.3f);
    customGui->addSlider("IOR DIVERGENCE", 0.0f, 0.2f, nDiv);
    
    customGui->addSpacer();
    vector<string> modes;
    modes.push_back("SPIRAL");
    modes.push_back("SPIRAL CLEAR");
    modes.push_back("RANDOM");
    customGui->addRadio("EMIT MODE", modes);
    
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5Caustics::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Caustics::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "NOISE SCALE") {
        castManager->noiseScale = ((ofxUISlider *)e.widget)->getScaledValue() / 1500.0f;
    }
    
    else if (e.widget->getName() == "SPIRAL") {
        castManager->algo = kCastAlgoSpiral;
    }
    else if (e.widget->getName() == "SPIRAL CLEAR") {
        castManager->algo = kCastAlgoSpiralClear;
    }
    else if (e.widget->getName() == "RANDOM") {
        castManager->algo = kCastAlgoRandom;
    }

    else if (e.widget->getName() == "DEPTH") {
        castManager->cR->d = castManager->cG->d = castManager->cB->d = ((ofxUISlider *)e.widget)->getScaledValue();
    }
    else if (e.widget->getName() == "IOR") {
        float val = ((ofxUISlider *)e.widget)->getScaledValue();
        castManager->cR->n = val;
        castManager->cG->n = val + nDiv;
        castManager->cB->n = val + nDiv * 2;
    }
    else if (e.widget->getName() == "IOR DIVERGENCE") {
        float val = castManager->cR->n;
        castManager->cG->n = val + nDiv;
        castManager->cB->n = val + nDiv * 2;
    }
    
    if (e.widget->getName() == "R INTENSITY" ||
        e.widget->getName() == "G INTENSITY" ||
        e.widget->getName() == "B INTENSITY" ||
        e.widget->getName() == "NOISE SCALE" ||
        e.widget->getName() == "DEPTH" ||
        e.widget->getName() == "IOR" ||
        e.widget->getName() == "IOR DIVERGENCE") {
        clear();
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5Caustics::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5Caustics::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5Caustics::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5Caustics::guiRenderEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Caustics::selfSetup()
{
    width = ofGetWidth();
    height = ofGetHeight();
    numPixels = width * height;
    
    outputTexture.allocate(width, height, GL_RGB);
    outputPixels = new unsigned char[numPixels * 3];
    for (int i = 0; i < numPixels; i++) {
        outputPixels[i * 3 + 0] = 0;  // R
        outputPixels[i * 3 + 1] = 0;  // G
        outputPixels[i * 3 + 2] = 0;  // B
    }
    castManager = new CastManager(width, height);
    castManager->start();
    
    nDiv = 0.02;
    bReset = false;
    bClear = false;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Caustics::reset()
{
    castManager->reseed();
    castManager->reset();
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Caustics::clear()
{
    castManager->reset();
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Caustics::selfPresetLoaded(string presetPath)
{

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5Caustics::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5Caustics::selfSceneTransformation(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Caustics::selfUpdate()
{
    if (bClear || castManager->algo == kCastAlgoSpiralClear) {
        clear();
        bClear = false;
    }
    
    if (bReset) {
        reset();
        bReset = false;
    }
    
    float sum = 0;
    float maxV = 0;
    float count = 0;
    for (int i = 0; i < castManager->cR->count; i++) {
        float m = MAX(castManager->cR->bg[i], MAX(castManager->cG->bg[i], castManager->cB->bg[i]));
        sum += m;
        maxV = MAX(maxV, m);
        if (m != 0) count++;
    }
    float avg = sum / count;

    for (int i = 0; i < numPixels; i++) {
        if (ofGetKeyPressed('n')) {
            int c = ofNoise(i % width * castManager->noiseScale, i / width * castManager->noiseScale, castManager->z * castManager->noiseScale) * 255;
            outputPixels[i * 3 + 0] = c;  // R
            outputPixels[i * 3 + 1] = c;  // G
            outputPixels[i * 3 + 2] = c;  // B
        }
        else {
            outputPixels[i * 3 + 0] = MAX(0, MIN(255, castManager->cR->bg[i] * 128 / avg));  // R
            outputPixels[i * 3 + 1] = MAX(0, MIN(255, castManager->cG->bg[i] * 128 / avg));  // G
            outputPixels[i * 3 + 2] = MAX(0, MIN(255, castManager->cB->bg[i] * 128 / avg));  // B
        }
    }

    outputTexture.loadData(outputPixels, width, height, GL_RGB);
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Caustics::selfDraw()
{

}

// draw any debug stuff here
void CloudsVisualSystemOpenP5Caustics::selfDrawDebug(){

}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Caustics::selfDrawBackground()
{
    ofSetColor(255);
    outputTexture.draw(0, 0);
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5Caustics::selfEnd(){

	
}

void CloudsVisualSystemOpenP5Caustics::selfExit()
{
    castManager->stop();
    delete castManager;
    
    numPixels = 0;
    delete [] outputPixels;
    
    outputTexture.clear();
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5Caustics::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5Caustics::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5Caustics::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystemOpenP5Caustics::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Caustics::selfMousePressed(ofMouseEventArgs& data){
    
}

void CloudsVisualSystemOpenP5Caustics::selfMouseReleased(ofMouseEventArgs& data){

}
