//
//  CloudsHUDLabel.h
//  CloudsHUDDesigner
//
//  Created by Charlie Whitney on 12/30/13.
//
//

#pragma once

#include "ofxFTGLSimpleLayout.h"

class CloudsHUDLabel {
public:

    void draw(){
        layout->drawString("TEST STRING", bounds.x, bounds.y + layout->getLineHeight() );
    };
    
    ofxFTGLSimpleLayout *layout;
    ofRectangle bounds;
};