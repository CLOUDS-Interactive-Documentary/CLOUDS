/*
 *  ofxOceanContourGenerator.h
 *  PSWaves
 *
 *  Created by Jim on 9/28/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxOceanContour.h"
//#include "ofxFFTLive.h"

typedef struct
{
	string name;
	ofVec2f velocityA;
	ofVec2f velocityB;
	float thicknessA;
	float thicknessB;
	int targetDensity;
	ofFloatColor baseColor;
	ofFloatColor accentColor;
} OceanContourMood;

class ofxOceanContourGenerator {
  public:
	ofVec2f tileSize;
	float step;
	
	void generate();
	void update();
	void draw();
	
	void assignMood(ofxOceanContour* contour);
	
	ofxOcean ocean;
//	ofxFFTBase* fft;
	
	vector<ofxOceanContour*> contours;
	vector<ofPoint> bounds;
	ofVec2f currentVelocity;
	OceanContourMood* currentMood;
	
	vector<float*> fftPeakData;
	int fftDataPointer;
	float fftScale;
	
	bool onA;
	float switchTime;

};