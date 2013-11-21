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
    customGui->addToggle("RESTART", &bRestart);
    customGui->addSlider("SCALAR", 0, 1, scalar * 100.0f);
    customGui->addSlider("SPEED", 0, 10, &speed);

    customGui->addSpacer();
    customGui->addToggle("2D", &bIs2D);
    customGui->addSlider("GRANULARITY", 0, 1, &granularity);
    customGui->addSlider("MAX HEIGHT", 0, 100, &maxHeight);
    
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
    
    else if (e.widget->getName() == "2D" ||
             e.widget->getName() == "GRANULARITY") {
        bRestart = true;
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
    displaceShader.load(getVisualSystemDataPath() + "shaders/displace");
	
    // Set defaults.
    scalar = 0.005f;
    speed = 1.0f;
    granularity = 0;
    maxHeight = 50;
    bRestart = true;
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
        srcFbo.allocate(width, height);
    }
	
    srcFbo.begin();
    {
        ofClear(0, 0);
    }
    srcFbo.end();
    
    if (!dstFbo.isAllocated() || dstFbo.getWidth() != width || dstFbo.getHeight() != height) {
        dstFbo.allocate(width, height);
    }
    dstFbo.begin();
    {
        ofClear(0, 0);
    }
    dstFbo.end();
    
    // Build a render mesh for drawing onto.
    mesh.clear();

    if (bIs2D) {
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        
        mesh.addVertex(ofVec3f(0, 0));
        mesh.addTexCoord(ofVec2f(0, 0));
        
        mesh.addVertex(ofVec3f(width, 0));
        mesh.addTexCoord(ofVec2f(width, 0));
        
        mesh.addVertex(ofVec3f(width, height));
        mesh.addTexCoord(ofVec2f(width, height));
        
        mesh.addVertex(ofVec3f(0, height));
        mesh.addTexCoord(ofVec2f(0, height));
    }
    else {
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        int numCols = ofMap(granularity, 0, 1, 2, width, true);
        int numRows = ofMap(granularity, 0, 1, 2, height, true);
        float sliceWidth = width / numCols;
        float sliceHeight = height / numRows;

        ofVec3f vert;
        ofVec3f normal(0, 0, 1);
        ofVec2f texCoord;
        
        for (int j = 0; j < numRows; j++) {
            for (int i = 0; i < numCols; i++) {
                texCoord.x = i * sliceWidth;
                texCoord.y = j * sliceHeight;
                
                vert.x = texCoord.x;
                vert.y = texCoord.y;
                
                mesh.addVertex(vert);
                mesh.addTexCoord(texCoord);
                mesh.addNormal(normal);
            }
        }

        for (int y = 0; y < numRows - 1; y++) {
            if ((y&1) == 0) {  // even
                for (int x = 0; x < numCols; x++) {
                    mesh.addIndex(y * numCols + x);
                    mesh.addIndex((y + 1) * numCols + x);
                }
            }
            else {  // odd
                for (int x = numCols - 1; x > 0; x--) {
                    mesh.addIndex((y + 1) * numCols + x);
                    mesh.addIndex(y * numCols + x - 1);
                }
            }
        }
        
        if (numRows % 2 != 0) {
            mesh.addIndex(mesh.getNumVertices() - numCols);   
        }
    }
    
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
    fgColor.setHsb(fgHue->getPos(), fgSat->getPos(), fgBri->getPos(), fgAlpha->getPos());
    
    if (bRestart || srcFbo.getWidth() != ofGetWidth() || srcFbo.getHeight() != ofGetHeight()) {
        restart();
        bRestart = false;
    }
    
    // Move towards the mouse.
    ofVec2f toMouse = ofVec2f(ofGetMouseX(), ofGetMouseY()) - attractor;
    if (toMouse.length() < speed) {
        attractor += toMouse;
    }
    else {
        attractor += toMouse.scale(speed);
    }
    
    dstFbo.begin();
    shader.begin();
    shader.setUniformTexture("baseTex", srcFbo.getTextureReference(), 1);
    shader.setUniform4f("fgColor", fgColor.r / 255.f, fgColor.g / 255.f, fgColor.b / 255.f, fgColor.a / 255.f);
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
    if (bIs2D) return;
    
    ofPushMatrix();
    ofScale(1, -1, 1);
    ofTranslate(dstFbo.getWidth() * -0.25f, dstFbo.getHeight() * -0.25f);
    {
        displaceShader.begin();
        displaceShader.setUniformTexture("tex", dstFbo.getTextureReference(), 1);
        displaceShader.setUniform1f("maxHeight", maxHeight);
        {
            ofSetColor(255);
            mesh.draw();
        }
        displaceShader.end();
    }
    ofPopMatrix();
    
    swap(srcFbo, dstFbo);
}

// draw any debug stuff here
void CloudsVisualSystemOpenP5Fifteen::selfDrawDebug(){

}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5Fifteen::selfDrawBackground()
{
    if (!bIs2D) return;
    
    ofPushStyle();
    ofEnableAlphaBlending();
    {
        ofSetColor(255);
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

