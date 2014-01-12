
#pragma once

#include "ofMain.h"


class curvedLine {
    
public:
    
    void setup();
    void update();
    void draw();
    
    void translateNodeLine( ofPoint trans);
    
    float   timeStep;
    float   timeAccumulator;
    int     timeFrame;
    int     prevTimeFrame;
    
    ofPoint prevPosition;

    ofNode a,b,c;
    float aEnergy;
    float bEnergy;
    float aEnergySmth;
    float bEnergySmth;
    float time;
    
    ofPolyline nodeLine;
    
    int matchCount;
    
    ofPolyline nodeLineForMatch;

};
