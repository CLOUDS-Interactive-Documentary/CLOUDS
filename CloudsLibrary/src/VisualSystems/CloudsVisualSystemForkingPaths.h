#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"


class ForkingPath {
  public:
	ForkingPath(){
		merged = false;
		foundTrack = false;
		backset = 0;
		trackIndex = 0;
		track.setMode(OF_PRIMITIVE_LINE_STRIP);

	}
	ofVec2f forkPosition;
	ofVec2f mergePosition;
	ofVec2f currentPos;
	ofVec2f currentDirection;
	
	float backset;
	float targetTrackHeight;
	bool foundTrack;
	bool merged;
	int trackIndex;
	
	ofMesh track;
};

class CloudsVisualSystemForkingPaths : public CloudsVisualSystem {
public:
	
	CloudsVisualSystemForkingPaths();
	
	string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
	void selfPresetLoaded(string presetPath);
	
    void selfDraw();
    void selfExit();
    void selfBegin();
	void selfEnd();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
	
protected:
	
	void addPath();
	bool forkPath(ForkingPath& path, vector<ForkingPath>& paths);
	void generateTracks();
	
	vector<float> tracks;
	vector<bool> usedTracks;
	
	vector<ForkingPath> paths;
	
	int closestUnusedTrack(int trackindex);
	
	bool regenerate;
	float speed;
	float forkPercent;
	float mergePercent;
	float trackDistance;
};