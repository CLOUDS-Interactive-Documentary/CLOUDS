//
//  Walker.h
//  OpenP5Spaghetti
//
//  Created by Jonathan Minard on 10/25/13.
//
//

#ifndef OpenP5Spaghetti_Walker_h
#define OpenP5Spaghetti_Walker_h

#include "ofMain.h"


class Walker {
    struct particle {
	ofVec3f position;
    ofColor color; 
};

public:
    
	float seed;
    int i;
    float maxX, minX, maxY, minY, maxZ, minZ;
    int nParticles;
    
    Walker();
    void init(int nParticles, ofColor color);
    void step();
    void draw();
    
    
    
protected:
    
    vector<particle>particles;
    ofVboMesh mesh;
  
};

#endif