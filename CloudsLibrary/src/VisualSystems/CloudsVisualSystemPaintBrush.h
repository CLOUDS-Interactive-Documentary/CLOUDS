//
//  CloudsVisualSystemPaintBrush.h
//  VSPaintBrush
//
//  Created by Patricio Gonzalez Vivo on 6/13/13.
//
//

#pragma once

#include "CloudsVisualSystemRezanator.h"

#include "Brush.h"
#include "ColorPalette.h"
#include "Button.h"

class CloudsVisualSystemPaintBrush : public CloudsVisualSystemRezanator {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfAutoMode();
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    
    void draw(ofEventArgs & args);
    
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
    
    void showPalette(int &_n );
    void cleanCanvas(int &_n );
    
    float   brushWidth;
    float   brushNumber;
    float   brushDamp;
    float   brushK;
    float   brushRepRad;
    float   brushRepPct;
    float   lineWidth;
    float   colorLerp;
    float   colorRandom;
    
    Brush           brush;
    ofFbo           canvas;
    
    ColorPalette    palette;
    
    SimpleBtn       paletteBtn;
    SimpleBtn       cleanBtn;
    
    int             iconSize;
    
    bool            bDebug;
    
};