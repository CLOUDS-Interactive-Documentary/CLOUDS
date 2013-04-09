#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxGameCamera.h"
#include "ofxUI.h"

#include "State.h"
#include "ForceSet.h"
#include "Points.h"
#include "GroundPlane.h"

using namespace Particles;

class testApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	void drawFromAbove();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void reload();
	
	void drawGrid();
	
	ofxGameCamera cam;

	ofShader null;
	State state;
	ForceSet forceSet;
	ofShader gravity;
	Points points;
	
	ofFloatImage random;
	
	ofFbo fromAbove;
	ofFbo multiSample;
	
	ofVec3f birthPlace;
	float birthSpread;
	float birthRate;
	
	bool updating;
	bool debug;
	
	ofxUICanvas* gui;
	ofxOscReceiver rx;
};
