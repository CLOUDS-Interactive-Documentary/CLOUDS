//
//  CloudsCamera.cpp
//  CLOUDS
//
//  Created by James George on 6/14/13.
//
//

#include "CloudsRGBDCamera.h"

CloudsRGBDCamera::CloudsRGBDCamera(){
	sideDistance = 100;
	frontDistance = 150;
	sidePullback = -50;
	liftAmount = 50;
	liftRange = 25;
	dropAmount = 25;
	isSetup = false;
	damp = .1;
}

void CloudsRGBDCamera::setup(){
	if(!isSetup){
		isSetup = true;
		setPositionFromMouse();
		currentPosition = targetPosition;
		
		ofAddListener(ofEvents().update, this, &CloudsRGBDCamera::update);
	}
}

void CloudsRGBDCamera::update(ofEventArgs& args){
	setPositionFromMouse();
}

void CloudsRGBDCamera::setPositionFromMouse(){
	
	float percentOnCurve = ofMap(ofGetMouseX(), ofGetWidth()*.2, ofGetWidth()*.8, 0, 1, true);
	ofVec3f sidePositionLeft = lookTarget + ofVec3f(-sideDistance,0,sidePullback);
	ofVec3f sidePositionRight = lookTarget + ofVec3f(sideDistance,0,sidePullback);
	ofVec3f frontPosition = lookTarget + ofVec3f(0,0,-frontDistance);
	ofVec3f position;
	if(percentOnCurve > .5){
		position = frontPosition.getInterpolated(sidePositionRight, ofMap(percentOnCurve, .5, 1.0, 0, 1.0) );
	}
	else{
		position = sidePositionLeft.getInterpolated(frontPosition, ofMap(percentOnCurve, 0, .5, 0, 1.0) );
	}
	
	float liftDrift = ofMap(ofGetMouseY(), ofGetHeight()*.2, ofGetHeight()*.8, -liftRange,liftRange, true);
	position.y += ofMap(abs(.5 - percentOnCurve), 0, .5, (liftDrift + liftAmount), (liftDrift-liftAmount)*.5);
	position.z -= MAX(liftDrift,0) * .5; // zoom in on mouse up

	targetPosition = position;
	currentPosition += (targetPosition - currentPosition) * damp;
	
	currentLookTarget = lookTarget - ofVec3f(0,dropAmount,0);
	setPosition(currentPosition);
	lookAt(currentLookTarget);
}
