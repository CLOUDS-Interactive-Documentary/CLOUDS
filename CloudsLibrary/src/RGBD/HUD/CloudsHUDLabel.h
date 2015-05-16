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
    void setDynamicMargin();
    void updateDynamicSize();
    void makeArrowPositionDynamic();
    
    void draw();
    
    void setText(const string& newText, bool forceOn = true);
    string getText();
    
    int getRightEdge();
	
    ofxFTGLSimpleLayout *layout;
    ofxFTGLFont *font;
    ofRectangle bounds;

    ofRectangle baseInteractiveBounds;
    ofRectangle scaledInteractiveBounds;
    
    //for dynamic backing text
    bool bDynamicBacking;
    ofRectangle dynamicBackingBounds;
    ofMesh* dynamicBackingMesh;

    void animateIn(bool force = false);
    void animateOut(bool clear = false);
    
    void instantIn();
    void instantOut();
    
    bool isVisible();
    bool isHovered();
    bool isClicked();
    void deselect();
    
    bool tab; //dont' deselect on other click
    bool tabSelected;
    bool caps;
    float hoverAlpha;
    
    bool hasTriangle;
    ofMesh triangleMesh;
    
	bool mouseMoved(ofVec2f mouse);
	bool mousePressed(ofVec2f mouse);
	bool mouseReleased(ofVec2f mouse);
    
    
  protected:
	string  text;
    string type;

    bool usesLayout();
    bool usesFont();
    
    
    bool clearTextOnAnimateOut;

    ofRange animationClamp;
    float baseAnimationSpeed;
    float animationSpeed;
    float fadeOutSpeed;
    float beginTime;
    float pct;
    float textAlpha;
    float dynamicBackingMargin;
    
    bool arrowPositionDynamic;
    float arrowBaseRightEdge;
    
    int playhead;
    
    bool bIsAnimatingIn, bIsAnimatingOut;
    bool bIsVisible;
    
    bool bIsHovered;
    bool bIsPressed;
    bool bIsClicked;
    float hoverChangedTime;

};