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
    float   latitud, longitud;
    float   size;
    
    float   noise;
    float   nNoise;
    float   *noisePeaks;
    float   *noiseThreshold;
    
    float   ripplePct;
    float   rippleScale;
    
    bool    bRipple;
    
private:
    void drawRipple( float _pct );
    
};
