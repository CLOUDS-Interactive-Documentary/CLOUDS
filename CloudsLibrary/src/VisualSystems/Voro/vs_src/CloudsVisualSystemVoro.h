//
//  CloudsVisualSystemVoro.h
//
//  Created by Patricio Gonzalez Vivo on 5/27/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"
#include "Voro.h"
#include "VoroParticle.h"

class CloudsVisualSystemVoro : public CloudsVisualSystem {
  public:
    
    string getSystemName();
	
	void selfSetDefaults();
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
    
//    void billBoard();
    
protected:
    vector<VoroParticle *> seedParticles;
    vector<ofMesh>  cellMeshes;
        
    ofTexture   dot;
    ofTexture   nucles[28];
    ofVec3f     objectLookAt;
    
    //  Particle System
    //
    float   atractionPct,repulsionPct;
    float   bornRate, growRate;
    float   MaxNumOfParticles, MaxSize;
    float   initialForce;
    float   zMove;
    float   wallThikness;
    int     fps;
    bool    bClear;
    
    float   tSize;
    bool    bDrawGlow;
    bool    bDrawNucles;
    
    //  Voronoi
    //
    float   containerSize,containerHeight;
    float   cellsAlpha;
    float   cellsWireAlpha;
	float	cellsWireThickness;
    float   cellsVertexAlpha;
    float   cellsVertexSize;
	
    bool    containerPeriodic;
    bool    bSphere;
    bool    bCyllinder;
    
	bool	bUseMaterial;
};