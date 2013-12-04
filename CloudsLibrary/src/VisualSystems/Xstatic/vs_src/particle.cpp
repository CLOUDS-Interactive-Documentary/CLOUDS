//
//  particle.cpp
//  Xstatic
//
//  Created by Jonathan Minard on 12/2/13.
//
//

#include "particle.h"

Particle::Particle(float velocityX, float velocityY, float velocityZ, int& minBri, int& maxBri){
    
    mass = 1;
    location.set(ofRandom(-100,100),ofRandom(-100,100),ofRandom(-100,100));
    velocity.set(velocityX, velocityY, velocityZ);
    acceleration.set(0, 0, 0);
    lumocity.set(1,1); 
    
    upperbounds.set(150,150,150);
    lowerbounds.set(-150,-150,-150);
    
    // timer
    
    triggered = false;
    //timerSet = true;
    luckyNumber = int(ofRandom(100));
 
    this->minBri = &minBri;
    this->maxBri = &maxBri;
    brightness = minBri;
    newBrightness = minBri;
    
}


void Particle::applyForce(ofVec3f force){
    
    ofVec3f f = force / mass;
    acceleration = acceleration + f;
    
}

void Particle::update(){
    
    currentTime = ofGetElapsedTimeMillis() - previousTime; 
    
    velocity = velocity + acceleration;
    location = location + velocity;
    acceleration = acceleration * 0;
    
    if(triggered == false){
    lottery();
    }
    
}

void Particle::display(){
    
    
    update(); 
    
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
        newBrightness = ofMap(currentTime, 0, 1000.0, *minBri, *maxBri);
        
        if(currentTime >= 1000.0){
            newBrightness = ofMap(currentTime, 1000.0, 2000.0, *maxBri,*minBri);
            if (brightness <= *minBri){
                triggered = false;
                newBrightness = *minBri+1;
            }
        }
        //float brightness = ofxTween::map(currentTime, 0, 1000.0, *minBri, *maxBri, true, eq, ofxTween::easeInOut);
    }
    
    brightness = newBrightness;
    cout << "triggered =  " << triggered <<  "  time = " << currentTime <<  "  brightness = " << brightness << endl;
    
//    ofSetColor(brightness,brightness,brightness);
//    ofCircle(location.x, location.y, location.z, mass*1);
    
    
}

void Particle::checkEdges(){
    
    if(location.x > upperbounds.x){
        location.x = upperbounds.x;
        velocity.x *= -1; 
    }
	if(location.y > upperbounds.y){
        location.y = upperbounds.y;
        velocity.y *= -1; 
        
    } 
	if(location.z > upperbounds.z){
        location.z = upperbounds.z;
        velocity.z *= -1;
    }
    if(location.x < lowerbounds.x){
        location.x = lowerbounds.x;
        velocity.x *= -1;
    }
	if(location.y < lowerbounds.y){
        location.y = lowerbounds.y;
        velocity.y *= -1;
        
    }
	if(location.z < lowerbounds.z){
        location.z = lowerbounds.z;
        velocity.z *= -1;
    }
}
    
void Particle::verticalWraparound(){
        
        if(location.x > upperbounds.x){
            location.x = upperbounds.x;
            velocity.x *= -1;
        }
        if(location.y > upperbounds.y){
            location.y = lowerbounds.y;
            
        }
        if(location.z > upperbounds.z){
            location.z = upperbounds.z;
            velocity.z *= -1;
        }
        if(location.x < lowerbounds.x){
            location.x = lowerbounds.x;
            velocity.x *= -1;
        }
        if(location.y < lowerbounds.y){
            location.y = upperbounds.y;
        }
        if(location.z < lowerbounds.z){
            location.z = lowerbounds.z;
            velocity.z *= -1;
        }
}
    
void Particle::lottery(){
    
    newRandomNumber = int(ofRandom(700));
    
    //cout << "newRandomNumber =  " << newRandomNumber << "  luckyNumber = " << luckyNumber << endl;
    
    if(newRandomNumber == luckyNumber){
        previousTime  = ofGetElapsedTimeMillis();
        brightness = *minBri+1; 
        triggered = true;
    }
    
    
}
