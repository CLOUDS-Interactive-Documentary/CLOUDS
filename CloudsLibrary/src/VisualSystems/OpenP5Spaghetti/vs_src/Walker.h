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
    float maxX, minX, maxY, minY, maxZ, minZ;
    static float nParticles;

    void init(int nParticles, ofColor color);
    void step();
    void smoothTrails();
    void gnarlyTrails();
    void draw();
    
    static float seed;
    static float noiseSpeed1;
    static float stepSize;
    
    static bool drawPoints;
	//float pointAlpha;
	static bool drawLines;
    
    
    
  protected:
    
//    vector<particle> particles;
    ofVboMesh mesh;
    
    
  
};
