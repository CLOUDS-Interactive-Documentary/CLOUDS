//
//  ScrapeCamera.cpp
//  ScrapeVisualSystem
//
//  Created by Elie Zananiri on 2013-07-27.
//
//

#include "ScrapeCamera.h"

#include "ofMath.h"
#include "ofUtils.h"

// when an ScrapeCamera is moving due to momentum, this keeps it
// from moving forever by assuming small values are zero.
float minDifference = 0.1e-5;

// this is the default on windows os
unsigned long doubleclickTime = 200;

//----------------------------------------
ScrapeCamera::ScrapeCamera()
{
	lastTap	= 0;
	lastDistance = 0;
	drag = 0.9f;
	sensitivityRot = 1.0f;//when 1 moving the mouse from one side to the other of the arcball (min(viewport.width, viewport.height)) will rotate 180degrees. when .5, 90 degrees.
	sensitivityXY = .5;
	sensitivityZ= .7;
	
	bDistanceSet = false;
	bDoRotate = false;
	bApplyInertia =false;
	bInsideArcball = true;
	bValidClick = false;
	bAutoDistance = true;
	
	reset();
    ofAddListener(ofEvents().update , this, &ScrapeCamera::update);
}

//----------------------------------------
ScrapeCamera::~ScrapeCamera()
{
    ofRemoveListener(ofEvents().update, this, &ScrapeCamera::update);
}

//----------------------------------------
void ScrapeCamera::update(ofEventArgs & args)
{
    if (!bDistanceSet && bAutoDistance){
        setDistance(getImagePlaneDistance(viewport), true);
    }
    
    rotationFactor = sensitivityRot * 180 / min(viewport.width, viewport.height);
    updateMouse();
    if (bDoRotate) {
        updateRotation();
    }
}

//----------------------------------------
void ScrapeCamera::begin(ofRectangle viewport)
{
	this->viewport = viewport;
	ofCamera::begin(viewport);
}

//----------------------------------------
void ScrapeCamera::reset()
{
	target.resetTransform();
	
	target.setPosition(0,0, 0);
	lookAt(target);
	
	resetTransform();
	setPosition(0, 0, lastDistance);
	   
	xRot = 0;
	yRot = 0;
	zRot = 0;
}

//----------------------------------------
void ScrapeCamera::setTarget(const ofVec3f& targetPoint)
{
	target.setPosition(targetPoint);
	lookAt(target);
}

//----------------------------------------
void ScrapeCamera::setTarget(ofNode& targetNode)
{
	target = targetNode;
	lookAt(target);
}

//----------------------------------------
ofNode& ScrapeCamera::getTarget()
{
	return target;
}

//----------------------------------------
void ScrapeCamera::setDistance(float distance)
{
	setDistance(distance, true);
}

//----------------------------------------
void ScrapeCamera::setDistance(float distance, bool save)  //should this be the distance from the camera to the target?
{
	if (distance > 0.0f) {
		if (save) {
			this->lastDistance = distance;
		}
		setPosition(target.getPosition() + (distance * getZAxis()));
		bDistanceSet = true;
	}
}

//----------------------------------------
float ScrapeCamera::getDistance() const {
	return target.getPosition().distance(getPosition());
}
//----------------------------------------
void ScrapeCamera::setAutoDistance(bool bAutoDistance){
    this->bAutoDistance = bAutoDistance;
    if (bAutoDistance) {
        bDistanceSet = false;
    }
}
//----------------------------------------
void ScrapeCamera::setDrag(float drag){
	this->drag = drag;
}
//----------------------------------------
float ScrapeCamera::getDrag() const {
	return drag;
}

//----------------------------------------
void ScrapeCamera::updateRotation()
{
	if (bApplyInertia) {
		xRot *= drag;
		yRot *= drag;
		zRot *= drag;
		
		if (ABS(xRot) <= minDifference && ABS(yRot) <= minDifference && ABS(zRot) <= minDifference) {
			bApplyInertia = false;
			bDoRotate = false;
		}
	}

	curRot = ofQuaternion(xRot, ofCamera::getXAxis(), yRot, ofCamera::getYAxis(), zRot, ofCamera::getZAxis());
	setPosition((ofCamera::getGlobalPosition()-target.getGlobalPosition())*curRot +target.getGlobalPosition());
	rotate(curRot);
}

//----------------------------------------
void ScrapeCamera::updateMouse()
{
	mouse = ofVec2f(ofGetMouseX(), ofGetMouseY());
    
//    cout << "mouse (" << mouse.x << ", " << mouse.y << ") last (" << lastMouse.x << ", " << lastMouse.y << ") dist " << mouse.distance(lastMouse) << endl;
    
    if (mouse.distance(lastMouse) > minDifference) {
        bDoRotate = true;
        bApplyInertia = false;
        
        if (ofVec2f(mouse.x - viewport.x - (viewport.width/2), mouse.y - viewport.y - (viewport.height/2)).length() < min(viewport.width/2, viewport.height/2)){
            bInsideArcball = true;
        }
        else {
            bInsideArcball = false;
        }
    }
    
    if (bDoRotate) {
        bApplyInertia = true;
        
        mouseVel = mouse  - lastMouse;
        
        xRot = 0;
        yRot = 0;
        zRot = 0;
        if (bInsideArcball) {
            xRot = -mouseVel.y * rotationFactor;
//            cout << "xRot " << xRot << endl;
//            yRot = -mouseVel.x * rotationFactor;
        }
        else {
//            ofVec2f center(viewport.width/2, viewport.height/2);
//            zRot = - ofVec2f(mouse.x - viewport.x - center.x, mouse.y -viewport.y -center.y).angle(lastMouse - ofVec2f(viewport.x, viewport.y) - center);
        }
    }
    
    lastMouse = mouse;
}
