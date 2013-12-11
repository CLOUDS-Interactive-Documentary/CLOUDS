//
//  CloudsVisualSystemXstatic.cpp
//

#include "CloudsVisualSystemXstatic.h"
#include "CloudsRGBDVideoPlayer.h"
#include "ofxBillboard.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif


static const float kBoxSize      = 300;
static const int   kMaxParticles = 2000;
static const int   kStrideData   = 8;

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemXstatic::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addSpacer();
    customGui->addToggle("DRAW BOX", &bDrawBox);
    customGui->addToggle("REGENERATE", &bShouldRegenerate);
    customGui->addIntSlider("NUM PARTICLES", 0, 2000, &nParticles);
    customGui->addToggle("BOUNCE OFF WALLS", &bBounceOffWalls);

    customGui->addSpacer();
    customGui->addToggle("FREEZE", &bShouldFreeze);
    customGui->addToggle("RISE", &bShouldRise);
    customGui->addToggle("FALL", &bShouldFall);
    customGui->addMinimalSlider("RISE/FALL SPEED", 0, 200, &riseFallSpeed);
    
    customGui->addSpacer();
    customGui->addToggle("EXPLODE", &bShouldExplode);
    customGui->addMinimalSlider("EXPLODE SPEED", 0, 200, &explodeSpeed);
    
    customGui->addSpacer();
    customGui->addLabel("GRAVITY");
    customGui->addMinimalSlider("GRAVITY X", -1, 1, &gravity.x);
    customGui->addMinimalSlider("GRAVITY Y", -1, 1, &gravity.y);
    customGui->addMinimalSlider("GRAVITY Z", -1, 1, &gravity.z);
    
    customGui->addSpacer();
    customGui->addSlider("ROTATE ANGLE", 45, 135, &rotateAngle);
    customGui->addSlider("ROTATE SPEED", 0, 10, &rotateSpeed);
    customGui->addSlider("PULL SPEED", 0, 10, &pullSpeed);
    
    customGui->addSpacer();
    customGui->addSlider("WIND SPEED", 0, 10, &windSpeed);
    
    customGui->addSpacer();
    customGui->addSlider("DRAG", 0, 1, &drag);
    
    customGui->addSpacer();
    customGui->addRangeSlider("SIZE", 0, 20, &XParticle::minSize, &XParticle::maxSize);
    customGui->addSlider("HEIGHT TO SIZE", 0, 1, &XParticle::heightToSize);
    customGui->addSlider("VEL TO SIZE", 0, 1, &XParticle::velToSize);
    
    customGui->addSpacer();
    customGui->addRangeSlider("BRIGHTNESS", 0, 1, &XParticle::minBri, &XParticle::maxBri);
    
    customGui->addSpacer();
    customGui->addSlider("OSC SPEED", 0, 1, &XParticle::oscSpeed);
    customGui->addToggle("OSCILLATE SIZE", &XParticle::bOscillateSize);
    customGui->addToggle("OSCILLATE BRI", &XParticle::bOscillateBri);
    customGui->addSlider("TRIGGER DURATION", 1, 1000, &XParticle::triggerDuration);
    customGui->addSlider("TRIGGER RANGE", 10, 1000, &XParticle::triggerRange);
    
    customGui->addSpacer();
    customGui->addMinimalSlider("HUE 1", 0, 1, &color1.r);
    customGui->addMinimalSlider("SAT 1", 0, 1, &color1.g);
    customGui->addMinimalSlider("BRI 1", 0, 1, &color1.b);
    customGui->addMinimalSlider("HUE 2", 0, 1, &color2.r);
    customGui->addMinimalSlider("SAT 2", 0, 1, &color2.g);
    customGui->addMinimalSlider("BRI 2", 0, 1, &color2.b);
    customGui->addSlider("WEIGHT", 1, 4, &colorWeight);
    
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemXstatic::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemXstatic::selfGuiEvent(ofxUIEventArgs &e)
{

}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemXstatic::selfSetupSystemGui(){
	
}

void CloudsVisualSystemXstatic::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemXstatic::selfSetupRenderGui(){

}

void CloudsVisualSystemXstatic::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemXstatic::selfSetup()
{	
    bDrawBox = false;
    
    gravity.set(0);
    drag = 0.0;
    
    XParticle::upperBounds.set(kBoxSize *  0.5, kBoxSize *  0.5, kBoxSize *  0.5);
    XParticle::lowerBounds.set(kBoxSize * -0.5, kBoxSize * -0.5, kBoxSize * -0.5);
    XParticle::diagonalSquared = sqrtf(2 * kBoxSize * kBoxSize) + (kBoxSize * kBoxSize);
    XParticle::minSize = XParticle::maxSize = 1.0f;
    XParticle::oscSpeed = 0.1f;
    XParticle::heightToSize = 0.0f;
    XParticle::velToSize = 0.0f;
    XParticle::minBri = XParticle::maxBri = 1.0f;
    XParticle::bOscillateBri = XParticle::bOscillateSize = false;
    
    color1.setHsb(1.0f, 1.0f, 1.0f);
    color2.setHsb(1.0f, 0.0f, 1.0f);
    colorWeight = 1.0f;
    
    explodeSpeed = 1.0;
    bShouldExplode = true;

    riseFallSpeed = 1.0;
    bShouldFreeze = false;
    bShouldFall = false;
    bShouldRise = false;
    
    bBounceOffWalls = true;
    
    nParticles = 500;
    data = new GLfloat[kMaxParticles * kStrideData];
    regenerate();
    bShouldRegenerate = true;
    
    ofDisableArbTex();
    tex.loadImage(getVisualSystemDataPath() + "spark.png");
    shader.load(getVisualSystemDataPath() + "shaders/particles");
    ofEnableArbTex();
}

