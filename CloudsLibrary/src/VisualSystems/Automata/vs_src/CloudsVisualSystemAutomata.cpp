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
    customGui->addSlider("FG HUE 1", 0.0f, 0.99999f, &fgParams1[0]);
    customGui->addSlider("FG SAT 1", 0.0f, 1.0f, &fgParams1[1]);
    customGui->addSlider("FG BRI 1", 0.0f, 1.0f, &fgParams1[2]);
    
    customGui->addSpacer();
    customGui->addSlider("FG HUE 2", 0.0f, 0.99999f, &fgParams2[0]);
    customGui->addSlider("FG SAT 2", 0.0f, 1.0f, &fgParams2[1]);
    customGui->addSlider("FG BRI 2", 0.0f, 1.0f, &fgParams2[2]);

    customGui->addSpacer();
    customGui->addSlider("FADE", 0.0, 0.5, &fade);  // Fucks up if I go up to 1.0, don't know why, don't care anymore...
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemAutomata::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::selfGuiEvent(ofxUIEventArgs &e)
{

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
    conwayShader.load("", getVisualSystemDataPath() + "shaders/conway.frag");
    blenderShader.load("", getVisualSystemDataPath() + "shaders/blender.frag");
    bIs2D = true;
	
    // Set defaults.
    radius = 5.0f;

}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::restart()
{
    ofImage seedImage;
    seedImage.loadImage(getVisualSystemDataPath() + "mem.gif");
    
    float width = getSharedRenderTarget().getWidth();
    float height = getSharedRenderTarget().getHeight();
    
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
        seedImage.draw((width - seedImage.getWidth()) / 2, (height - seedImage.getHeight()) / 2);
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
    //bRestart = true;
	restart();
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
    fgColor1.setHsb(fgParams1[0], fgParams1[1], fgParams1[2]);
    fgColor2.setHsb(fgParams2[0], fgParams2[1], fgParams2[2]);
    
    if (bRestart || outFbo.getWidth() != ofGetWidth() || outFbo.getHeight() != ofGetHeight()) {
        restart();
        bRestart = false;
    }
    
    ofPushStyle();
    ofEnableAlphaBlending();
    {
        texFbo.begin();
        {
//            ofClear(255, 255);

            ofSetColor(255);
            outFbo.draw(0, 0);
            
            ofSetColor(255);
            ofCircle(GetCloudsInputX(), GetCloudsInputY(), radius);
        }
        texFbo.end();

        outFbo.begin();
        conwayShader.begin();
        conwayShader.setUniformTexture("tex", texFbo.getTextureReference(), 1);
        conwayShader.setUniform1f("fade", fade);
        {
//            ofClear(0, 255);

            ofSetColor(255);
            mesh.draw();
        }
        conwayShader.end();
        outFbo.end();
    }
    ofPopStyle();
}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::selfDraw()
{
    if (!bIs2D) {
        ofPushMatrix();
        ofScale(1, -1, 1);
        ofTranslate(-ofGetWidth() / 2, -ofGetHeight() / 2);
        render();
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
        render();
    }
}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::render()
{
    ofPushStyle();
    ofEnableAlphaBlending();
    blenderShader.begin();
    blenderShader.setUniformTexture("tex", outFbo.getTextureReference(), 1);
    blenderShader.setUniform2f("dims", outFbo.getWidth(), outFbo.getHeight());
    blenderShader.setUniform4f("frontColor1", fgColor1.r, fgColor1.g, fgColor1.b, fgColor1.a);
    blenderShader.setUniform4f("frontColor2", fgColor2.r, fgColor2.g, fgColor2.b, fgColor2.a);
    {
        ofSetColor(255);
        mesh.draw();
    }
    blenderShader.end();
    ofPopStyle();
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
	if(args.key == 'R'){
		bRestart = true;
	}
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