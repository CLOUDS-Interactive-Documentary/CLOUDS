//
//  particle.cpp
//  Xstatic
//
//  Created by Jonathan Minard on 12/2/13.
//
//

#include "particle.h"

Particle::Particle(float velocityX, float velocityY, float velocityZ){
    
    mass = 1;
    location.set(ofRandom(-100,100),ofRandom(-100,100),ofRandom(-100,100));
    velocity.set(velocityX, velocityY, velocityZ);
    acceleration.set(0, 0, 0);
    
    upperbounds.set(150,150,150);
    lowerbounds.set(-150,-150,-150);
    
    // timer
    
    triggered = false;
    //timerSet = true;
    luckyNumber = int(ofRandom(100));
    brightness = 50;
    
}


void Particle::applyForce(ofVec3f force){
    
    ofVec3f f = force / mass;
    acceleration = acceleration + f;
    
}

void Particle::update(){
    
    velocity = velocity + acceleration;
    location = location + velocity;
    acceleration = acceleration * 0;
    
    if(triggered == false){
    lottery();
    }
    
}

void Particle::display(){
    
   
    if(triggered == true){
        brightness += 5;
    }
    if (brightness >= 240){
        brightness = 50;
        triggered = false;
    }
    
    //cout << "triggered =  " << triggered << "  brightness = " << brightness << endl;
    
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
    
    newRandomNumber = int(ofRandom(2000));
    
    //cout << "newRandomNumber =  " << newRandomNumber << "  luckyNumber = " << luckyNumber << endl;
    
    if(newRandomNumber == luckyNumber){
        triggered = true; 
    }
    
}
