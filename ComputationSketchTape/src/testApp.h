#pragma once

#include "ofMain.h"
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
	void exit();
		
	int width, height;
	ofVboMesh mesh;
	vector<int> startIndeces;

	
	void regenerateParticles();
	
	ofEasyCam cam;
	
	ofImage speedTexture;
	ofImage shiftTexture;
	
	void reloadShaders();
	ofShader updateShader;
	ofShader drawShader;

	ofVboMesh offsetMesh;
	ofFbo sourceOffset,targetOffset;
	
	ofFbo fbo;
	
	float pathDeviation;
	
	float speed;
	float scale;
	float pointSize;
	float deviation;
	float colorflip;
	
	bool debug;
	bool regenerate;
	ofxUICanvas* gui;
};
