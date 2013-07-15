#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"

// LIA includes
#include "Globals.h"
#include "ColorPalette.h"
#include "Module.h"



class CloudsVisualSystemLia : public CloudsVisualSystem {
public:
    
	string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    void selfPresetLoaded(string presetPath);

    
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
    
    
     // ---------------------------------------------- LIA public stuff
    ofImage paletteImage;
    int num;
    vector<Module> modules;
    
	
protected:
	float numCircles;
	float maxRadius;
    
    float moduleArcRad;
    float moverSpeed;
    float moduleSizeOfTinyCross;
    
    float moduleArcAlpha;
    
    float moduleLightsStrokeWidth;
    float moduleLightsStrokeAlpha;
    
    float moduleArcLenBase;
    float moduleArcLenRandom;
    float moduleArcRotationRandom;
    float moduleFakeMouseX;
    float moduleKeepTheCircleSize;
    
    float relativeFakeMouseX; // 0..1
    
  
    
    
    
};