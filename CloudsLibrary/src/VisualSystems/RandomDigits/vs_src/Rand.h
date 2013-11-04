//
//  Rand.h
//  OpenP5SpinningSolids
//
//  Created by Jonathan Minard on 11/2/13.
//
//

#pragma once 
#include "ofMain.h"
//#include "ofxFTGL.h"

class Rand {
    
public:
    
    int ID; 
    float posX;
    float posY;
    float posZ;
    ofVec3f axis;
    float angle;
    ofMesh mesh;
    
    Rand(float posX, float posY, float posZ);
    void setup(); 
    void noiseRotate(float _x, float _y);
    void drawNumbers();
    float lerp(float _a, float _b, float _f);
    
    // text
    //FTGL
    //string text;
    //float fontSize;
    //static ofxFTGLSimpleLayout font;
    
    string typeStr;
    
    ofTrueTypeFont  franklinBook14;
    ofTrueTypeFont	verdana14;
    static ofTrueTypeFont  Font;
    
protected:
    
    float rx;
    float ry;
    float speed = .02;
    float rotNoise; 
    
    float period = 120; //period of oscillation
    
    float previousMouseX = 0;
    float previousMouseY = 0;

    
};