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
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5DrawingMachine10::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfGuiEvent(ofxUIEventArgs &e)
{
    
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
    string shadersFolder = "shaders";
    updateShader.load("", shadersFolder + "/update.frag");
    renderShader.load(shadersFolder + "/render.vert", shadersFolder + "/render.frag");
    
    // Load defaults.
    numParticles = 10000;
    numAttractors = 200;
    
    restart();
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::restart()
{
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
    updatePingPong.allocate(textureRes, textureRes,GL_RGB32F);
    updatePingPong.src->getTextureReference().loadData(posData, textureRes, textureRes, GL_RGB);
    updatePingPong.dst->getTextureReference().loadData(posData, textureRes, textureRes, GL_RGB);
    
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
        updateShader.setUniform2fv("attractors", attData, numAttractors);
        updateShader.setUniform1f("factor", 0.05f);
        updateShader.setUniform1f("maxDist", 0.2f);  // 0-1
    }
    updateShader.end();
    
    // Clean up.
    delete [] attData;
    
    // Allocate the output FBO.
    renderFBO.allocate(width, height, GL_RGBA32F);
    renderFBO.begin();
    {
        ofClear(0, 0);
    }
    renderFBO.end();
    
    // Build a mesh of points to use as content for the system.
    mesh.setMode(OF_PRIMITIVE_POINTS);
    for(int x = 0; x < textureRes; x++){
        for(int y = 0; y < textureRes; y++){
            mesh.addVertex(ofVec3f(x,y));
            mesh.addTexCoord(ofVec2f(x, y));
        }
    }
    
    timeStep = ofGetElapsedTimeMillis();
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5DrawingMachine10::selfPresetLoaded(string presetPath){

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
    timeStep = ofGetElapsedTimeMillis() - timeStep;
    
    // Calculate the new position affected by the attractors.
    updatePingPong.dst->begin();
    {
        ofClear(0);
        updateShader.begin();
        {
            updateShader.setUniformTexture("posData", updatePingPong.src->getTextureReference(), 0); // Previus position
            updateShader.setUniform1f("timestep",(float) timeStep );
            
            // Draw the source position texture to be updated.
            updatePingPong.src->draw(0, 0);
        }
        updateShader.end();
    }
    updatePingPong.dst->end();
    updatePingPong.swap();
    
    // Convert the position texture to points in space and render.
    renderFBO.begin();
    {
        ofSetColor(255, 200);
        //        ofClear(0, 0, 0, 0);
        renderShader.begin();
        {
            renderShader.setUniformTexture("posTex", updatePingPong.dst->getTextureReference(), 0);
            renderShader.setUniform2f("screen", (float)width, (float)height);
            
            mesh.draw();
        }
        renderShader.end();
    }
    renderFBO.end();
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
    ofSetColor(255);
    renderFBO.draw(0,0);
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5DrawingMachine10::selfEnd(){

	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfExit()
{

}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5DrawingMachine10::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5DrawingMachine10::selfKeyReleased(ofKeyEventArgs & args){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfMouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemOpenP5DrawingMachine10::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5DrawingMachine10::selfMousePressed(ofMouseEventArgs& data)
{
    restart();
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5DrawingMachine10::selfMouseReleased(ofMouseEventArgs& data)
{

}