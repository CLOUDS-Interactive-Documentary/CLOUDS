//
//  CloudsHUDLayer.h
//  CloudsHUDDesigner
//
//  Created by James George on 12/14/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxSvg.h"
#include "ofxTween.h"

class CloudsHUDLayer
{
  public:
	
	CloudsHUDLayer(string svgFilePath);
	
	ofxSVG svg;
	
	void start();
	void update();
	void draw();
	
	bool animating;
	float delayTime;
	float startTime;
	float duration;
	
	float maxUpdateInterval;
	float lastUpdateTime;
	
	ofVec2f startPercent;
	
	ofVec2f percentComplete;
	float xlag;
	ofRectangle drawRect;
	ofVec2f startPoint;
	ofVec2f endPoint;
	
};
