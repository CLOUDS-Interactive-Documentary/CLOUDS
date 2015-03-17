#pragma once

#include "ofMain.h"
#include "CloudsPortal.h"
#ifdef OCULUS_RIFT
#include "ofxOculusRift.h"
#endif

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
		
	ofEasyCam easyCam;
	CloudsPortal portal;
    ofFbo fbo;
#ifdef OCULUS_RIFT
    ofxOculusRift oculusRift;
#endif
};
