#pragma once

#include "ofMain.h"
#include "CloudsFCPParser.h"
#include <algorithm> 
//#include "../../../CloudsLibrary/VisualSystems"
#include "CloudsVisualSystemRezanator.h"
#include "VoroParticle.h"
#include "Voro.h"


class testApp : public ofBaseApp{
  public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
    void getMedianValues();

    CloudsFCPParser parser;
    ofTrueTypeFont font;
    vector<string> keyWords;


  
    float maxD;
    float minD;
    float maxClip;
    float minClip;
    float maxDist;
    float minDist;
protected:
    vector<float>medianValues;
    vector<ofVec2f> centres;
    vector<float> maxDistances;
    vector<ofColor> colors;
    vector <VoroParticle *> seedParticles;
    vector<ofMesh>  cellMeshes;
    vector<ofPolyline> cells;
    vector<ofPoint> cohesiveCells;
    ofTexture dot;
    ofVec3f objectLookAt;
    int particleIndex;
    //  Particle System
    //
    float   atractionPct,repulsionPct;
    float   bornRate, growRate;
    float   MaxNumOfParticles, MaxSize;
    float   initialForce;
    float   zMove;
    int     fps;
    bool    bClear,bDrawParticles;
    float   glowSize;
    bool    bDrawGlow;
    
    //  Voronoi
    //
    float   containerSize,containerHeight;
    float   cellsAlpha;
    float   cellsWireAlpha;
    bool    containerPeriodic;
    bool    bSphere;
    bool    bCyllinder;
    bool    bDrawVoronoi;
    bool    bDrawVoronoiWireFrames;
};
