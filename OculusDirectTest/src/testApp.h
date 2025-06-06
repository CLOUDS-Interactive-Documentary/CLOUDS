#pragma once

#include "ofMain.h"
#include "ofxOculusDK2.h"

typedef struct {
	ofVec3f pos;
	ofVec3f screenPos;
	bool lookedAt;
	float distance;
} LookTarget;

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

	bool showOverlay;
	ofEasyCam cam;
	ofxOculusDK2 oculus;
	ofTrueTypeFont font;

	void drawScene();

	void createLookTargets();
	vector<LookTarget> lookTargets;
};
