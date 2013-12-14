//
//  Circle.cpp
//  CirclePacking
//
//  Created by Jonathan Minard on 12/9/13.
//
//

#include "Circle.h"

Circle::Circle()
{
    
}

Circle::Circle(float _x, float _y, float _r, string _label)
{
    this->x = _x; //this->x
    this->y = _y;
    this->r = _r;
    this->label = _label;
}

float Circle::distance(float _x1, float _y1, float _x2, float _y2)
{
    return sqrt((_x1-_x2)*(_x1-_x2)+(_y1-_y2)*(_y1-_y2));
}

float Circle::getOffset(float _x, float _y)
{
    return ofDist(this->x, this->y, _x, _y); // ofDist
}

Boolean Circle::contains(float _x, float _y)
{
    return ofDist(this->x, this->y, _x, _y) <= this->r;
}

Boolean Circle::intersect(Circle _circle)
{
    float d = ofDist(this->x, this->y, _circle.x, _circle.y);
    return d <= (this->r + _circle.r);
}

void Circle::draw()
{
    ofCircle(x, y, r);
}