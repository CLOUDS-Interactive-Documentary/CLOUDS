//
//  CloudsVisualSystemTerrain.h
//  VSTerrain
//
//  Created by Patricio Gonzalez Vivo on 5/28/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"

class CloudsVisualSystemTerrain : public CloudsVisualSystem {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    void selfGuiEvent(ofxUIEventArgs &e);
    
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
 
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
    void billBoard();

        // if you use a custom camera to fly through the scene
    // you must implement this method for the transitions to work properly
//  ofCamera& getCameraRef(){
//      return myCustomCamera;
//  }

    //
  
    
protected:
    void     setResolution( int _width, int _height );
    
    ofxUISuperCanvas* customGui;
    bool customToggle;
    
    // Noise
    //
    ofShader    noiseShader;
    ofFbo       noiseFbo;
    float       noiseZoom;
    float       noiseSpeed;
    
    //  Normals
    //
    ofShader    normalsShader;
    ofFbo       normalsFbo;
    
    //  Terrain
    //
    float       terrainResolution;
    float       terrainHeight;
    float       size;
    
    void        makeTerrain( ofTexture &_heightMap );
    ofVbo       terrainVbo;
        ofVec3f*    pNormals;
    ofVec3f*    pVertices;          // Vertex Data
    ofVec2f*    pTexCoords;         // Texture Coordinates
        int         nVertexCount;       // Vertex Count
    bool        bChange;
    
    //  Texture
    //
    ofShader patternShader;
    ofFbo    patternFbo;
    float    patternScale;
    float    stripeAlpha;
    float    hexAlpha;
    float    dotsAlpha;
    
    //  Camera
    //
    ofVec2f     camPosition;
    float       camAltitud;
};