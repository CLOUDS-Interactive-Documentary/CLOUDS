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
//#include "ofxTween.h"
#include <set>

class Particle {
 
public:
    
    int* minBri;
    int* maxBri;
    
    Particle(float vx, float vy, float vz, int& minBri, int& maxBri);
    
    void applyForce(ofVec3f _force);
    void update();
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
    
//protected:
    
    int mass;
    int brightness;
    float newBrightness;
    
    bool triggered;
    int luckyNumber;
    int newRandomNumber;
    int previousTime = 0;
    int currentTime;
        
};

#endif /* defined(__Xstatic__particle__) */
