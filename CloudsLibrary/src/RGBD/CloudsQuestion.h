

#pragma once

#include "ofMain.h"
#include "CloudsClip.h"

class CloudsQuestion {
  public:
	
	CloudsQuestion();
		
	CloudsClip clip; //must have a question
	string topic;
	string question;
	
	ofCamera* cam;
	ofTrueTypeFont* font;
	
	void setup();
	
	void draw();
	void drawOverlay();
	void update();
	
	bool hovering;
	float radius;
	ofVec3f position;
	
	void mouseMoved(ofMouseEventArgs& args);
	void mousePressed(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	
	//intro question variables
	bool introQuestion;
	
  protected:

	bool isSetup;
	ofVec2f currentScreenPoint;
	float screenRadius;
};