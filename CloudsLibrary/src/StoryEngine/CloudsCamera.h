#pragma once

#include "ofMain.h"

class CloudsCamera : public ofCamera {
  public:
	
	CloudsCamera();

	void setup();
	
	void update(ofEventArgs& args);
	
	float sideDistance;
	float frontDistance;
	ofVec3f lookTarget;
	
  protected:
	bool isSetup;


};