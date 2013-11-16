//
//  CloudsVisualSystemAutomata.cpp
//

#include "CloudsVisualSystemAutomata.h"

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addSpacer();
    customGui->addToggle("RESTART", &bRestart);
    customGui->addToggle("2D", &bIs2D);
    customGui->addSlider("RADIUS", 1.0, 50.0, &radius);
    
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
    
    customGui->addSpacer();
    bgAlpha = new ofx1DExtruder(0);
    bgAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(bgAlpha);
    customGui->addSlider("BG ALPHA", 0.0, 255.0, bgAlpha->getPosPtr());
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemAutomata::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "FG HUE") {
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
    else if (e.widget->getName() == "BG ALPHA") {
        bgAlpha->setPosAndHome(bgAlpha->getPos());
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemAutomata::selfSetupSystemGui(){
	
}

void CloudsVisualSystemAutomata::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemAutomata::selfSetupRenderGui(){

}

void CloudsVisualSystemAutomata::guiRenderEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::selfSetup()
{
    // Load the shaders.
    shader.load("", getVisualSystemDataPath() + "shaders/conway.frag");
    bIs2D = true;
	
    // Set defaults.
    radius = 5.0f;
    bRestart = true;
}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::restart()
{
    float width = ofGetWidth();
    float height = ofGetHeight();
    
    ofFbo::Settings fboSettings = ofFbo::Settings::Settings();
    fboSettings.width = width;
    fboSettings.height = height;
    fboSettings.internalformat = GL_RGBA;
    fboSettings.minFilter = GL_NEAREST;
    fboSettings.maxFilter = GL_NEAREST;
    
    texFbo.allocate(fboSettings);
    texFbo.begin();
    {
        ofClear(0, 0);
    }
    texFbo.end();

    outFbo.allocate(fboSettings);
    outFbo.begin();
    {
        ofClear(0, 0);
    }
    outFbo.end();
    
    // Build a mesh to render a quad.
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
}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::selfPresetLoaded(string presetPath)
{
    bRestart = true;
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemAutomata::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemAutomata::selfSceneTransformation(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::selfUpdate()
{
    fgColor.setHsb(fgHue->getPos(), fgSat->getPos(), fgBri->getPos(), fgAlpha->getPos());
    bgColor.a = bgAlpha->getPos();
    
    if (bRestart || outFbo.getWidth() != ofGetWidth() || outFbo.getHeight() != ofGetHeight()) {
        restart();
        bRestart = false;
    }
    
    ofPushStyle();
    ofEnableAlphaBlending();
    texFbo.begin();
    {
//        ofClear(255, 255);

        ofSetColor(255);
        outFbo.draw(0, 0);
        
        ofSetColor(fgColor);
        ofCircle(ofGetMouseX(), ofGetMouseY(), radius);
    }
    texFbo.end();
    ofPopStyle();
    
    ofPushStyle();
    ofEnableAlphaBlending();
    outFbo.begin();
    shader.begin();
    shader.setUniformTexture("tex", texFbo.getTextureReference(), 1);
    shader.setUniform4f("liveColor", fgColor.r / 255.0f, fgColor.g / 255.0f, fgColor.b / 255.0f, fgColor.a / 255.0f);
    shader.setUniform4f("deadColor", bgColor.r / 255.0f, bgColor.g / 255.0f, bgColor.b / 255.0f, bgColor.a / 255.0f);
    {
//        ofClear(0, 255);

        ofSetColor(255);
        mesh.draw();
    }
    shader.end();
    outFbo.end();
    ofPopStyle();
}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::selfDraw()
{
    if (!bIs2D) {
        ofPushMatrix();
        ofScale(1, -1, 1);
        ofTranslate(-ofGetWidth() / 2, -ofGetHeight() / 2);
        ofPushStyle();
        ofEnableAlphaBlending();
        ofDisableLighting();
        {
            ofSetColor(255, 255);
            outFbo.draw(0, 0);
        }
        ofPopStyle();
        ofPopMatrix();
    }
}

// draw any debug stuff here
void CloudsVisualSystemAutomata::selfDrawDebug(){

}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::selfDrawBackground()
{
    if (bIs2D) {
        ofPushStyle();
        ofEnableAlphaBlending();
        {
            ofSetColor(255, 255);
            outFbo.draw(0, 0);
        }
        ofPopStyle();
    }
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemAutomata::selfEnd(){

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemAutomata::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemAutomata::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemAutomata::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemAutomata::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemAutomata::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemAutomata::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemAutomata::selfMouseReleased(ofMouseEventArgs& data){
	
}