//
//  CirclePacker.h
//  CirclePacking
//
//  Created by Jonathan Minard on 12/9/13.
//
//

#ifndef __CirclePacking__CirclePacker__
#define __CirclePacking__CirclePacker__

#include <iostream>
#include "ofMain.h"
#include "Circle.h"

//circle packing

class CirclePacker {
    
public:
    CirclePacker();
    CirclePacker(float _width, float _height);
    float fast_distance(float _x1, float _y1, float _x2, float _y2);
    void pack();
    void update();
    void draw();
    
    float width, height, padding, xcenter, ycenter;
    vector <Circle> circles; // vector
    
protected:
    
    float damping, iterations;
    
};


#endif /* defined(__CirclePacking__CirclePacker__) */
