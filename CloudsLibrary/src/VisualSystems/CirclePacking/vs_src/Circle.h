//
//  Circle.h
//  CirclePacking
//
//  Created by Jonathan Minard on 12/9/13.
//
//

#ifndef __CirclePacking__Circle__
#define __CirclePacking__Circle__

#include <iostream>
#include "ofMain.h"


class Circle {
public:
    
    string typeStr;
    ofTrueTypeFont  franklinBook14;
    ofTrueTypeFont	verdana14;
    static ofTrueTypeFont  Font;
    
    Circle();
    Circle(float _x, float _y, float _r, string _label);
    void draw();
    void drawCompanies();
    
    float distance(float _x1, float _y1, float _x2, float _y2);
    float getOffset(float _x, float _y);
    Boolean contains(float _x, float _y);
    Boolean intersect(Circle _circle);
    
    float growth;
    ofColor color1;
    float hue;
    float brightness;
    
    float x;
    float y;
    float r;
    string label;
    
};

#endif /* defined(__CirclePacking__Circle__) */