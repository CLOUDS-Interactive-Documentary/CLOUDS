//
//  CloudsCamera.h
//  CLOUDS
//
//  Created by James George on 6/14/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofRange.h"

class CloudsRGBDCamera : public ofCamera {
  public:
	
	CloudsRGBDCamera();

	void setup();
	void remove(); //removes events
	void update(ofEventArgs& args);
	
	//position/movement params
	float sideDistance;
	float frontDistance;
	float sidePullback;
	float dropAmount;
	float liftAmount;
	//y params range --
	float liftRange;
	
	//updated every frame with canvas dimensions
	void setCanvasWidthHeight(float width, float height );

	//looking and dampening params
	ofVec3f lookTarget;
	ofVec3f targetPosition;
	float damp;
	
	//perlin drift params
	ofVec3f driftPos;
	float driftRadius;
	
	void setTransitionStartNode( ofNode* _startNode );
	void setTransitionTargetNode( ofNode* _targetNode );
	void setTransitionPercent( float t );
	void setTransitionRotationPercent(float t);
	
	
	void jumpToPosition();
	
	//up vector drift
	ofVec3f startPos;
	ofQuaternion startQuat;
	
	ofNode* startNode;
	ofNode* targetNode;
	ofNode mouseBasedNode;
	
	//camera drift
	ofVec3f driftTarget;
	float maxDriftAngle;
	float driftNoiseDensity;
	float driftNoiseSpeed;
	float driftNoisePosition;
	

	ofRange zoomFOVRange;
	
  protected:
	ofVec3f currentPosition;
	ofVec3f currentLookTarget;
	ofNode startTransform;
	float canvasWidth;
	float canvasHeight;
	
	void setPositionFromMouse();
	
	bool isSetup;
	bool isHovering;
	
	float transitionAmount;
	float transitionRotAmount;
	bool transitioning;
	
	
};