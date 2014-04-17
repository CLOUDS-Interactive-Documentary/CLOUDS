

#pragma once

#include "ofMain.h"
#include "ofRange.h"

class CalibrationNode{
  public:
	CalibrationNode();
	
	ofVec3f worldPosition;
	ofVec2f screenPosition;
	ofVec2f cursorDirection;
	
	//controlled internally
	float cursorDistance;
	bool hover;
	bool finished;
	float finishedTime;
	float percentComplete;
	float hoverStartTime;
	float gazePercent;
	int multiplier;
	
	ofSoundPlayer* clickSound;
	ofSoundPlayer* selectSound;
	
	//SET ON EACH UPDATE
	ofVec3f baseOffset;
	float titleTypeOffset;
	ofRange activationDistance;
	float holdTime;
	static float nodeActivatedTime;
	
	//SET ON EACH DRAW
	float nodeAlphaAttenuate;
	float nodeBaseSize;
	ofFloatColor tint;
	
	void updatePosition();
	void updateInteraction();
	void draw();
};
