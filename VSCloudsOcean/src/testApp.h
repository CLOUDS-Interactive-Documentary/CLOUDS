#pragma once

#include "ofMain.h"
#include "ofxOcean.h"
#include "ofxGameCamera.h"
#include "ofxOceanRenderer.h"

class testApp : public ofBaseApp{

  public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofxGameCamera cam;

	ofxOcean ocean;
	ofxOceanRenderer renderer;
	
	float targetChoppy;
	float targetHeight;
	float targetSpeed;
	
    int oceanTileSizeX;
    int oceanTileSizeY;
	
	float windSpeed;
	float waveScale;
	float choppyScale;
	float waveSpeed;
		
	bool editingHandles;
	bool editingTextureRatios;
	bool scaleToView;
};
