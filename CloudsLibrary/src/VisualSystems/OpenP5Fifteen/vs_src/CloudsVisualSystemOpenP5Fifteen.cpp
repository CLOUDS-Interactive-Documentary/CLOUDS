//
//  CloudsVisualSystemOpenP5Fifteen.cpp
//

#include "CloudsVisualSystemOpenP5Fifteen.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Fifteen::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("FIFTEEN", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Fifteen");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addSpacer();
    customGui->addSlider("SCALAR", 0, 1, scalar * 100.0f);
    
    customGui->addSpacer();
    fgHue = new ofx1DExtruder(0);
    fgHue->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(fgHue);
    customGui->addSlider("FG HUE", 0.0, 255.0, fgHue->getPosPtr());
    fgSat = new ofx1DExtruder(0);
    fgSat->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(fgSat);
    customGui->addSlider("FG SAT", 0.0, 255.0, fgSat->getPosPtr());
    fgBri = new ofx1DExtruder(0);
    fgBri->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(fgBri);
    customGui->addSlider("FG BRI", 0.0, 255.0, fgBri->getPosPtr());
    fgAlpha = new ofx1DExtruder(0);
    fgAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(fgAlpha);
    customGui->addSlider("FG ALPHA", 0.0, 255.0, fgAlpha->getPosPtr());
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5Fifteen::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Fifteen::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "SCALAR") {
        scalar = ((ofxUISlider *)e.widget)->getScaledValue() / 100.0f;
    }
    
    else if (e.widget->getName() == "FG HUE") {
        fgHue->setPosAndHome(fgHue->getPos());
	}
    else if (e.widget->getName() == "FG SAT") {
        fgSat->setPosAndHome(fgSat->getPos());
	}
    else if (e.widget->getName() == "FG BRI") {
        fgBri->setPosAndHome(fgBri->getPos());
	}
    else if (e.widget->getName() == "FG ALPHA") {
        fgAlpha->setPosAndHome(fgAlpha->getPos());
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5Fifteen::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5Fifteen::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5Fifteen::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5Fifteen::guiRenderEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Fifteen::selfSetup()
{
    // Load the shaders.
    shader.load("", getVisualSystemDataPath() + "shaders/fifteen.frag");
    bIs2D = true;
	
    // Set defaults.
    scalar = 0.005f;

    restart();
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Fifteen::restart()
{
    float width = ofGetWidth();
    float height = ofGetHeight();
    
    // Update the dimensions in the shader.
    shader.begin();
    shader.setUniform2f("dims", width, height);
    shader.end();
    
    // Allocate/Reset the FBOs.
    if (!srcFbo.isAllocated() || srcFbo.getWidth() != width || srcFbo.getHeight() != height) {
        srcFbo.allocate(width, height, GL_RGB);
    }
    srcFbo.begin();
    {
        ofClear(0, 0);
    }
    srcFbo.end();
    
    if (!dstFbo.isAllocated() || dstFbo.getWidth() != width || dstFbo.getHeight() != height) {
        dstFbo.allocate(width, height, GL_RGB);
    }
    dstFbo.begin();
    {
        ofClear(0, 0);
    }
    dstFbo.end();
    
    // Build a render mesh for drawing onto.
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    mesh.addVertex(ofVec3f(0, 0));
    mesh.addVertex(ofVec3f(width, 0));
    mesh.addVertex(ofVec3f(width, height));
    mesh.addVertex(ofVec3f(0, height));
    mesh.addTexCoord(ofVec2f(0, 0));
    mesh.addTexCoord(ofVec2f(width, 0));
    mesh.addTexCoord(ofVec2f(width, height));
    mesh.addTexCoord(ofVec2f(0, height));
    
    // Reset variables.
    count = 0;
    attractor.set(ofGetMouseX(), ofGetMouseY());
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Fifteen::selfPresetLoaded(string presetPath)
{
    restart();
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5Fifteen::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5Fifteen::selfSceneTransformation(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Fifteen::selfUpdate()
{
    if (srcFbo.getWidth() != ofGetWidth() || srcFbo.getHeight() != ofGetHeight()) {
        restart();
    }
    
    attractor.x += (attractor.x > ofGetMouseX())? -1 : 1;
    attractor.y += (attractor.y > ofGetMouseY())? -1 : 1;
    
    dstFbo.begin();
    shader.begin();
    shader.setUniformTexture("baseTex", srcFbo.getTextureReference(), 1);
    shader.setUniform1f("scalar", scalar);
    shader.setUniform2f("attractor", attractor.x, attractor.y);
    shader.setUniform1i("count", count);
    {
        ofSetColor(255);
        mesh.draw();
    }
    shader.end();
    dstFbo.end();
    
    ++count;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Fifteen::selfDraw()
{

}

// draw any debug stuff here
void CloudsVisualSystemOpenP5Fifteen::selfDrawDebug(){

}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Fifteen::selfDrawBackground()
{
    ofSetColor(255, 255);
    
    ofPushStyle();
    ofEnableAlphaBlending();
    {
        dstFbo.draw(0, 0);
    }
    ofPopStyle();
    
    swap(srcFbo, dstFbo);
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5Fifteen::selfEnd(){

}

void CloudsVisualSystemOpenP5Fifteen::selfExit(){

}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5Fifteen::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5Fifteen::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5Fifteen::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystemOpenP5Fifteen::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Fifteen::selfMousePressed(ofMouseEventArgs& data){
    
}

void CloudsVisualSystemOpenP5Fifteen::selfMouseReleased(ofMouseEventArgs& data){

}
