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
    /*
	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
//	customGui->addSlider("Color 1 Hue", 0, 255, &color1HSB.r);
//	customGui->addSlider("Color 1 Sat", 0, 255, &color1HSB.g);
//	customGui->addSlider("Color 1 Bri", 0, 255, &color1HSB.b);
//
//	customGui->addSlider("Color 2 Hue", 0, 255, &color2HSB.r);
//	customGui->addSlider("Color 2 Sat", 0, 255, &color2HSB.g);
//	customGui->addSlider("Color 2 Bri", 0, 255, &color2HSB.b);
	
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemRipples::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
     */
}

void CloudsVisualSystemRipples::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
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
    
    float width = ofGetWidth();
    float height = ofGetHeight();
        
    // Load the ripples shader and FBOs.
    ripplesShader.load("", getVisualSystemDataPath() + "shaders/ripples.frag");
    
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
void CloudsVisualSystemRipples::selfPresetLoaded(string presetPath){

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
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    ofPushStyle();
    ofPushMatrix();
    ripplesSrcFbo.begin();
    {
        ofSetColor(ofNoise(ofGetFrameNum()) * 255 * 5, 255);
        ofCircle(ofGetMouseX(), ofGetMouseY(), 10, 10);
    }
    ripplesSrcFbo.end();
    ofPopMatrix();
    ofPopStyle();
    
    ripplesDstFbo.begin();
    ripplesShader.begin();
    ripplesShader.setUniformTexture("backbuffer", ripplesDstFbo.getTextureReference(), 0);
    ripplesShader.setUniformTexture("tex0", ripplesSrcFbo.getTextureReference(), 1);
    ripplesShader.setUniform1f("damping", 0.995f);
    {
        ofSetColor(255);
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