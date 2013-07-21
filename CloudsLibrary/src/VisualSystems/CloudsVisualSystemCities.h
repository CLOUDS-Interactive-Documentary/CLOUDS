//
//  CloudsVisualSystemCities.h
//  VSCloudsRezanator
//
//  Created by Patricio Gonzalez Vivo on 5/26/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"

class CloudsVisualSystemCities : public CloudsVisualSystem {
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
    void    makeGrid(float _size, int _resolution);
    void    generateCube(float _sizeX, float _sizeY, float _sizeZ);
    float   size, resolution, height;
    
    //  Noise
    //
    ofShader noiseShader;
    ofFbo    noiseFbo;
    float    noiseSpeed,noiseZoom;
    
    //  GrayScot
    //
    ofShader grayscottShader;
    ofFbo    grayscottFbo[2];
    float    diffU,diffV,k,f;
    float    grayscottLoops;
    float    grayscottFade;
    int      nPingPong;
    bool     bCleanGrayscott;
    bool     bGrayscott;
    
    //  Mask
    //
    ofShader maskShader;
    ofFbo    maskFbo;
    float   maskSize,maskCurve;
    
    //  Blocks
    //
    float   blocksMinSize,blocksMinDist,blocksAlpha;
    float   blockSize;
    GLuint  cube;
};