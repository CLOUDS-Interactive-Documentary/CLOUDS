#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "CloudsHUDController.h"
#include "CloudsFCPParser.h"
#include "CloudsVisualSystemEmpty.h"


class testApp : public ofBaseApp{
  public:
	void setup();
	void update();
	void draw();
    void exit();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
    CloudsVisualSystemEmpty empty;
 
	CloudsHUDController hud;
	CloudsFCPParser parser;
    void guiEvent(ofxUIEventArgs &e);
};
