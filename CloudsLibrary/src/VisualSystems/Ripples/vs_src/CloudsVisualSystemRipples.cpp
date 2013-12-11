//
//  CloudsVisualSystemRipples.cpp
//

#include "CloudsVisualSystemRipples.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemRipples::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("RIPPLES", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Ripples");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSpacer();
    customGui->addButton("RESTART", &bRestart);
    customGui->addSlider("DAMPING", 0, 1, &damping);
    customGui->addSlider("RADIUS", 1.0, 50.0, &radius);
    
    customGui->addSpacer();
    customGui->addToggle("DROP ON PRESS", &bDropOnPress);
    customGui->addIntSlider("RATE", 1, 30, &dropRate);
    
    customGui->addSpacer();
    customGui->addRangeSlider("DROP HUE", 0, 255, &minDropHue, &maxDropHue);
    customGui->addRangeSlider("DROP SAT", 0, 255, &minDropSat, &maxDropSat);
    customGui->addRangeSlider("DROP BRI", 0, 255, &minDropBri, &maxDropBri);
    
    customGui->addSpacer();
    tintHue = new ofx1DExtruder(0);
    tintHue->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(tintHue);
    customGui->addSlider("TINT HUE", 0.0, 255.0, tintHue->getPosPtr());
    tintSat = new ofx1DExtruder(0);
    tintSat->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(tintSat);
    customGui->addSlider("TINT SAT", 0.0, 255.0, tintSat->getPosPtr());
    tintBri = new ofx1DExtruder(0);
    tintBri->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(tintBri);
    customGui->addSlider("TINT BRI", 0.0, 255.0, tintBri->getPosPtr());
    tintAlpha = new ofx1DExtruder(0);
    tintAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(tintAlpha);
    customGui->addSlider("TINT ALPHA", 0.0, 255.0, tintAlpha->getPosPtr());
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemRipples::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemRipples::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "TINT HUE") {
        tintHue->setPosAndHome(tintHue->getPos());
	}
    else if (e.widget->getName() == "TINT SAT") {
        tintSat->setPosAndHome(tintSat->getPos());
	}
    else if (e.widget->getName() == "TINT BRI") {
        tintBri->setPosAndHome(tintBri->getPos());
	}
    else if (e.widget->getName() == "TINT ALPHA") {
        tintAlpha->setPosAndHome(tintAlpha->getPos());
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemRipples::selfSetupSystemGui(){
	
}

void CloudsVisualSystemRipples::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemRipples::selfSetupRenderGui(){

}

void CloudsVisualSystemRipples::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemRipples::selfSetup()
{
    bIs2D = true;
//    bClearBackground = false;
    
    // Set defaults.
    bDropOnPress = false;
    dropRate = 1;
    damping = 0.995f;
    radius = 10.0f;
    
    minDropHue = 0;
    maxDropHue = 0;
    minDropSat = 0;
    maxDropSat = 0;
    minDropBri = 255;
    maxDropBri = 255;
    
    // Load the ripples shader.
    ripplesShader.load("", getVisualSystemDataPath() + "shaders/ripples.frag");
    
    bRestart = true;
}

void CloudsVisualSystemRipples::restart()
{
    float width = ofGetWidth();
    float height = ofGetHeight();
    
    ripplesSrcFbo.allocate(width, height, GL_RGBA32F);
    ripplesSrcFbo.begin();
    {
        ofClear(0, 0);
    }
    ripplesSrcFbo.end();
    
    ripplesDstFbo.allocate(width, height, GL_RGBA32F);
    ripplesDstFbo.begin();
    {
        ofClear(0, 0);
    }
    ripplesDstFbo.end();
    
    // Build a mesh to render a quad.
    renderMesh.clear();
    renderMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    renderMesh.addVertex(ofVec3f(0, 0));
    renderMesh.addVertex(ofVec3f(width, 0));
    renderMesh.addVertex(ofVec3f(width, height));
    renderMesh.addVertex(ofVec3f(0, height));
    renderMesh.addTexCoord(ofVec2f(0, 0));
    renderMesh.addTexCoord(ofVec2f(width, 0));
    renderMesh.addTexCoord(ofVec2f(width, height));
    renderMesh.addTexCoord(ofVec2f(0, height));
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemRipples::selfPresetLoaded(string presetPath)
{
    bRestart = true;
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemRipples::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemRipples::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemRipples::selfUpdate()
{    
    if (bRestart || ripplesSrcFbo.getWidth() != ofGetWidth() || ripplesSrcFbo.getHeight() != ofGetHeight()) {
        restart();
        bRestart = false;
    }
    
    tintColor.setHsb(tintHue->getPos(), tintSat->getPos(), tintBri->getPos(), tintAlpha->getPos());
    dropColor.setHsb(ofRandom(minDropHue, maxDropHue), ofRandom(minDropSat, maxDropSat), ofRandom(minDropBri, maxDropBri));
    
    if ((bDropOnPress && GetCloudsInputPressed()) || (!bDropOnPress && ofGetFrameNum() % dropRate == 0)) {
        ofPushStyle();
        ofPushMatrix();
        ripplesSrcFbo.begin();
        {
            ofSetColor(dropColor);
            ofNoFill();
#ifdef OCULUS_RIFT
            // I don't know why everything is flipped, but it is.
            ofCircle(ofGetHeight() - GetCloudsInputY(), ofGetWidth() - GetCloudsInputX(), radius);
#else
            ofCircle(GetCloudsInputX(), GetCloudsInputY(), radius);
#endif
        }
        ripplesSrcFbo.end();
        ofPopMatrix();
        ofPopStyle();
    }
    
    ripplesDstFbo.begin();
    ripplesShader.begin();
    ripplesShader.setUniformTexture("backbuffer", ripplesDstFbo.getTextureReference(), 1);
    ripplesShader.setUniformTexture("tex0", ripplesSrcFbo.getTextureReference(), 2);
    ripplesShader.setUniform1f("damping", damping / 10.0f + 0.9f);  // 0.9 - 1.0 range
    {
        ofSetColor(tintColor);
        renderMesh.draw();
    }
    ripplesShader.end();
    ripplesDstFbo.end();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemRipples::selfDraw(){
    
}

// draw any debug stuff here
void CloudsVisualSystemRipples::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemRipples::selfDrawBackground()
{    
    ofSetColor(255, 255);
    
    ofPushStyle();
    ofEnableAlphaBlending();
    {
        ripplesDstFbo.draw(0, 0);
    }
    ofPopStyle();
    
    swap(ripplesSrcFbo, ripplesDstFbo);
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemRipples::selfEnd(){
	
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemRipples::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemRipples::selfKeyPressed(int key){

}

void CloudsVisualSystemRipples::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemRipples::selfMouseDragged(int x, int y, int button){

}

void CloudsVisualSystemRipples::selfMouseMoved(int x, int y, int button){
	
}

void CloudsVisualSystemRipples::selfMousePressed(int x, int y, int button){
	
}

void CloudsVisualSystemRipples::selfMouseReleased(int x, int y, int button){
	
}