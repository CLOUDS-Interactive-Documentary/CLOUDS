//
//  CloudsCamera.cpp
//  CLOUDS
//
//  Created by James George on 6/14/13.
//
//

#include "CloudsCamera.h"

CloudsCamera::CloudsCamera(){
	sideDistance = 200;
	frontDistance = 100;
	isSetup = false;
}

void CloudsCamera::setup(){
	if(!isSetup){
		isSetup = true;
		ofAddListener(ofEvents().update, this, &CloudsCamera::update);
	}
}

void CloudsCamera::update(ofEventArgs& args){
	float percentOnCurve = ofMap(ofGetMouseX(), ofGetWidth()*.2, ofGetWidth()*.8, 0, 1, true);
	ofVec3f sidePositionLeft = lookTarget + ofVec3f(-sideDistance,0,0);
	ofVec3f sidePositionRight = lookTarget + ofVec3f(sideDistance,0,0);
	ofVec3f frontPosition = lookTarget + ofVec3f(0,0,-frontDistance);
	ofVec3f position;
	if(percentOnCurve > .5){
		position = frontPosition.getInterpolated(sidePositionRight, ofMap(percentOnCurve, .5, 1.0, 0, 1.0) );
	}
	else{
		position = sidePositionLeft.getInterpolated(frontPosition, ofMap(percentOnCurve, 0, .5, 0, 1.0) );
	}
	
	setPosition(position);
	lookAt(lookTarget);
}

