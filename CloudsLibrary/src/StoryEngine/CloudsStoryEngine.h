
#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"

#include "CloudsFCPParser.h"
#include "CloudsVisualSystemManager.h"


typedef struct {
    string left;
    string right;
    int balance; //pos/neg
} KeywordDichotomy;


/**
 * The Clouds story engine generates sequences of clips
 * based on the keyword connectivity and rules.
 * It has state history for each story sequence
 */
class CloudsAct;
class CloudsStoryEngine {
  public:
	CloudsStoryEngine();
	~CloudsStoryEngine();
	
	CloudsFCPParser* parser;
	CloudsVisualSystemManager* visualSystems;
	
	void setup();
	
	CloudsAct* buildAct(CloudsClip& seed);
	CloudsAct* buildAct(CloudsClip& seed, string topic);
	
//	void seedWithClip(CloudsClip& seed);
//	void seedWithClip(CloudsClip& seed, string topic);
//	bool playNextClip(); //you can use this to skip if it's waiting
//	bool clipEnded(); //call this when the clip is done!
	
	//void update(ofEventArgs& args);
	
//	CloudsClip& getCurrentClip();
//	vector<CloudsClip>& getClipHistory();
//	string getCurrentTopic();
//	float getTotalSecondsWatched();

//	bool historyContainsClip(CloudsClip& m);
	//after this many times the topic becomes available again
//	int topicTimeoutPeriod;
	int getTimesOnTopic();
	
	//after this many clips the topic opens up again
	int maxTimesOnTopic;
	bool printDecisions;
	bool printCriticalDecisions;
	bool atDeadEnd();
	
	//for use in the main clouds repository
	bool combinedClipsOnly;
	
//	bool isWaiting();
	
	CloudsEvents& getEvents();
	
	//TODO: make dynamic, will be improved
	float fixedClipDelay;
	
//	void drawStoryEngineDebug();
//	void drawActDebug();
    void updateDichotomies(CloudsClip& clip);
	void clearDichotomiesBalance();
    
//    CloudsAct& getAct();
	
	//TODO: these should be private once the gui is internal
    //VS Story Engine Parameters
    float systemMaxRunTime;
    float maxVisualSystemGapTime;
    float longClipThreshold;
    float longClipFadeInPercent;
    float actLength;
    
    int topicsInCommonMultiplier;
    int topicsinCommonWithPreviousMultiplier;
    int samePersonOccuranceSuppressionFactor;
    int dichomoiesFactor;
    int linkFactor;
protected:
	
    ofBuffer scoreBuffer;
    stringstream scoreStream;
	CloudsEvents events;
	bool isSetup;
	
//	bool hasclip;
//	CloudsClip currentClip;
//	int totalFramesWatched;
//	CloudsAct act;

    float soloPointCloudTime;
    float minLengthToShowPointCloudInClip;
	float totalPoints;
	int nextClipTopScore;
    int dichotomyThreshold;
	
//	vector<CloudsClip> validNextClips;
//	vector<CloudsClip> allNextClips;
	

	
//	bool populateNextClips();
//	void checkVisualSystems();
	
//	float getNextClipDelay();
//	bool waitingForNextClip;
//	float nextClipTime;
	
//	bool watchingVisualSystem;
//	float visualSystemEndTime;
//	CloudsVisualSystemPreset currentVisualSystem;
	
//	vector<CloudsClip> clipHistory;
//	vector<CloudsClip> clipQueue;
	vector<KeywordDichotomy> dichotomies;
    
//	vector<string> topicHistory;
//	map<string, int> peopleVisited;
	
	string currentTopic;
	int timesOnTopic; //how many times have we heard about this specific topic
	bool freeTopic; //means the topic is up for grabs on the next traverse
	
//	float scoreForClip(CloudsClip& clip);
	string selectTopic(CloudsAct* act, CloudsClip& clip, vector<string>& topicHistory, string topic);
	float scoreForClip(vector<CloudsClip>& history, CloudsClip& clip, string topic,string& log); //queue based
	float scoreForTopic(vector<string>& topicHistory, vector<CloudsClip>& history, string currentTopic, string newTopic);
	
	
//	void loadClip(CloudsClip& clip);
//	void chooseNewTopic(CloudsClip& clip);
	
	bool historyContainsClip(CloudsClip& m, vector<CloudsClip>& history);
//	int occurrencesOfPerson(string person, int stepsBack);
	int occurrencesOfPerson(string person, int stepsBack, vector<CloudsClip>& history);
};
