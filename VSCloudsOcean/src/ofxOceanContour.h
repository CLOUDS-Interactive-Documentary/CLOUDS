/**
 *  ofxOceanContour.h
 *  PSWaves
 *
 *  Created by Jim on 9/28/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxOcean.h"

class ofxOceanContour {
  public:
	
	ofxOceanContour();
	string moodName;
	int contourNumber;
	
	ofFloatColor baseColor;
	ofFloatColor accentColor;
	
	float age;
	ofVec2f velocity;
	
	ofVec2f startpos;
	ofVec2f endpos;
	float thickness;
	float sampleStep;
	
	void update();
	void draw();
	
	ofxOcean* ocean;
	float fftOffset;
	float* fftData;
	int fftDataLength;
	float fftScale;
};