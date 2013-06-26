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
    
    void    place( float _ra, float _dec );
   
	void	draw();
    
    string  constName;
    wStar   *connect;
    
    float   alpha;
};