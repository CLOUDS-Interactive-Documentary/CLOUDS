//
//  SOLPlayer.h
//  OpenP5SeaOfLines
//
//  Created by Elie Zananiri on 2013-11-01.
//
//

#pragma once

#include "ofMain.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class SOLPlayer
{
    public:
        SOLPlayer(float x, float y, float max);
        void update();
    
        static float width, height;
    
        ofColor col;
        float x, y;
        float size;
    
        float speed;
        float sx, sy;
};
