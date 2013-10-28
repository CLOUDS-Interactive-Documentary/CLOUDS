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
    
	float seed;
    int i;
    float maxX, minX, maxY, minY, maxZ, minZ;
    int nParticles;

    void init(int nParticles, ofColor color);
    void step();
    void draw();
    
    
    
  protected:
    
//    vector<particle> particles;
    ofVboMesh mesh;
  
};
