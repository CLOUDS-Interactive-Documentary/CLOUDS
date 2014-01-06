//
//  particle.cpp
//  Xstatic
//
//  Created by Jonathan Minard on 12/2/13.
//
//

#include "XParticle.h"

float   XParticle::dt;
ofVec3f XParticle::upperBounds;
ofVec3f XParticle::lowerBounds;
float   XParticle::diagonalSquared;
float   XParticle::minSize;
float   XParticle::maxSize;
float   XParticle::heightToSize;
float   XParticle::velToSize;
float   XParticle::minBri;
float   XParticle::maxBri;
float   XParticle::oscSpeed;
float   XParticle::triggerDuration;
float   XParticle::triggerRange;
bool    XParticle::bOscillateSize;
bool    XParticle::bOscillateBri;

XParticle::XParticle()
{
    location.set(ofRandom(lowerBounds.x, upperBounds.x),
                 ofRandom(lowerBounds.y, upperBounds.y),
                 ofRandom(lowerBounds.z, upperBounds.z));
    velocity.set(0, 0, 0);
    acceleration.set(0, 0, 0);
    lumocity.set(1,1); 
    
    bTriggered = false;
    luckyNumber = int(ofRandom(10));
    //mass = 1;
 
    brightness = minBri;
    
    oscPos = ofRandom(100);
    
    colorPicker = ofRandomuf();
}

XParticle::XParticle(ofVec3f pos){
    location.set(pos.x,pos.y,pos.z);
    velocity.set(0, 0, 0);
    acceleration.set(0, 0, 0);
    lumocity.set(1,1);
    
    bTriggered = false;
    luckyNumber = int(ofRandom(10));
    
    brightness = minBri;
    
    oscPos = ofRandom(100);
    
    colorPicker = ofRandomuf();
}

void XParticle::applyForce(ofVec3f force)
{    
    //ofVec3f f = force / *mass; << the right way of calculating Newton's 2nd takes mass into consideration in calculating force
//    ofVec3f f = force / 1.5; //let's pretend all the masses are the same
    acceleration += force;
}

void XParticle::update(float drag)
{    
    currentTime = ofGetElapsedTimeMillis() - previousTime;
    
    velocity += (acceleration * dt);
    velocity -= (velocity * dt * drag);
    location += (velocity * dt);
    acceleration -= (acceleration * dt);
    
    bool bShouldDropTrigger = false;
    
    if (bOscillateSize) {
        size = minSize + (sinf(oscPos) + 1) * (maxSize - minSize);
    }
    else if (bTriggered) {
        float newSize;
        if (currentTime >= triggerDuration) {
            newSize = ofMap(currentTime, triggerDuration, triggerDuration * 2, maxSize, minSize);
            if (size <= minSize) {
                newSize = minSize;
                bShouldDropTrigger = true;
            }
        }
        else {
            newSize = ofMap(currentTime, 0, triggerDuration, minSize, maxSize);
        }
        size = newSize;
    }
    
    if (bOscillateBri) {
        brightness = ofMap(sinf(oscPos), -1, 1, minBri, maxBri);
    }
    else if (bTriggered) {
        float newBri;
        if (currentTime >= triggerDuration) {
            newBri = ofMap(currentTime, triggerDuration, triggerDuration * 2, maxBri, minBri);
            if (brightness <= minBri) {
                newBri = minBri;
                bShouldDropTrigger = true;
            }
        }
        else {
            newBri = ofMap(currentTime, 0, triggerDuration, minBri, maxBri);
        }
        brightness = newBri;
    }
    
    if (bOscillateSize || bOscillateBri) {
        oscPos += oscSpeed;
    }
    
    if (bShouldDropTrigger) {
        bTriggered = false;
    }
    
    if (!bTriggered && (!bOscillateSize || !bOscillateBri)) {
        lottery();
    }
}

void XParticle::bounceEdges()
{    
    if (location.x > upperBounds.x){
        location.x = upperBounds.x;
        velocity.x *= -1; 
    }
    else if (location.x < lowerBounds.x) {
        location.x = lowerBounds.x;
        velocity.x *= -1;
    }
    
	if (location.y > upperBounds.y) {
        location.y = upperBounds.y;
        velocity.y *= -1; 
        
    } 
    else if (location.y < lowerBounds.y) {
        location.y = lowerBounds.y;
        velocity.y *= -1;
        
    }

	if (location.z > upperBounds.z) {
        location.z = upperBounds.z;
        velocity.z *= -1;
    }
	else if(location.z < lowerBounds.z) {
        location.z = lowerBounds.z;
        velocity.z *= -1;
    }
}
    
void XParticle::wrapEdges()
{
    if (location.x > upperBounds.x) {
        location.x -= (upperBounds.x - lowerBounds.x);
    }
    else if (location.x < lowerBounds.x) {
        location.x += (upperBounds.x - lowerBounds.x);
    }

    if (location.y > upperBounds.y) {
        location.y -= (upperBounds.y - lowerBounds.y);
    }
    else if (location.y < lowerBounds.y) {
        location.y += (upperBounds.y - lowerBounds.y);
    }
    
    if (location.z > upperBounds.z) {
        location.z -= (upperBounds.z - lowerBounds.z);
    }
    else if (location.z < lowerBounds.z) {
        location.z += (upperBounds.z - lowerBounds.z);
    }
}

void XParticle::lottery()
{    
    newRandomNumber = int(ofRandom(triggerRange));
    if (newRandomNumber == luckyNumber) {
        previousTime  = ofGetElapsedTimeMillis();
        if (!bOscillateSize) {
            size = minSize;
        }
        if (!bOscillateBri) {
            brightness = minBri;
        }
        bTriggered = true;
    }
}

float XParticle::mappedSize()
{
    float sz = (size * (1.0f - heightToSize) + size * ofMap(location.y, lowerBounds.y, upperBounds.y, 0, 1,true) * heightToSize);
    return (sz * (1.0f - velToSize) + sz * ofMap(velocity.lengthSquared(), 0, diagonalSquared, 0, 1,true) * velToSize);
}
