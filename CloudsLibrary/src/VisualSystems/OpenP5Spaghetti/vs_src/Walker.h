//
//  Walker.h
//  OpenP5Spaghetti
//
//  Created by Jonathan Minard on 10/25/13.
//
//

#pragma once

#include "ofMain.h"


class Walker {

  public:
	Walker();
	
	ofVec3f position;
    ofFloatColor color;
    
	
    static int i;
    static float j;
    static float nParticles;
    
    float maxX, minX, maxY, minY, maxZ, minZ;
    

    void init(int nParticles, ofColor color);
    void step();
    void smoothTrails();
    void gnarlyTrails();
    void draw();
    
    //random seed for noise
    float seed;
    static float noiseSpeedx;
    static float noiseSpeedy;
    static float noiseSpeedz;
    
    static float stepSizex;
    static float stepSizey;
    static float stepSizez;
    
    static bool drawPoints;
	//float pointAlpha;
	static bool drawLines;
    
    //the number of vertices in the mesh
    int numVertices;
    
    
    
  protected:
    
//    vector<particle> particles;
    ofVboMesh mesh;
    
    
  
};
