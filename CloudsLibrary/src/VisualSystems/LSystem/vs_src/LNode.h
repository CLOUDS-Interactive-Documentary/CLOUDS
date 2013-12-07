//
//  LNode.h
//  VSCloudsLSystem
//
//  Created by Patricio Gonzalez Vivo on 7/18/13.
//
// yellow slime mode tokio subway

#pragma once
#include "ofMain.h"

class LNode : public ofPoint {
public:
    
    float   startTime;
    
    float   trailsPct;
    vector< vector<ofPoint> > trails;
    
    vector<int> branchesIndex;
    
    
};
