//
//  CloudsCamera.cpp
//  CLOUDS
//
//  Created by James George on 6/14/13.
//
//

#include "CloudsRGBDCamera.h"
#include "CloudsInput.h"

CloudsRGBDCamera::CloudsRGBDCamera(){
	
	targetNode = NULL;
	startNode = NULL;
	
	sideDistance = 100;
	frontDistance = 147.54;
	sidePullback = -48;
	liftAmount = 20;
	liftRange = 65;
	dropAmount = 33;
	isSetup = false;
	damp = .1;
	driftNoisePosition = 0;
	
	canvasWidth = ofGetWidth();
	canvasHeight = ofGetHeight();

	maxDriftAngle = 0;
//	driftNoiseDensity = 0;
//	driftNoiseSpeed = ;

}

void CloudsRGBDCamera::setup(){
	if(!isSetup){
		isSetup = true;
		setPositionFromMouse();
		currentPosition = targetPosition;
		
		ofAddListener(ofEvents().update, this, &CloudsRGBDCamera::update);
	}
}

void CloudsRGBDCamera::remove(){
	if(isSetup){
		isSetup = false;
		ofRemoveListener(ofEvents().update, this, &CloudsRGBDCamera::update);
	}
}

void CloudsRGBDCamera::update(ofEventArgs& args){
	setPositionFromMouse();
	
	if( targetNode != NULL && startNode != NULL ){
		
		//cout << " transitioning" << endl;
		
		//update transition
		float t = transitionAmount;
		
		ofQuaternion rotQuat;
		rotQuat.slerp( t, startNode->getOrientationQuat(), targetNode->getOrientationQuat() );
		
		setOrientation( ofQuaternion() );
		setPosition( targetNode->getPosition()*t + startNode->getPosition()*(1.-t) );
		setOrientation( rotQuat );
	}
	else{
		setPosition( mouseBasedNode.getPosition() );
		setOrientation( mouseBasedNode.getOrientationQuat() );
	}
}


void CloudsRGBDCamera::jumpToPosition(){
	mouseBasedNode.setPosition(currentPosition);
	mouseBasedNode.lookAt(currentLookTarget);	
}

void CloudsRGBDCamera::setCanvasWidthHeight(float width, float height ){
	canvasWidth = width;
	canvasHeight = height;
}

void CloudsRGBDCamera::setPositionFromMouse(){
	
	float percentOnCurve = ofMap(GetCloudsInputX(), canvasWidth*.2, canvasWidth*.8, 0, 1, true);
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
	
	float liftDrift = ofMap(GetCloudsInputY(), canvasHeight*.2, canvasHeight*.8, -liftRange,liftRange, true);
	position.y += ofMap(abs(.5 - percentOnCurve), 0, .5, (liftDrift + liftAmount), (liftDrift-liftAmount)*.5);
	position.z -= MAX(liftDrift,0) * .5; // zoom in on mouse up

	targetPosition = position;
	currentPosition += (targetPosition - currentPosition) * damp;
	
	currentLookTarget = lookTarget - ofVec3f(0,dropAmount,0);
	
	//calculate drift;
	//ofVec3f driftOffset(0,0,0);
	ofVec3f driftPosition = currentPosition;
	float channelA = 1000;
	float channelB = 1500;
	if(maxDriftAngle > 0){
		ofVec3f toCamera = currentPosition - currentLookTarget;
		ofQuaternion driftQuatX,driftQuatY;
		
		driftNoisePosition += driftNoiseSpeed;// * (ofGetElapsedTimef() - ofGetLastFrameTime());
		float driftX = ofSignedNoise( channelA, driftNoisePosition );
		float driftY = ofSignedNoise( channelB, driftNoisePosition );
		
		driftPosition.rotate(driftX*maxDriftAngle, currentLookTarget, ofVec3f(0,1,0));
		driftPosition.rotate(driftY*maxDriftAngle, currentLookTarget, ofVec3f(1,0,0));
		
//		driftQuatX.makeRotate(maxDriftAngle*driftX, 0, 1, 0);
//		driftQuatY.makeRotate(maxDriftAngle*driftY, 1, 0, 0);
//		driftOffset = driftQuatX * driftQuatY * (ofVec3f(0,0,1) * toCamera.length());
	}
	
	mouseBasedNode.setPosition(driftPosition);
	mouseBasedNode.lookAt(currentLookTarget);
}

void CloudsRGBDCamera::setTransitionStartNode( ofNode* _startNode ){
	cout << "CloudsRGBDCamera::setStartNode: " << endl;
	startNode = _startNode;
}
void CloudsRGBDCamera::setTransitionTargetNode( ofNode* _targetNode ){
	cout << "CloudsRGBDCamera::setTargetNode: " << endl;
	targetNode = _targetNode;
}

void CloudsRGBDCamera::setTransitionPercent( float t ){
	transitionAmount = t;
}
