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
    customGui->addSlider("2D SCALE", 1, 18, &scale2D);
    customGui->addToggle("USE INPUT", &bDoInput);
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
    
    customGui->addSpacer();
    customGui->addLabel("SEED IMAGE");
    vector<string> seedNames;
    for (int i = 0; i < seedDir.size(); i++) {
        seedNames.push_back(seedDir.getName(i));
    }
    customGui->addRadio("SEEDS", seedNames);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemAutomata::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::selfGuiEvent(ofxUIEventArgs &e)
{
    // Let's look through the files dropdown for a match.
    string name = e.widget->getName();
    for (int i = 0; i < seedDir.numFiles(); i++) {
        if (name == seedDir.getName(i) && ((ofxUIToggle *)e.widget)->getValue()) {
            selectedSeedIdx = i;
            bRestart = true;
            break;
        }
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
    conwayShader.load("", getVisualSystemDataPath() + "shaders/conway.frag");
    blenderShader.load("", getVisualSystemDataPath() + "shaders/blender.frag");
    bIs2D = true;
	
    // Set defaults.
    radius = 5.0f;
    bDoInput = true;

    seedDir.listDir(getVisualSystemDataPath() + "seedImages");
    seedDir.sort();
    selectedSeedIdx = -1;
}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::restart()
{
    ofImage seedImage;
    if (selectedSeedIdx > -1) {
        seedImage.loadImage(seedDir.getPath(selectedSeedIdx));
    }
    
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
        if (seedImage.isAllocated()) {
            seedImage.draw((width - seedImage.getWidth()) / 2, (height - seedImage.getHeight()) / 2);
        }
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
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    ofPushStyle();
    
    glDisable(GL_LIGHTING);
    
    fgColor1.setHsb(fgParams1[0], fgParams1[1], fgParams1[2]);
    fgColor2.setHsb(fgParams2[0], fgParams2[1], fgParams2[2]);
    
    if (bRestart || outFbo.getWidth() != getSharedRenderTarget().getWidth() || outFbo.getHeight() != getSharedRenderTarget().getHeight()) {
        restart();
        bRestart = false;
    }
    
    ofEnableAlphaBlending();
    {
        texFbo.begin();
        {
//            ofClear(255, 255);

            ofSetColor(255);
            outFbo.draw(0, 0);
            
            if (bDoInput) {
                // Map the coords to the 2D scale.
                float inputX = ofMap(GetCloudsInputX(), 0, getSharedRenderTarget().getWidth(),
                                     getSharedRenderTarget().getWidth() * (0.5f - 0.5f / scale2D), getSharedRenderTarget().getWidth() * (0.5f + 0.5f / scale2D));
                float inputY = ofMap(GetCloudsInputY(), 0, getSharedRenderTarget().getHeight(),
                                     getSharedRenderTarget().getHeight() * (0.5f - 0.5f / scale2D), getSharedRenderTarget().getHeight() * (0.5f + 0.5f / scale2D));
                ofSetColor(255);
//                ofNoFill();
                ofCircle(inputX, inputY, radius);
            }
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
    glPopAttrib();
}

//--------------------------------------------------------------
void CloudsVisualSystemAutomata::selfDraw()
{
    if (!bIs2D) {
        ofPushMatrix();
        ofScale(1, -1, 1);
        //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
        ofTranslate(-getCanvasWidth() / 2, -getCanvasHeight() / 2);
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
        ofPushMatrix();
        //MA: changed ofGetWidth() to GetCanvasWidth() and ofGetHeight() to GetCanvasHeight()
        ofTranslate(getCanvasWidth() / 2, getCanvasHeight() / 2);
        ofScale(scale2D, scale2D, 1);
        ofTranslate(-getCanvasWidth() / 2, -getCanvasHeight() / 2);
        render();
        ofPopMatrix();
    }
    
    // Debug panels.
//    ofSetColor(128);
//    ofRect(0, 0, texFbo.getWidth() * 0.5, texFbo.getHeight() * 0.5);
//    ofRect(texFbo.getWidth() * 0.5, 0, outFbo.getWidth() * 0.5, outFbo.getHeight() * 0.5);
//    
//    ofSetColor(255);
//    texFbo.draw(0, 0, texFbo.getWidth() * 0.5, texFbo.getHeight() * 0.5);
//    outFbo.draw(texFbo.getWidth() * 0.5, 0, outFbo.getWidth() * 0.5, outFbo.getHeight() * 0.5);
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

}

void CloudsVisualSystemAutomata::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemAutomata::selfInteractionEnded(CloudsInteractionEventArgs& args){
	
}

void CloudsVisualSystemAutomata::selfInteractionMoved(CloudsInteractionEventArgs& args){
	currentInput = ofVec2f(args.position.x, args.position.y);
}

void CloudsVisualSystemAutomata::selfInteractionStarted(CloudsInteractionEventArgs& args){
	
}

void CloudsVisualSystemAutomata::selfInteractionDragged(CloudsInteractionEventArgs& args){
	
}