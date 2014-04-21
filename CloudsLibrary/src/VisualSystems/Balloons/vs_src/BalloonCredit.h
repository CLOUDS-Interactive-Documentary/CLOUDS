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

class BalloonCredit
{
  public:
	BalloonCredit(string _title, string _name, ofVec3f _pos);
	void setup();
	
	void getTextWidth();
	ofVec3f getLeft();
	void draw();
	
	string title;
	string name;
	
	ofVec3f pos;
	float width;
};
