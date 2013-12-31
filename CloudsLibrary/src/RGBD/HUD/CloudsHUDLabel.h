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
    CloudsHUDLabel();
    void setup( ofxFTGLSimpleLayout *textLayout, ofRectangle textBounds );
    void draw();
    
    void setText( string newText );
    
    ofxFTGLSimpleLayout *layout;
    ofRectangle bounds;
    string  text;
};