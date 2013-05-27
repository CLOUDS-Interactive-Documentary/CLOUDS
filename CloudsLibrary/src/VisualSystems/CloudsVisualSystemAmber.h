#pragma once

#include "CloudsVisualSystemRezanator.h"

#define NUM_BILLBOARDS 1000

class CloudsVisualSystemAmber : public CloudsVisualSystemRezanator
{
public:
	string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    void selfDraw();
    void selfExit();
    void selfBegin();
	void selfEnd();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
    void setupSphereGui(ofxUISuperCanvas *g, ofxSphericalAttractionBehavior *s, string _name, string _settings);
    void guiSphereEvent(ofxUIEventArgs &e);
    void setupHomingGui(ofxUISuperCanvas *g);
    void guiHomingEvent(ofxUIEventArgs &e);
    void setupPerlinGui(ofxUISuperCanvas *g);
    void guiPerlinEvent(ofxUIEventArgs &e);
    void setupDamperGui(ofxUISuperCanvas *g);
    void guiDamperEvent(ofxUIEventArgs &e);
    void setupDistortGui(ofxUISuperCanvas *g);
    void guiDistortEvent(ofxUIEventArgs &e);
    void setupElectroGui(ofxUISuperCanvas *g);
    void guiElectroEvent(ofxUIEventArgs &e);
    void setupBufferGui(ofxUISuperCanvas *g);
    void guiBufferEvent(ofxUIEventArgs &e); 
    
protected:
    
    float particleAlpha;
    ofxUISuperCanvas *sphereGui;
    ofxUISuperCanvas *sphereGui1;
    ofxUISuperCanvas *homingGui;
    ofxUISuperCanvas *perlinGui;
    ofxUISuperCanvas *damperGui;
    ofxUISuperCanvas *distortGui;
    ofxUISuperCanvas *bufferGui;
    ofxUISuperCanvas *electroGui;
    
    ofVec3f billboardVels[NUM_BILLBOARDS];
    float billboardSizeTarget[NUM_BILLBOARDS];
    ofVboMesh billboards;
    ofShader *billboardShader;
    ofImage *glow;
    
    float *bufferData;
    float noiseScaleX;
    float noiseScaleY;
    int bufferSize;
    
    ofxRParticleRenderer *renderer;
    ofxRParticleGlowieRenderer *glowRenderer;
    ofxRParticleSystem *ps;
    ofxHomingBehavior *homing;
    ofxPerlinBehavior *perlin;
    ofxDamperBehavior *damper;
    ofxDistorterBehavior *distort;
    ofxSphericalAttractionBehavior *sphere;
    ofxSphericalAttractionBehavior *sphere1;
    ofxElectroStaticBehavior *electro;
    ofxBufferEffectorBehavior *buffer;
};