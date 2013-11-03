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
    
    int ID; 
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
    void noiseRotate(float _x, float _y);
    float lerp(float _a, float _b, float _f); 
    
protected:
    
    float rx;
    float ry;
    float speed = .02;
    float rotNoise; 
    
    float period = 120; //period of oscillation
    
    float previousMouseX = 0;
    float previousMouseY = 0;

    
};