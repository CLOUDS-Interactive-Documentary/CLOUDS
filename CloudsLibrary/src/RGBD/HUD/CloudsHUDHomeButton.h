//
//  CloudsHUDHomeButton.h
//  CloudsHUDDesigner
//
//  Created by James George on 12/14/13.
//
//

#pragma once

#include "ofMain.h"
#include "CloudsInput.h"

class CloudsHUDHomeButton {
  public:
	CloudsHUDHomeButton();
	
	void setup();
	void update();
	
	void draw();
	
	bool isActive();
	
	ofImage sampleImage;
	ofVec2f homeLocation;
  protected:
	
	float maxHoverTime;
	float hoverStartTime;
};