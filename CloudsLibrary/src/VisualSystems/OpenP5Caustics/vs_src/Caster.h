//
//  Caster.h
//  OpenP5Caustics
//
//  Created by Elie Zananiri on 11/13/2013.
//
//

#pragma once

#include "ofMain.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class Caster
{
    public:
        Caster(int width, int height);
        ~Caster();
        
        void reset();
        void cast(float x, float y, float nX, float nY, float nZ, float thetaI);
        
        float * bg;
        int count;
        int width, height;
    
        float n;  // Index of refraction
        float d;  // Distance to back wall
        float intensity;
        
        bool bReset;
        long loopCount;
};
