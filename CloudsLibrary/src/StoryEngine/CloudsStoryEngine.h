
#pragma once

#include "ofMain.h"
#include "CloudsFCPParser.h"
#include "CloudsEvents.h"


/**
 * The Clouds story engine generates sequences of clips
 * based on the keyword connectivity and rules.
 * It has state history for each story sequence
 */
class CloudsStoryEngine {
  public:
	CloudsStoryEngine();
	~CloudsStoryEngine();
	
	//CloudsFCPVisualizer* visualizer;
	CloudsFCPParser* network;
	
	void setup();

	void seedWithClip(CloudsClip& seed);
	bool selectNewClip();
	
	CloudsClip& getCurrentClip();
	vector<CloudsClip>& getClipHistory();
	string getCurrentTopic();
	float getTotalSecondsWatched();

	bool historyContainsClip(CloudsClip& m);
		
	//after this many times the topic becomes available again
	int topicTimeoutPeriod;
	int getTimesOnTopic();
	
	//after this many clips the topic opens up again
	int maxTimesOnTopic;
	bool printDecisions;
	bool atDeadEnd();
	
	CloudsEvents& getEvents();

  protected:
	
	CloudsEvents events;
	
	bool hasclip;
	CloudsClip currentClip;
	int totalFramesWatched;
	
	float totalPoints;
//	vector< pair<int, CloudsClip> > clipScores;
	int nextClipTopScore;
	vector<CloudsClip> validNextClips;
	vector<CloudsClip> allNextClips;
	bool populateNextClips();
	
	
	vector<CloudsClip> clipHistory;
	vector<string> topicHistory;
	map<string, int> peopleVisited;
	
	string currentTopic;
	int timesOnTopic; //how many times have we heard about this specific topic
	bool freeTopic; //means the topic is up for grabs on the next traverse
	
	int scoreForClip(CloudsClip& clip);
	void loadClip(CloudsClip& clip);
	void chooseNewTopic(CloudsClip& clip);
	int occurrencesOfPerson(string person, int stepsBack);

};