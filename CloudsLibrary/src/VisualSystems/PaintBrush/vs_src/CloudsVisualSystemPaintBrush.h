//
//  CloudsVisualSystemPaintBrush.h
//  VSPaintBrush
//
//  Created by Patricio Gonzalez Vivo on 6/13/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"

#include "Brush.h"

class CloudsVisualSystemPaintBrush : public CloudsVisualSystem {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfAutoMode();
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    
    void selfExit();
    void selfBegin();
	void selfEnd();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void mouseDragged(ofMouseEventArgs & args);
    void mouseMoved(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
protected:
    ofFbo               canvas;
    Brush               brush;
    vector<Particle>    particles;
    
    float   brushWidth;
    float   brushNumber;
    float   brushDamp;
    float   brushK;
    float   brushRepRad;
    float   brushRepPct;
    
    float   lineWidth;
    
    float   particlesThreshold;
    float   particlesTurbulence;
    float   particlesAlpha;

    float   colorHue;
    float   colorLerp;
    float   colorRandom;
    
    bool                    bUseColorMap;
    ofPixels *              colorMap;
    vector<string>          colorMapNames;
	map<string, ofPixels *> colorMapPixelsMap;
    int                     mapX, mapY;
    bool                    bMapForward;

};