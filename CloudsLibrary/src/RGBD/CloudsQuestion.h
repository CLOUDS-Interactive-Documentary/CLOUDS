

#pragma once

#include "ofMain.h"
#include "CloudsClip.h"

class CloudsQuestion {
  public:
	
	CloudsQuestion();
		
	ofCamera* cam;
	ofTrueTypeFont* font;
	
	void setup();
	
	void draw();
	void drawOverlay();
	void update();
	
	bool hovering;
	float radius;
	ofVec3f position;
	CloudsClip clip; //must have a question
	
	void mouseMoved(ofMouseEventArgs& args);
	void mousePressed(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	
  protected:
	
	
	bool isSetup;
	ofVec2f currentScreenPoint;
	float screenRadius;
};