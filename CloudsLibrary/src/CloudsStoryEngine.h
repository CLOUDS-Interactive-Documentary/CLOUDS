
#pragma once

#include "ofMain.h"
#include "CloudsFCPVisualizer.h"
#include "CloudsFCPParser.h"

class CloudsStoryEngine {
  public:
	CloudsStoryEngine();
	~CloudsStoryEngine();
	
	CloudsFCPVisualizer* visualizer;
	CloudsFCPParser* network;
	vector<ClipMarker> history;
	
	void setup();

	void seedWithClip(ClipMarker& seed, string topic = "");
	bool selectNewClip();
	
	int maxTimesOnTopic;
	bool printDecisions;
	
  protected:
	set<string> topicsVisited;
	
	
	bool hasclip;
	ClipMarker currentClip;
	
	string currentPerson;
	string currentTopic;
	int timesOnTopic;
	
	int scoreForClip(ClipMarker& clip);
	void loadClip(ClipMarker& clip);
	
};