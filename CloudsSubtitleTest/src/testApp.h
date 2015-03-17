#pragma once

#include "ofMain.h"
#include "CloudsFCPParser.h"
#include "CloudsRGBDVideoPlayer.h"
#include "ofxUI.h"
#ifndef CLOUDS_NO_OSC
#include "CloudsSecondaryDisplayOSCSender.h"
#endif

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
	
	CloudsFCPParser parser;
	CloudsRGBDVideoPlayer player;
	vector<CloudsClip*> subtitleClips;
	int currentClip;
	void playCurrentClip();

};
