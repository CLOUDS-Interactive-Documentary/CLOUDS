#pragma once

#include "ofMain.h"
#include "CloudsFCPParser.h"
#include "ofxUI.h"

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
    void checkColorDistance();
	void exit();
    
    CloudsFCPParser parser;
    ofVideoPlayer player;
    ofImage img;
    float threshold;
    ofVec2f samplePoint;
    
    float hueWeight;
    float satWeight;
    float brightWeight;
    
    ofxUISuperCanvas *gui;
	void guiEvent(ofxUIEventArgs &e);
};
