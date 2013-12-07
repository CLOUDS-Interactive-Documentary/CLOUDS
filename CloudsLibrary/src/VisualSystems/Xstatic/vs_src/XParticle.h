//
//  particle.h
//  Xstatic
//
//  Created by Jonathan Minard on 12/2/13.
//
//

#ifndef __Xstatic__particle__
#define __Xstatic__particle__

#include <iostream>
#include "ofMain.h"
#include <set>

class XParticle {
 
public:
    
    int* minBri;
    int* maxBri;
    float* mass;
    
    XParticle(float& mass, float vx, float vy, float vz, int& minBri, int& maxBri);
    
    void applyForce(ofVec3f _force);
    void update(float _topspeed);
    void display();
    void checkEdges();
    void verticalWraparound();
    void lottery();
    
    ofVec3f location;
    ofVec3f velocity;
    ofVec3f acceleration;
    ofVec2f lumocity; 
    
    ofVec3f upperbounds;
    ofVec3f lowerbounds; 
    
    float uniqueVal;
    int brightness;
    
protected:
    
    float newBrightness;
    
    bool triggered;
    int luckyNumber;
    int newRandomNumber;
    int previousTime = 0;
    int currentTime;
        
};

#endif /* defined(__Xstatic__particle__) */
