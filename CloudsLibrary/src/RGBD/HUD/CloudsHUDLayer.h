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

class CloudsHUDLayer
{
	static ofShader lineShader;
  public:
	
	CloudsHUDLayer();
    void load(string svgPath);
		
	vector<string> textBoxIds;
	CloudsSVGMesh svg;
	
	void start(bool animate = true);
    void close(bool animate = true);
    bool isOpen();

    bool bForceHover;
    void hoverOn();
    void hoverOff();
    bool isHovering();
    
	void update();
	void draw();
	   
	bool animatingFade;
    bool animatingHover;
	float fadeStartTime;
    float hoverStartTime;
	float duration;
	
	float animateOnPercentComplete;
    float hoverOnPercentComplete;
    
  protected:
    bool bIsOpen;
    bool bIsHovering;
    string fileName;
};
