
#pragma once

#include "ofMain.h"
#include "CloudsFCPParser.h"

class CloudsVisualizer {
  public:
	void setup();
	
	void draw(float x, float y);
	void playClip(ClipMarker& clip);
    
	ofVideoPlayer preview;
	
  protected:
	
	
};