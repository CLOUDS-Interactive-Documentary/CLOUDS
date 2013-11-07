//
//  ScrapeCamera.h
//  CloudsVisualSystemScrape
//
//  Created by Elie Zananiri on 2013-07-27.
//
//

#pragma once

#include "ofCamera.h"
#include "ofEvents.h"

class ScrapeCamera : public ofCamera
{
public:
	ScrapeCamera();
	~ScrapeCamera();
    
	// TODO: this should be ofGetViewRect() eventually
	virtual void begin(ofRectangle viewport = ofGetCurrentViewport());
	void reset();
    
	//----------------------------------------
	// advanced functions
    
	void setTarget(const ofVec3f& target);
	void setTarget(ofNode& target);
	ofNode& getTarget();
    
	void setDistance(float distance);
	float getDistance() const;
    
	// drag is how quickly the camera picks up and slows down
	// it is a normalized value between 0-1
	void setDrag(float drag);
	float getDrag() const;
	
    void setAutoDistance(bool bAutoDistance);
	
private:
	void setDistance(float distance, bool save);
    
	ofNode target;
	
	bool bApplyInertia;
	bool bDoRotate;
	bool bValidClick;
	bool bInsideArcball;
	bool bDistanceSet;
    bool bAutoDistance;
	float lastDistance;
    
	float drag;
	
	float xRot;
	float yRot;
	float zRot;
	
	float sensitivityXY;
	float sensitivityZ;
	float sensitivityRot;
	
	float rotationFactor;
    
	ofVec2f mouse;
	ofVec2f lastMouse;
	ofVec2f mouseVel;
	
	void updateRotation();
	void update(ofEventArgs & args);
	void updateMouse();
	
	unsigned long lastTap;
    
	ofQuaternion curRot;
    
	ofRectangle viewport;// having the  viewport saved localy will make it easier for all the needed maths dealing with viewport.
};
