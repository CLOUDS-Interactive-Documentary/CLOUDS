
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
	
	bool historyContainsClip(ClipMarker& m);
	
	float totalSecondsWatched();
	
	int maxTimesOnTopic;
	bool printDecisions;
	
  protected:
	set<string> topicsVisited;
	
	bool hasclip;

	ClipMarker currentClip;
	
	string currentTopic;
	int timesOnTopic; //how many times have we heard about this specific topic
	bool freeTopic; //means the topic is up for grabs on the next traverse
	
	int scoreForClip(ClipMarker& clip);
	void loadClip(ClipMarker& clip);
	void chooseNewTopic(ClipMarker& clip);
};