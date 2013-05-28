/*
 *  ofxOceanContourGenerator.cpp
 *  PSWaves
 *
 *  Created by Jim on 9/28/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxOceanContourGenerator.h"

bool contourSort(ofxOceanContour* a, ofxOceanContour* b){
	return a->startpos.x > b->startpos.x;
}

void ofxOceanContourGenerator::generate(){
	int numContours = 0;

	/*
	for(int i = 0; i < tileSize; i += step){
		ofxOceanContour* c = new ofxOceanContour();
		c->startpos = ofVec2f(i, 0);
		c->endpos = ofVec2f(i, tileSize);

		assignMood(c);
		c->sampleStep = 4;

		c->ocean = ocean;
		c->contourNumber = numContours++;

		contours.push_back( c );
	}
	*/

	for(int i = 0; i < 500; i++){
		float* newData = new float[512];
		fftPeakData.push_back(newData);
	}
	fftDataPointer = 0;

	/*
	for(int i = 0; i < tileSize; i += step){
		ofxOceanContour* c = new ofxOceanContour();
		c->startpos = ofVec2f(0, i);
		c->endpos = ofVec2f(tileSize, i);

		c->velocity = ofVec2f(0,0);
		c->thickness = .1;
		c->sampleStep = 4;

		c->ocean = ocean;

		contours.push_back( c );
	}
	*/

	onA = true;
	switchTime = ofGetElapsedTimef();

	bounds.push_back(ofPoint(0,0));
	bounds.push_back(ofPoint(tileSize.x,0));
	bounds.push_back(ofPoint(tileSize.x,tileSize.y));
	bounds.push_back(ofPoint(0,tileSize.y));

}

void ofxOceanContourGenerator::update(){
	
	//fft->getFftPeakData(fftPeakData[fftDataPointer], 512);
	
	if(contours.size() > 0){
		contours[fftDataPointer%contours.size()]->fftData = fftPeakData[fftDataPointer];
	}

	for(int i = 0; i < contours.size(); i++){
		contours[i]->update();
		int fftIndex = (fftDataPointer - i + contours.size()) % contours.size();
		contours[i]->fftData = fftPeakData[fftIndex];
		contours[i]->fftScale = fftScale;
	}

	bool needsSort = false;
	fftDataPointer = (fftDataPointer+1)%500;
	for(int i = contours.size()-1; i >= 0; i--){
		ofxOceanContour* contour = contours[i];

		if (!ofInsidePoly(contour->startpos, bounds) && !ofInsidePoly(contour->endpos, bounds)) {
			//chance to die
			if(contours.size() > currentMood->targetDensity && ofRandomuf() > .75){
				contours.erase(contours.begin()+i);
				delete contour;
				break;
			}
			else{
				contour->startpos = contour->startpos - contour->velocity.normalized() * tileSize.x;
				contour->endpos = contour->endpos - contour->velocity.normalized() * tileSize.x;
				if(ofRandomuf() > .5){
					assignMood(contour);
				}
			}
			needsSort = true;
		}
	}

	if(currentMood->targetDensity > contours.size() && ofRandomuf() > .9){
		ofxOceanContour* c = new ofxOceanContour();
		c->startpos = ofVec2f(0, 0);
		c->endpos = ofVec2f(0, tileSize.y);

		assignMood(c);
		c->sampleStep = 7;

		c->ocean = ocean;
		c->contourNumber = contours.size();

		contours.push_back( c );
		needsSort = true;
	}

	if(needsSort){
		sort(contours.begin(), contours.end(), contourSort);
	}
}

void ofxOceanContourGenerator::assignMood(ofxOceanContour* contour){
	if(ofRandomuf()*ofRandomuf() > .3){
		contour->velocity = currentMood->velocityA;
		contour->thickness = currentMood->thicknessA;
	}
	else{
		contour->velocity = currentMood->velocityB;
		contour->thickness = currentMood->thicknessB;
	}
	contour->baseColor = currentMood->baseColor;
	contour->accentColor = currentMood->accentColor;
	contour->fftDataLength = 512;
}

void ofxOceanContourGenerator::draw(){
	for(int i = 0; i < contours.size(); i++){
		contours[i]->draw();
	}
}


