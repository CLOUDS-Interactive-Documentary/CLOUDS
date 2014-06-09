//
//  terrainManager.h
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-02-14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxMeshUtils.h"
#include "audioAnalytics.h"
#include "colorSchemeDesigner.h"

class terrainManager {
public:
    void setup(audioAnalytics * _aa,
               colorSchemeDesigner * _cs);
    void update();
    void draw();
    
    audioAnalytics * aa;
    colorSchemeDesigner * cs;
    
    vector<vector<float> > waveHistory;
    vector<ofFloatColor> waveHiHistory;
    vector<ofFloatColor> waveLoHistory;
    
    ofxMeshUtils meshUtils;
};
