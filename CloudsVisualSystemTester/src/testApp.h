#pragma once

#include "ofMain.h"
#include "CloudsVisualSystemManager.h"

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
	void advanceSystem();
	void shuffleSystemIndices();
	void toLog( const std::string& to_log );
	void logEnd();

	float elapsedTime( float& start_time );

	CloudsVisualSystemManager systemManager;
	CloudsVisualSystem* currentSystem;
	vector<int> presetIndices;
	int currentSystemIndex;
	float lastSystemChangedTime;

	ofBuffer log;
	char logFilename[1024];
	float startTime;
	bool bForward;
	vector<std::pair<string,string>> blackList;

};
