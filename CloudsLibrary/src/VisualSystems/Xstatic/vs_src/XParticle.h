//
//  particle.h
//  Xstatic
//
//  Created by Jonathan Minard on 12/2/13.
//
//

#pragma once

#include "ofMain.h"

class XParticle
{ 
public:
    XParticle();
    XParticle(ofVec3f pos);
    
    void applyForce(ofVec3f force);
    void update(float drag);
    
    void bounceEdges();
    void wrapEdges();
    
    void lottery();
    
    float mappedSize();
    
    ofVec3f location;
    ofVec3f velocity;
    ofVec3f acceleration;
    ofVec2f lumocity; 
    
    static float dt;
    static ofVec3f upperBounds;
    static ofVec3f lowerBounds;
    static float diagonalSquared;
    
    static float minSize;
    static float maxSize;
    static float heightToSize;
    static float velToSize;

    static float minBri;
    static float maxBri;
    
    static float oscSpeed;
    
    static float triggerDuration;
    static float triggerRange;
    
    static bool bOscillateSize;
    static bool bOscillateBri;
    
    float size;
    float colorPicker;
    float brightness;
    
    float oscPos;
        
protected:
    bool bTriggered;
    int luckyNumber;
    int newRandomNumber;
    int previousTime = 0;
    int currentTime;
};
