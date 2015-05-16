//
//  CloudsHUDLayer.h
//  CloudsHUDDesigner
//
//  Created by James George on 12/14/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxTween.h"
#include "CloudsSVGMesh.h"

class CloudsHUDScroller {
  public:
    CloudsHUDScroller();
    
    void          update();

    float         scrollPosition;
    float         totalScrollHeight;
    
    ofRectangle   scrollBounds;
    ofRectangle   scrollBoundsScaled;
    ofRectangle   scrollUpBounds;
    ofRectangle   scrollDownBounds;
    ofRectangle   scrollUpBoundsScaled;
    ofRectangle   scrollDownBoundsScaled;
    
    bool          bIsScrollUpHover;
    bool          bIsScrollDownHover;
    bool          bIsScrollUpPressed;
    bool          bIsScrollDownPressed;
    bool          bIsHoldScrolling;
    
    float         scrollPressedTime;
    float         scrollIncrement;
    
    bool          mouseMoved(ofVec2f pos);
    bool          mousePressed(ofVec2f pos);
    void          mouseDragged(ofVec2f pos);
    void          mouseReleased(ofVec2f pos);
    
};