void CloudsVisualSystemXstatic::regenerate()
{
    particles.clear();
    for (int i = 0; i < kMaxParticles; i++) {
        particles.push_back(XParticle());
    }
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemXstatic::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemXstatic::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemXstatic::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemXstatic::selfUpdate()
{
    if (bShouldRegenerate) {
        regenerate();
        bShouldRegenerate = false;
    }
    
    XParticle::dt = ofGetLastFrameTime();
    
    if (bShouldFreeze) {
        rotateSpeed = 0;
        pullSpeed = 0;
        windSpeed = 0;
        gravity = ofVec3f::zero();
    }
    
    if (bShouldRise || bShouldFall) {
        drag = 0;
        rotateSpeed = 0;
        pullSpeed = 0;
        windSpeed = 0;
        gravity = ofVec3f::zero();
    }
    
    for (int i = 0; i < nParticles; i++) {
        if (bShouldExplode) {
            ofVec3f explodeForce(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
            explodeForce.scale(explodeSpeed);
            particles[i].applyForce(explodeForce);
        }
        
        if (bShouldFreeze) {
            particles[i].velocity = ofVec3f::zero();
            particles[i].acceleration = ofVec3f::zero();
        }
        else if (bShouldRise) {
            particles[i].velocity.set(0, riseFallSpeed, 0);
            particles[i].acceleration = ofVec3f::zero();
        }
        else if (bShouldFall) {
            particles[i].velocity.set(0, -riseFallSpeed, 0);
            particles[i].acceleration = ofVec3f::zero();
        }
        
        ofVec3f rotateForce = particles[i].location.getRotated(rotateAngle, ofVec3f(0, 1, 0));//(particles[i].location.x, 0, particles[i].location.z);
        rotateForce -= particles[i].location;
        rotateForce.scale(rotateSpeed);
        particles[i].applyForce(rotateForce);
        
        ofVec3f pullForce(-particles[i].location.x, 0, -particles[i].location.z);
        pullForce.scale(pullSpeed);
        particles[i].applyForce(pullForce);
        
        ofVec3f windForce(ofSignedNoise(particles[i].location.x),
                          0,//ofSignedNoise(particles[i].location.y , particles[i].uniqueVal + particles[i].location.y),
                          ofSignedNoise(particles[i].location.z , 0, ofGetElapsedTimef()));
        windForce.scale(windSpeed);
        particles[i].applyForce(windForce);

        particles[i].applyForce(gravity);
        
        particles[i].update(drag);
        
        if (bBounceOffWalls) {
            particles[i].bounceEdges();
        }
        else {
            particles[i].wrapEdges();
        }
        
        ofFloatColor pColor;
        pColor.setHsb(ofMap(powf(particles[i].colorPicker, colorWeight), 0, 1, color1.r, color2.r),
                      ofMap(powf(particles[i].colorPicker, colorWeight), 0, 1, color1.g, color2.g),
                      ofMap(powf(particles[i].colorPicker, colorWeight), 0, 1, color1.b, color2.b));
                
        data[i * kStrideData + 0] = particles[i].location.x;
        data[i * kStrideData + 1] = particles[i].location.y;
        data[i * kStrideData + 2] = particles[i].location.z;
        data[i * kStrideData + 3] = particles[i].mappedSize();
        data[i * kStrideData + 4] = pColor.r;
        data[i * kStrideData + 5] = pColor.g;
        data[i * kStrideData + 6] = pColor.b;
        data[i * kStrideData + 7] = particles[i].brightness;
    }
    
    vbo.setVertexData(&data[0], 4, nParticles, GL_STREAM_DRAW, kStrideData * sizeof(GLfloat));
    vbo.setColorData(&data[4], nParticles, GL_STREAM_DRAW, kStrideData * sizeof(GLfloat));
    
    gravityLine.clear();
    gravityLine.setMode(OF_PRIMITIVE_LINES);
    gravityLine.addVertex(ofVec3f::zero());
    gravityLine.addVertex(gravity * 10);

    bShouldExplode = false;
    bShouldFreeze = false;
    bShouldRise = false;
    bShouldFall = false;
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemXstatic::selfDraw()
{
    ofSetColor(255);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    if (bDrawBox) {
        ofNoFill();
        ofBox(0, 0, 0, kBoxSize);
    }

    glDisable(GL_DEPTH_TEST);
    
    shader.begin();
    ofEnablePointSprites();
//    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    tex.getTextureReference().bind();
    {
        vbo.draw(GL_POINTS, 0, nParticles);
    }
    tex.getTextureReference().unbind();
    ofDisablePointSprites();
    shader.end();
}

// draw any debug stuff here
void CloudsVisualSystemXstatic::selfDrawDebug()
{
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    gravityLine.draw();
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemXstatic::selfDrawBackground(){

}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemXstatic::selfEnd(){
		
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemXstatic::selfExit()
{
    delete [] data;
    data = NULL;
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemXstatic::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemXstatic::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemXstatic::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemXstatic::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemXstatic::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemXstatic::selfMouseReleased(ofMouseEventArgs& data){
	
}