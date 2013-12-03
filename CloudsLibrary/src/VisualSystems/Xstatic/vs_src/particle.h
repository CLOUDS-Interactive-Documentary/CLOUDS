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

class Particle {
 
public:
    Particle(float vx, float vy, float vz);
    
    void applyForce(ofVec3f _force);
    void update();
    void display();
    void checkEdges();
    void verticalWraparound();
    void lottery();
    
    ofVec3f location;
    ofVec3f velocity;
    ofVec3f acceleration;
    
    ofVec3f upperbounds;
    ofVec3f lowerbounds; 
    
protected:
    
    int mass;
    int brightness;
    bool triggered;
    int luckyNumber;
    int newRandomNumber;
        
};

#endif /* defined(__Xstatic__particle__) */
