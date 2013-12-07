//
//  CloudsVisualSystemGPUParticles.cpp
//

#include "CloudsVisualSystemGPUParticles.h"

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemGPUParticles::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
    customGui->addButton("REGENERATE", &shouldRegenerate);
    
	customGui->addIntSlider("Particle Count", 1, 2000, &numParticles);
    customGui->addSlider("Particle Size", 1.0, 300.0, &particleSize);
    customGui->addSlider("Time Step", 0.0001, .05, &timeStep);
    
    customGui->addMinimalSlider("Hue1", 0.0, 255.0, &hue1);
    customGui->addMinimalSlider("Saturation1", 0.0, 200.0, &saturation1);
    customGui->addMinimalSlider("Brightness1", 0.0, 255.0, &brightness1);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemGPUParticles::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
    
    ///////////////////
    
    particleSize = 30.0f;
    timeStep = 0.005f;
    numParticles = 50;
    
    // Width and Heigth of the windows
    width = ofGetWindowWidth();
    height = ofGetWindowHeight();
    
    // Loading the Shaders
    updatePos.load("", getVisualSystemDataPath() + "shaders/posUpdate.frag");// shader for updating the texture that store the particles position on RG channels
    updateVel.load("", getVisualSystemDataPath() + "shaders/velUpdate.frag");// shader for updating the texture that store the particles velocity on RG channels
    
    // Frag, Vert and Geo shaders for the rendering process of the spark image
    updateRender.setGeometryInputType(GL_POINTS);
	updateRender.setGeometryOutputType(GL_TRIANGLE_STRIP);
	updateRender.setGeometryOutputCount(6);
    updateRender.load(getVisualSystemDataPath() + "shaders/render.vert",
                      getVisualSystemDataPath() + "shaders/render.frag",
                      getVisualSystemDataPath() + "shaders/render.geom");
    
    // Seting the textures where the information ( position and velocity ) will be
    textureRes = (int)sqrt((float)numParticles);
    numParticles = textureRes * textureRes;
    
    // 1. Making arrays of float pixels with position information
    float * pos = new float[numParticles*3];
    for (int x = 0; x < textureRes; x++){
        for (int y = 0; y < textureRes; y++){
            int i = textureRes * y + x;
            
            pos[i*3 + 0] = ofRandom(1.0); //x*offset;
            pos[i*3 + 1] = ofRandom(1.0); //y*offset;
            pos[i*3 + 2] = 0.0;
        }
    }
    // Load this information in to the FBO«s texture
    posPingPong.allocate(textureRes, textureRes, GL_RGB32F);
    posPingPong.src->getTextureReference().loadData(pos, textureRes, textureRes, GL_RGB);
    posPingPong.dst->getTextureReference().loadData(pos, textureRes, textureRes, GL_RGB);
    delete [] pos;    // Delete the array
    
    
    // 2. Making arrays of float pixels with velocity information and the load it to a texture
    float * vel = new float[numParticles*3];
    for (int i = 0; i < numParticles; i++){
        vel[i*3 + 0] = ofRandom(-1.0,1.0);
        vel[i*3 + 1] = ofRandom(-1.0,1.0);
        vel[i*3 + 2] = 1.0;
    }
    // Load this information in to the FBO«s texture
    velPingPong.allocate(textureRes, textureRes,GL_RGB32F);
    velPingPong.src->getTextureReference().loadData(vel, textureRes, textureRes, GL_RGB);
    velPingPong.dst->getTextureReference().loadData(vel, textureRes, textureRes, GL_RGB);
    delete [] vel; // Delete the array
    
    // Loading and setings of the variables of the textures of the particles
    sparkImg.loadImage(getVisualSystemDataPath() + "glow2.png");
    imgWidth = sparkImg.getWidth();
    imgHeight = sparkImg.getHeight();
    
    // Allocate the final
    renderFBO.allocate(width, height, GL_RGB32F);
    renderFBO.begin();
    ofClear(0, 0, 0, 255);
    renderFBO.end();
    
    
    
    
}

void CloudsVisualSystemGPUParticles::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
    
    if(e.widget->getName() == "REGENERATE" && ((ofxUIButton*)e.widget)->getValue() ){
		regenerate();
	}
    
}

