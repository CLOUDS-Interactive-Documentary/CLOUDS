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
    
	
    int i;
    float j;
    float maxX, minX, maxY, minY, maxZ, minZ;
    int nParticles;

    void init(int nParticles, ofColor color);
    void step();
    void smoothTrails();
    void gnarlyTrails();
    void draw();
    
    float seed;
    float noiseSpeed1;
    float stepSize;
    
    bool drawPoints;
	//float pointAlpha;
	bool drawLines;
    
    
    
  protected:
    
//    vector<particle> particles;
    ofVboMesh mesh;
    
    
  
};
