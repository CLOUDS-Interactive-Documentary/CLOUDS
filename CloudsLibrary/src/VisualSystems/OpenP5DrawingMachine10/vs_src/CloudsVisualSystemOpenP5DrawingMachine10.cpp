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
	customGui = new ofxUISuperCanvas("DRAWING MACHINE 10", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Drawing_Machine_10");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addSpacer();
    customGui->addButton("REDRAW", false);
    
    customGui->addSpacer();
    customGui->addIntSlider("NUM PARTICLES", 1, 100000, &numParticles);
    customGui->addIntSlider("NUM ATTRACTORS", 1, 500, &numAttractors);
    customGui->addSlider("SPEED FACTOR", 0, 500, &speedFactor);
    customGui->addSlider("MAX DIST", 0, 1, &maxDist);
    
    customGui->addSpacer();
    customGui->addSlider("DRAW TIME", 1, 60, &drawLength);
    customGui->addSlider("FADE TIME", 0, 60, &fadeLength);
    customGui->addSlider("WAIT TIME", 0, 60, &waitLength);
    
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
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5DrawingMachine10::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "REDRAW") {
        if (((ofxUIButton *)e.widget)->getValue() == true) {
            restart();
        }
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
    // Save the width and height of the window.
    width = ofGetWindowWidth();
    height = ofGetWindowHeight();
    
    // Load the shaders.
    string shadersFolder = getVisualSystemDataPath() + "shaders/";
    updateShader.load("", shadersFolder + "update.frag");
    renderShader.load(shadersFolder + "render.vert", shadersFolder + "render.frag");
    
    // Load defaults.
    numParticles = 10000;
    numAttractors = 200;
    speedFactor = 1.0f;
    maxDist = 0.2f;
    
    drawLength = 5.0f;
    fadeLength = 1.0f;
    waitLength = 2.0f;
    state = DM10STATE_DRAWING;
    
    restart();
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::restart()
{
//    cout << "Restarting with " << numParticles << " particles and " << numAttractors << " attractors" << endl;
	
    // Make an array of float pixels with position data.
    textureRes = (int)sqrt((float)numParticles);
    numParticles = textureRes * textureRes;
    float * posData = new float[numParticles * 3];
    for (int y = 0; y < textureRes; y++) {
        for (int x = 0; x < textureRes; x++) {
            int idx = textureRes * y + x;
            
            posData[idx * 3 + 0] = ofRandomuf();  // x;
            posData[idx * 3 + 1] = ofRandomuf();  // y;
            posData[idx * 3 + 2] = 0.0;
        }
    }
    
    // Load the data to a texture.
    updateBuffer.allocate(textureRes, textureRes, GL_RGB32F);
    updateBuffer.src->getTextureReference().loadData(posData, textureRes, textureRes, GL_RGB);
    updateBuffer.dst->getTextureReference().loadData(posData, textureRes, textureRes, GL_RGB);
    
    // Clean up.
    delete [] posData;
    
    // Make an array of float pixels with attractor data.
    float * attData = new float[numAttractors * 2];
    for (int i = 0; i < numAttractors; i++) {
        attData[i * 2 + 0] = ofRandomuf();  // x
        attData[i * 2 + 1] = ofRandomuf();  // y
    }
    
    // Load the data in the shader right away.
    updateShader.begin();
    {
        updateShader.setUniform1i("numAttractors", numAttractors);
        updateShader.setUniform2fv("attractors", attData, numAttractors);
    }
    updateShader.end();
    
    // Clean up.
    delete [] attData;
    
    // Allocate the output FBO.
    renderBuffer.allocate(width, height, GL_RGBA32F);
    renderBuffer.begin();
    {
        ofClear(0, 0);
    }
    renderBuffer.end();
    
    // Build a mesh of points to use as content for the system.
    mesh.setMode(OF_PRIMITIVE_POINTS);
    for(int x = 0; x < textureRes; x++){
        for(int y = 0; y < textureRes; y++){
            mesh.addVertex(ofVec3f(x,y));
            mesh.addTexCoord(ofVec2f(x, y));
        }
    }
    
    timeStepMs = ofGetElapsedTimeMillis();
    drawStartMs = timeStepMs;
    state = DM10STATE_DRAWING;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfPresetLoaded(string presetPath)
{
    restart();
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
    ofEnableAlphaBlending();
    elapsedTimeMs = ofGetElapsedTimeMillis();
    
    timeStepMs = elapsedTimeMs - timeStepMs;
    
    if (state == DM10STATE_DRAWING) {
        float drawTime = (elapsedTimeMs - drawStartMs) / 1000.0f;
        if (drawTime >= drawLength) {
            state = DM10STATE_FADING;
            fadeStartMs = elapsedTimeMs;
        }
    }
    
    // Calculate the new position affected by the attractors.
    updateBuffer.dst->begin();
    {
        ofClear(0);
        updateShader.begin();
        {
            updateShader.setUniformTexture("posData", updateBuffer.src->getTextureReference(), 0); // Previus position
            updateShader.setUniform1f("timestep", timeStepMs / 1000.0f);
            updateShader.setUniform1f("factor", speedFactor);
            updateShader.setUniform1f("maxDist", maxDist);

            // Draw the source position texture to be updated.
            updateBuffer.src->draw(0, 0);
        }
        updateShader.end();
    }
    updateBuffer.dst->end();
    updateBuffer.swap();
    
    // Convert the position texture to points in space and render.
    renderBuffer.begin();
    {
//        ofClear(0, 0);
        ofSetColor(255, fgAlpha->getPos());
        renderShader.begin();
        {
            renderShader.setUniformTexture("posTex", updateBuffer.dst->getTextureReference(), 0);
            renderShader.setUniform2f("screen", (float)width, (float)height);
            
            mesh.draw();
        }
        renderShader.end();
    }
    renderBuffer.end();
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
    if (state == DM10STATE_WAITING) {
        float waitTime = (elapsedTimeMs - waitStartMs) / 1000.0f;
        if (waitTime >= waitLength) {
            restart();
        }
    }
    else {
        float fadeAlpha = 255;
        if (state == DM10STATE_FADING) {
            float fadeTime = (elapsedTimeMs - fadeStartMs) / 1000.0f;
            if (fadeTime < fadeLength) {
                fadeAlpha = ofMap(fadeTime, 0, fadeLength, 255, 0);
            }
            else {
                fadeAlpha = 0;
                state = DM10STATE_WAITING;
                waitStartMs = elapsedTimeMs;
            }
        }
        
        ofSetColor(ofColor::fromHsb(fgHue->getPos(), fgSat->getPos(), fgBri->getPos(), fadeAlpha));
        renderBuffer.draw(0, 0, ofGetWidth(), ofGetHeight());
    }
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5DrawingMachine10::selfEnd(){

	
}

void CloudsVisualSystemOpenP5DrawingMachine10::selfExit(){

}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5DrawingMachine10::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5DrawingMachine10::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5DrawingMachine10::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystemOpenP5DrawingMachine10::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5DrawingMachine10::selfMousePressed(ofMouseEventArgs& data){
    
}

void CloudsVisualSystemOpenP5DrawingMachine10::selfMouseReleased(ofMouseEventArgs& data){

}
