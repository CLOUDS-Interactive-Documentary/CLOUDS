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
float   XParticle::briDuration;
float   XParticle::briRange;

XParticle::XParticle(float& mass, float velocityX, float velocityY, float velocityZ, int& minBri, int& maxBri)
{
    location.set(ofRandom(lowerBounds.x, upperBounds.x),
                 ofRandom(lowerBounds.y, upperBounds.y),
                 ofRandom(lowerBounds.z, upperBounds.z));
    velocity.set(velocityX, velocityY, velocityZ);
    acceleration.set(0, 0, 0);
    lumocity.set(1,1); 
    
    // timer
    
    triggered = false;
    //timerSet = true;
    luckyNumber = int(ofRandom(10));
 
    this->minBri = &minBri;
    this->maxBri = &maxBri;
    this->mass = &mass;
    brightness = minBri;
    newBrightness = minBri;
    
    float uniqueVal = ofRandom(-10000, 10000);
    
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
    velocity -= (velocity * dt * (1.0 - drag));
    location += (velocity * dt);
    acceleration -= (acceleration * dt);
    
//    velocity.limit(topspeed);
//    
    if(triggered == false){
    lottery();
    }
    
}

void XParticle::display(){
    
    
    //ofxEasingQuad eq;
   
    /*
    if(triggered == true){
        brightness = brightness + lumocity.x;
    }
    if (brightness >= 240){
        brightness = 50;
        triggered = false;
    }
     */
    
    if (triggered == true){
        //brightness = brightness + 2.0;  //
        newBrightness = ofMap(currentTime, 0, briDuration, *minBri, *maxBri);
       // *mass = ofMap(currentTime, 0, 1000.0, .1, 2);
        
        if(currentTime >= briDuration){
            newBrightness = ofMap(currentTime, briDuration, briDuration * 2, *maxBri,*minBri);
            //*mass = ofMap(currentTime, 1000.0, 2000.0, 2, .1);
            
            if (brightness <= *minBri){
                triggered = false;
                newBrightness = *minBri+1;
               // *mass = .1;
            }
        }
        //float brightness = ofxTween::map(currentTime, 0, 1000.0, *minBri, *maxBri, true, eq, ofxTween::easeInOut);
    }
    
    brightness = newBrightness;
   // cout << "triggered =  " << triggered <<  "  time = " << currentTime <<  "  brightness = " << brightness << endl;
    
//    ofSetColor(brightness,brightness,brightness);
//    ofCircle(location.x, location.y, location.z, mass*1);
    
    
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

void XParticle::lottery(){
    
    newRandomNumber = int(ofRandom(briRange));
    
    //cout << "newRandomNumber =  " << newRandomNumber << "  luckyNumber = " << luckyNumber << endl;
    
    if(newRandomNumber == luckyNumber){
    
//    if (ofRandomuf() < 0.001f) {
        previousTime  = ofGetElapsedTimeMillis();
        brightness = *minBri+1; 
        triggered = true;
    }
    
    
}
