//
//  CirclePacker.h
//  CirclePacking
//
//  Created by Jonathan Minard on 12/9/13.
//
//

#pragma once

#include "ofMain.h"
#include "Circle.h"

//circle packing

class CirclePacker {
    
public:
    CirclePacker();
    CirclePacker(float _width, float _height, float _padding);
    float fast_distance(float _x1, float _y1, float _x2, float _y2);
    void pack();
    void update();
    void draw(bool _a, bool _b, bool _c);
    
    float width, height, padding, xcenter, ycenter;
    list <Circle> circles; // vector
    
protected:
    
    float damping, iterations;
    
};
