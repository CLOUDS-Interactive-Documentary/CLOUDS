//
//  ParkedCar.h
//  yearLong
//
//  Created by Surya on 31/03/13.
//
//

#ifndef __yearLong__ParkedCar__
#define __yearLong__ParkedCar__

#include <iostream>
#include "ofMain.h"
#include "Path.h"
#endif /* defined(__yearLong__ParkedCar__) */
class ParkedCar{
    
    
public:
    ParkedCar();
    ParkedCar(ofImage _img,ofVec2f _location);
        
    void update();
    void draw();
    void applyForce(ofVec2f force);
    void seek(ofVec2f target);
    void follow(Path p);
    ofVec2f getNormalPoint(ofVec2f p ,ofVec2f a, ofVec2f b);
   
    ofVec2f location;
    ofVec2f velocity;
    ofVec2f acceleration;
    float maxSpeed;
    float maxForce;
    ofImage img;


};