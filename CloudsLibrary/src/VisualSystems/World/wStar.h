//
//  wStar.h
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/25/13.
//
//  Reference: http://en.wikipedia.org/wiki/Equatorial_coordinate_system
#pragma once

#include "ofMain.h"

class wStar : public ofPoint {
public:
	wStar();
    
    void    pair( ofVec2f _ra, ofVec2f _dec);
   
	void	draw();
    
    string  constName;
    ofVec2f ra;     // analog to long, sideral hours (360° / 24h)
    ofVec2f dec;    // analog to lat
    wStar   *connect;
};