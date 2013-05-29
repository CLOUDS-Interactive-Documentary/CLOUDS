//
//  CloudsVisualSystemTerrain.h
//  VSTerrain
//
//  Created by Patricio Gonzalez Vivo on 5/28/13.
//
//

#pragma once

#include "CloudsVisualSystemRezanator.h"

class CloudsVisualSystemTerrain : public CloudsVisualSystemRezanator {
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
    void     setResolution( int _width, int _height );
    
    // Noise;
    //
    ofShader noiseShader;
    ofFbo    noiseFbo;
    float    noiseSpeed,noiseZoom;
    
    // GraysCott
    //
    ofShader grayscottShader;
    ofFbo    grayscottFbo[2];
    float    diffU,diffV,k,f;
    float    grayscottLoops;
    int      nPingPong;
    bool     bCleanGrayscott;
    bool     bGrayscott;
    float    grayscottFade;
    
    //  Normals
    //
    ofShader normalsShader;
    ofFbo    normalsFbo;
    
    //  Terrain
    //
    float       terrainHeight;
    
    //  VBO Terrain
    //
    void        makeTerrain( ofTexture &_heightMap );
    void        drawVboTerrain();
    ofVbo       terrainVbo;
	ofVec3f*    pNormals;
    ofVec3f*    pVertices;          // Vertex Data
    ofVec2f*    pTexCoords;         // Texture Coordinates
	int         nVertexCount;       // Vertex Count
    float       terrainResoultion;
    
    //  ofMesh Terrain
    //
    void        makeMesh(ofTexture &_heightMap);
    ofShader    extrusionShader;
    ofMesh      terrainMesh;
    
    float   size;
    
};