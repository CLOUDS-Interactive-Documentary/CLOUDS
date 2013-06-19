#pragma once

#include "ofMain.h"

class CloudsCamera : public ofCamera {
  public:
	
	CloudsCamera();

	void setup();
	
	void update(ofEventArgs& args);
	
	float sideDistance;
	float frontDistance;
	float sidePullback;
	ofVec3f lookTarget;
	ofVec3f driftPos;
	
  protected:
	bool isSetup;

};