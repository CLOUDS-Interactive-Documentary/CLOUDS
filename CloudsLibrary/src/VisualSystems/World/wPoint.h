//
//  wPoint.h
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/24/13.
//
//

#pragma once

#include "ofMain.h"

class wPoint : public ofPoint {
public:
    
    wPoint();
    
    void place(float _lat, float _long);
    void update();
    void draw();

    ofColor color;
    
    ofQuaternion latRot,longRot;
    float latitud, longitud;
    float *noisePeaks;
    float rippleDeepnes;
    
    bool bRipple;
    
};
