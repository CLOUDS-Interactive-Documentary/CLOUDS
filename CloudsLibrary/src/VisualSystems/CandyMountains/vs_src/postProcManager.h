//
//  postProcManager.h
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-02-14.
//
//

#pragma once
#include "ofMain.h"
#include "ofxPostProcessing.h"

class postProcManager {
public:
    void setup();
    void keyPressed(int key);
    
    float width, height;
    ofxPostProcessing post;
    vector<RenderPass::Ptr> renderPasses;
    
};