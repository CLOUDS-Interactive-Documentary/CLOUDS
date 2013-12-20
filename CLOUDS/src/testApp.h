#pragma once

#include "ofMain.h"
#include "CloudsStoryEngine.h"
#include "CloudsFCPParser.h"
#include "CloudsPlaybackController.h"
#include "CloudsSound.h"
#include "CloudsMixer.h"
#include "CloudsWebSocketConnection.h"

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
	
	CloudsFCPParser parser;
	CloudsVisualSystemManager visualSystems;
	CloudsPlaybackController player;
	CloudsStoryEngine storyEngine;
	CloudsMixer mixer;
	CloudsSound sound;
	CloudsRun run; //temp
	CloudsAct* currentAct;
	
	CloudsWebSocketConnection websockets;
	
	void actCreated(CloudsActEventArgs& args);
	
	bool useScratch;
	
	
	void audioRequested(float * output, int bufferSize, int nChannels);
};
