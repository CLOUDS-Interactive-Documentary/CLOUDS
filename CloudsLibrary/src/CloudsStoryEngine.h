
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
	
	void setup();

	void seedWithClip(ClipMarker& seed);
	bool selectNewClip();
	
	ClipMarker& getCurrentClip();
	vector<ClipMarker>& getClipHistory();
	string getCurrentTopic();
	float getTotalSecondsWatched();

	bool historyContainsClip(ClipMarker& m);
		
	//after this many times the topic becomes available again
	int topicTimeoutPeriod;
	int getTimesOnTopic();
	
	//after this many clips the topic opens up again
	int maxTimesOnTopic;
	bool printDecisions;
	
  protected:
	bool hasclip;
	ClipMarker currentClip;
	int totalFramesWatched;
	
	vector<ClipMarker> clipHistory;
	list<string> topicHistory;
	map<string, int> peopleVisited;
	
	string currentTopic;
	int timesOnTopic; //how many times have we heard about this specific topic
	bool freeTopic; //means the topic is up for grabs on the next traverse
	
	int scoreForClip(ClipMarker& clip);
	void loadClip(ClipMarker& clip);
	void chooseNewTopic(ClipMarker& clip);
	int occurrencesOfPerson(string person, int stepsBack);

};