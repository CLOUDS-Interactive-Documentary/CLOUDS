//
//  CloudsHUDLabel.h
//  CloudsHUDDesigner
//
//  Created by Charlie Whitney on 12/30/13.
//
//

#pragma once

#include "ofxFTGLSimpleLayout.h"
#include "ofRange.h"

class CloudsHUDLabel {
  public:
    CloudsHUDLabel();
    void setup( ofxFTGLSimpleLayout *textLayout, ofRectangle textBounds );
    void setup( ofxFTGLFont *textFont, ofRectangle textBounds );

    void draw();
    
    void setText(const string& newText, bool forceOn = true);
    string getText();
    
    int getRightEdge();
	
    ofxFTGLSimpleLayout *layout;
    ofxFTGLFont *font;
    ofRectangle bounds;

    ofRectangle baseInteractiveBounds;
    ofRectangle scaledInteractiveBounds;

    void animateIn(bool force = false);
    void animateOut(bool force = false);
    
    void instantIn();
    void instantOut();
    
    bool isVisible();
    bool isHovered();
    bool isClicked();
    
    //bool clearTextOnAnimateOut;
    
    bool caps;
    
    
	void mouseMoved(ofVec2f mouse);
	void mousePressed(ofVec2f mouse);
	void mouseReleased(ofVec2f mouse);
    
  protected:
	string  text;
    string type;
    
    ofRange animationClamp;
    float baseAnimationSpeed;
    float animationSpeed;
    float fadeOutSpeed;
    float beginTime;
    float pct;
    float textAlpha;
    int playhead;
    
    bool bIsAnimatingIn, bIsAnimatingOut;
    bool bIsVisible;
    
    bool bIsHovered;
    bool bIsPressed;
    bool bIsClicked;
    float hoverChangedTime;

};