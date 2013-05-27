//
//  CloudsVisualSystemCities.h
//  VSCloudsRezanator
//
//  Created by Patricio Gonzalez Vivo on 5/26/13.
//
//

#pragma once

#include "CloudsVisualSystemRezanator.h"

class CloudsVisualSystemCities : public CloudsVisualSystemRezanator {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfAutoMode();
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
    
    void billBoard();
    
protected:
    void    makeGrid(float _size, int _nGrains);
    
    ofShader noiseShader;
    ofFbo    noiseFbo;
    
    ofShader grayscottShader;
    ofFbo    grayscottFbo[2];
    int      nPingPong;
    bool     bCleanGrayscott;
    
    ofShader maskShader;
    ofFbo   maskFbo;
    
    GLuint  cube;
    
    //  Noise
    //
    float   noiseSpeed,noiseZoom;
    
    //  GrayScott
    //
    float   grayscottFade;
    
    //  Mask
    //
    float   maskSize,maskCurve;
    
    //  Blocks
    //
    float   size, nGrains, grainResolution;
    float   minSize,maxHeight,minAlpha,minDist;
    
};