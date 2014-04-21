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
	
	void getTextWidth();
	ofVec3f getLeft();
	void draw();
	
	string title;
	string name;
	
	ofVec3f pos;
	float width;
	
	ofxFTGLFont* font;
};
