#pragma once

#include "ofMain.h"

class CloudsRGBDCamera : public ofCamera {
  public:
	
	CloudsRGBDCamera();

	void setup();
	
	void update(ofEventArgs& args);
	
	//position/movement params
	float sideDistance;
	float frontDistance;
	float sidePullback;
	float dropAmount;
	float liftAmount;
	//y params range --
	float liftRange;

	
	//looking and dampening params
	ofVec3f lookTarget;
	ofVec3f targetPosition;
	float damp;
	
	//perlin drift params
	ofVec3f driftPos;
	float driftRadius;
	
	//up vector drift
	
  protected:
	ofVec3f currentPosition;
	ofVec3f currentLookTarget;
	
	void setPositionFromMouse();
	
	bool isSetup;
	bool isHovering;
};