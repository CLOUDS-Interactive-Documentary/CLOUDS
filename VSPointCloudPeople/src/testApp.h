#pragma once

#include "ofMain.h"
#include "CloudsVisualSystemRGBD.h"
#include "CloudsTransitionController.h"
#include "CloudsHUDController.h"
#include "CloudsFCPParser.h"

#include "ofxFTGLFont.h"

class testApp : public ofBaseApp{
  public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void exit();
	
	void updateTransitions();
	
	ofxFTGLFont font;
	CloudsHUDController hud;
	CloudsFCPParser parser;

	bool shouldPlayTestVideo;
	CloudsVisualSystemRGBD rgbd;
	CloudsVisualSystem::RGBDTransitionType type;
	CloudsTransitionController transitionController;
};
