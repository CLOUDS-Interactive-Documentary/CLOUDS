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
    ofVec3f axis;
    float angle;
    ofMesh mesh;
    
    Tetra(float posX, float posY, float posZ, float edgeLength);
    void drawShape();
    void makeShape();
    void noiseRotate(float _x, float _y);
    float lerp(float _a, float _b, float _f); 
    
protected:
	
    float edgeLength;
    
    float rx;
    float ry;
    float speed ;
    float rotNoise; 
    
    float period ; //period of oscillation
    
    float previousMouseX ;
    float previousMouseY ;

    
};