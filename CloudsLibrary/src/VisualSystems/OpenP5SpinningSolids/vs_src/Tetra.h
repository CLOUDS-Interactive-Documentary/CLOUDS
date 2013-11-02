//
//  Tetra.h
//  OpenP5SpinningSolids
//
//  Created by Jonathan Minard on 11/2/13.
//
//

#pragma once 
#include "ofMain.h"

class Tetra {
    
public:
    
    float posX;
    float posY;
    float posZ;
    float edgeLength;
    ofVec3f axis;
    float angle;
    ofMesh mesh;
    
    Tetra(float posX, float posY, float posZ, float edgeLength);
    void drawShape();
    void makeShape();
    
protected:
    
    float rx;
    float ry;
    
};