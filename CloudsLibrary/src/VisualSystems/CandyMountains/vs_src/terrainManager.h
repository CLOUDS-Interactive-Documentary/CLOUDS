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
    void setup(colorSchemeDesigner * _cs);
    void update(vector<float> &wave);
    void draw();
    
    audioAnalytics * aa;
    colorSchemeDesigner * cs;
    
    vector<vector<float> > waveHistory;
    vector<ofFloatColor> waveHiHistory;
    vector<ofFloatColor> waveLoHistory;
    
    ofxMeshUtils meshUtils;
};
