

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
	
//	float secondsToWriteQuestion;
	float charsPerSecond;
	float secondsToConsiderSelected;
	
	void setup();
	
	void draw();
	void drawOverlay();
	void update();
	
	void startHovering();
	void stopHovering();
	bool isSelected();
	
	float radius;
	ofVec3f position;
	
	void mouseMoved(ofMouseEventArgs& args);
	void mousePressed(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);

	ofMesh geometry;
	
	ofVec2f currentScreenPoint;

	//intro question variables
	bool introQuestion;
	
  protected:

	float hoveringStartTime;
	
	bool isSetup;
	bool hovering;
	float screenRadius;
};