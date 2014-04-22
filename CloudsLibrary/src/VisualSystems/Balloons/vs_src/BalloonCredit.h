//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
//
//
//

#pragma once

#include "CloudsVisualSystem.h"
#include "ofxFTGL.h"

class BalloonCredit
{
  public:
//	BalloonCredit();
		
	void setup();
	ofCamera* camera;
	
	float getTextWidth();
	ofVec3f getLeft();
	void update();
	void draw();
	
	string title;
	string name;
	
	ofVec3f pos, left, right;
	float width;
	
	ofxFTGLFont* font;
};
