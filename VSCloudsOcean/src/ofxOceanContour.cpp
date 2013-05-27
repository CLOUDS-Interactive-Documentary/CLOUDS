/*
 *  ofxOceanContour.cpp
 *  PSWaves
 *
 *  Created by Jim on 9/28/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxOceanContour.h"

ofxOceanContour::ofxOceanContour(){
	age = 0;
	velocity = ofVec2f(0,0);

	startpos = ofVec2f(0,0);
	endpos = ofVec2f(0,0);
	thickness = 10;
	sampleStep = 5;

	ocean = NULL;
	fftData = NULL;
	fftOffset = 0;
}

void ofxOceanContour::update(){
	startpos += velocity;
	endpos += velocity;
	age++;
}

void ofxOceanContour::draw(){

	ofVec2f currentPoint = startpos;
	ofVec2f trajectory = (endpos - startpos).normalized();
	ofVec2f leftStep = trajectory.getRotated(90);
	ofVec2f rightStep = trajectory.getRotated(-90);

	int numsteps = (startpos.distance(endpos)/sampleStep);

	vector<ofVec3f> points;
	vector<ofFloatColor> colors;
	for(int i = 0; i < numsteps; i++){
		float fftPush = 0;
		if(fftData != NULL){
			int fftPointA = ofMap(i, 0, numsteps-1, 0, fftDataLength-1);
			int fftPointB = fftPointA+1;
			float fftSamp = ofMap(fftData[fftPointA]*fftData[fftPointA], .25, 1.0, 0, 1.0, true);
			fftPush = ofMap(fftSamp, .0, 1.0, 0, fftScale, false);
		}
		ofVec2f sampleLeft = currentPoint + leftStep*thickness/2.0;
		ofVec2f sampleRight = currentPoint + rightStep*thickness/2.0;

		ofVec3f buoyLeft  = ocean->floatingPosition( sampleLeft.x, sampleLeft.y  );
		ofVec3f buoyRight = ocean->floatingPosition( sampleRight.x, sampleRight.y  );
		buoyLeft.y += fftPush;
		buoyRight.y += fftPush;
		points.push_back( buoyLeft );
		points.push_back( buoyRight );

		ofFloatColor colorLeft = baseColor;
		ofFloatColor colorRight	= baseColor;
		if(contourNumber % 10 == 0){
			colorLeft = accentColor;
			colorRight = accentColor;
		}

		colors.push_back(colorLeft);
		colors.push_back(colorRight);

		currentPoint += trajectory*sampleStep;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, sizeof(ofFloatColor), &(colors[0].r));
	glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), &(points[0].x));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, colors.size());
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);


}
