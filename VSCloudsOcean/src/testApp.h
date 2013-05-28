#pragma once

#include "ofMain.h"
#include "ofxOcean.h"
#include "ofxGameCamera.h"
#include "ofxOceanRenderer.h"
//#include "ofxOceanContourGenerator.h"
//#include "ofxOsc.h"

//#include "ofxPSScreen.h"

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

//	void handleOSC();
	
	ofxGameCamera cam;
	
	ofxOcean ocean;
	ofxOceanRenderer renderer;
//	ofxOceanContourGenerator* contours;
	
	float targetChoppy;
	float targetHeight;
	float targetSpeed;
	
//	void saveScreens();
//	void loadScreens();

    int oceanTileSizeX;
    int oceanTileSizeY;
	
	float windSpeed;
	float waveScale;
	float choppyScale;
	float waveSpeed;
		
	bool editingHandles;
	bool editingTextureRatios;
	bool scaleToView;
//	void gaussian_elimination(float *input, int n);
	
//	bool draggingCorner;
//	ofxPSScreen* currentScreen;
//	int currentPointDragIndex;
//	ofVec2f dragOffset;
	
//	bool fencepostSelected;
//	int selectedFencepostIndex;
//	float fencepostSelectOffset;
	
//	float lastSwitchedTime;
//	bool autoMode;
//	bool hideEverything;
	

//	void findHomography(ofVec2f src[4], ofVec2f dst[4], float homography[16]);
//	bool insidePolygon(ofVec2f *polygon, int N, ofVec2f p);
};
