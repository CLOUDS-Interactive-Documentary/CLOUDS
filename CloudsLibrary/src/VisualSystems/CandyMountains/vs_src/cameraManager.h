//
//  cameraManager.h
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-02-14.
//
//

#pragma once

#include "smoother.h"
#include "decayer.h"
#include "terrainManager.h"

class cameraManager {
public:
    void setup(terrainManager * _tm);
    void update();
    
    terrainManager * tm;
    
    ofCamera cam;
//    ofEasyCam cam;
    void begin() { cam.begin(); }
    void end() { cam.end(); }
    
    float camX, camY, camZ;
    float lastCamX, lastCamY;
    float lookatX, lookatY, lookatZ;
    
    decayer camYDecayed;
    smoother camXSmoothed, camYSmoothed;
    
    int camXLookAhead, camYLookAhead;
    int camXRange;
    int center;
    
};