void CloudsVisualSystemGPUParticles::regenerate(){
    
    ofClear(0);
    textureRes = (int)sqrt((float)numParticles);
    numParticles = textureRes * textureRes;
    
    // Seting the textures where the information ( position and velocity ) will be
    textureRes = (int)sqrt((float)numParticles);
    numParticles = textureRes * textureRes;
    
    float * pos = new float[numParticles*3];
    for (int x = 0; x < textureRes; x++){
        for (int y = 0; y < textureRes; y++){
            int i = textureRes * y + x;
            
            pos[i*3 + 0] = ofRandom(1.0); //x*offset;
            pos[i*3 + 1] = ofRandom(1.0); //y*offset;
            pos[i*3 + 2] = 0.0;
        }
    }
    
  
    posPingPong.allocate(textureRes, textureRes,GL_RGB32F);
    posPingPong.src->getTextureReference().loadData(pos, textureRes, textureRes, GL_RGB);
    posPingPong.dst->getTextureReference().loadData(pos, textureRes, textureRes, GL_RGB);
    delete [] pos;    // Delete the array
    
    
    // 2. Making arrays of float pixels with velocity information and the load it to a texture
    float * vel = new float[numParticles*3];
    for (int i = 0; i < numParticles; i++){
        vel[i*3 + 0] = ofRandom(-1.0,1.0);
        vel[i*3 + 1] = ofRandom(-1.0,1.0);
        vel[i*3 + 2] = 1.0;
    }
    // Load this information in to the FBO«s texture
    velPingPong.allocate(textureRes, textureRes,GL_RGB32F);
    velPingPong.src->getTextureReference().loadData(vel, textureRes, textureRes, GL_RGB);
    velPingPong.dst->getTextureReference().loadData(vel, textureRes, textureRes, GL_RGB);
    delete [] vel; // Delete the array
    
    // Allocate the final
    renderFBO.allocate(width, height, GL_RGB32F);
    renderFBO.begin();
    ofClear(0, 0, 0, 255);
    renderFBO.end();
     
     
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemGPUParticles::selfSetupSystemGui(){
	
}

void CloudsVisualSystemGPUParticles::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemGPUParticles::selfSetupRenderGui(){

}

void CloudsVisualSystemGPUParticles::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemGPUParticles::selfSetup(){

	//this is how to load assets
//	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemGPUParticles::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemGPUParticles::selfBegin(){
	// START STUFF
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemGPUParticles::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemGPUParticles::selfUpdate(){
	// CHANGE STUFF
    
    velPingPong.dst->begin();
    ofClear(0);
    updateVel.begin();
    updateVel.setUniformTexture("backbuffer", velPingPong.src->getTextureReference(), 0);   // passing the previus velocity information
    updateVel.setUniformTexture("posData", posPingPong.src->getTextureReference(), 1);  // passing the position information
    updateVel.setUniform1i("resolution", (int)textureRes);
    updateVel.setUniform2f("screen", (float)width, (float)height);
    updateVel.setUniform1f("timestep", (float)timeStep);
    
    // draw the source velocity texture to be updated
    velPingPong.src->draw(0, 0);
    
    updateVel.end();
    velPingPong.dst->end();
    
    velPingPong.swap();
    
    
    // Positions PingPong
    //
    // With the velocity calculated updates the position
    //
    posPingPong.dst->begin();
    ofClear(0);
    updatePos.begin();
    updatePos.setUniformTexture("prevPosData", posPingPong.src->getTextureReference(), 0); // Previus position
    updatePos.setUniformTexture("velData", velPingPong.src->getTextureReference(), 1);  // Velocity
    updatePos.setUniform1f("timestep",(float) timeStep );
    
    // draw the source position texture to be updated
    posPingPong.src->draw(0, 0);
    
    updatePos.end();
    posPingPong.dst->end();
    
    posPingPong.swap();
    
    
    // Rendering
    //
    // 1.   Sending this vertex to the Vertex Shader.
    //      Each one it«s draw exactly on the position that match where it«s stored on both vel and pos textures
    //      So on the Vertex Shader (that«s is first at the pipeline) can search for it information and move it
    //      to it right position.
    // 2.   Once it«s in the right place the Geometry Shader make 6 more vertex in order to make a billboard
    // 3.   that then on the Fragment Shader it«s going to be filled with the pixels of sparkImg texture
    //
    renderFBO.begin();
    ofClear(0,0,0,0);
    updateRender.begin();
    updateRender.setUniformTexture("posTex", posPingPong.dst->getTextureReference(), 0);
    updateRender.setUniformTexture("sparkTex", sparkImg.getTextureReference() , 1);
    updateRender.setUniform1i("resolution", (float)textureRes);
    updateRender.setUniform2f("screen", (float)width, (float)height);
    updateRender.setUniform1f("size", (float)particleSize);
    updateRender.setUniform1f("imgWidth", (float)sparkImg.getWidth());
    updateRender.setUniform1f("imgHeight", (float)sparkImg.getHeight());
    
    ofPushStyle();
    ofEnableBlendMode( OF_BLENDMODE_ADD);
    ofSetColor(255);
    
    glBegin( GL_POINTS );
    for(int x = 0; x < textureRes; x++){
        for(int y = 0; y < textureRes; y++){
            glVertex2d(x,y);
            glTexCoord2i(x, y);
        }
    }
    
    ofDisableBlendMode();
    glEnd();
    
    updateRender.end();
    renderFBO.end();
    ofPopStyle();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemGPUParticles::selfDraw(){
    
    
}

// draw any debug stuff here
void CloudsVisualSystemGPUParticles::selfDrawDebug(){
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemGPUParticles::selfDrawBackground(){
		// DRAW STUFF
	//use this to Draw 2D stuff
    ofPushStyle();
    ofEnableAlphaBlending();
    
    newColor.setHsb(hue1,saturation1, brightness1);
    ofSetColor(newColor); //(100,255,255);
    renderFBO.draw(0,0);
    
    //ofSetColor(255);
    //ofDrawBitmapString("Fps: " + ofToString( ofGetFrameRate()), 15,15);
	//turn the background refresh off
	//bClearBackground = false;
    ofPopStyle();
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemGPUParticles::selfEnd(){

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemGPUParticles::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemGPUParticles::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemGPUParticles::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemGPUParticles::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemGPUParticles::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemGPUParticles::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemGPUParticles::selfMouseReleased(ofMouseEventArgs& data){
	
}