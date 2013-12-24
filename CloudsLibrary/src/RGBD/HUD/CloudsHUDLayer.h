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
#include "CloudsSVGMesh.h"

typedef struct {
	string gid;
	ofRectangle rect;
} TextBounds;

class CloudsHUDLayer
{
  public:
	
	CloudsHUDLayer();
	
	void parse(string svgFilePath);
	
	vector<TextBounds> textBoxes;
	ofxSVG svg;
//	CloudsSVGMesh svg;
	
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
