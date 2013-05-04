#pragma once

#include "ofMain.h"

class CloudsVisualization {
  public:
	CloudsVisualization();
	
	virtual void setup() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;

	virtual bool isReleventToKeyword(string keyword);

  protected:

};