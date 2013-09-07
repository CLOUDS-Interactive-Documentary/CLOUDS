
#pragma once

#include "ofMain.h"
#include "ofxUI.h"

#include "CloudsEvents.h"
#include "CloudsGlobal.h"
#include "CloudsFCPParser.h"
#include "CloudsVisualSystemManager.h"
#include "CloudsRun.h"
#include "CloudsDichotomy.h"

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
	
	CloudsAct* buildAct(CloudsRun& run, CloudsClip& seed);
	CloudsAct* buildAct(CloudsRun& run, CloudsClip& seed, string topic);
	
    void initGui();
    void saveGuiSettings();
    void toggleGuis();
	void positionGuis();
	
	//after this many clips the topic opens up again
	float maxTimesOnTopic;
	bool printDecisions;
	bool printCriticalDecisions;
	bool atDeadEnd();
	
	//for use in the main clouds repository
	bool combinedClipsOnly;
	
	CloudsEvents& getEvents();
	
    void updateDichotomies(CloudsClip& clip);
	void clearDichotomiesBalance();

    vector<CloudsDichotomy> getCurrentDichotomyBalance();

 protected:
	
	ofxUISuperCanvas *actGui;
    ofxUISuperCanvas *gui;
    ofxUISuperCanvas *clipGui;
    ofxUISuperCanvas *vsGui;
	ofxUISuperCanvas *topicGui;
	
    void guiEvent(ofxUIEventArgs &e);

	CloudsEvents events;
	bool isSetup;

	vector<CloudsDichotomy> dichotomies;
    int dichotomyThreshold;
    
//	int timesOnTopic; //how many times have we heard about this specific topic
//	bool freeTopic; //means the topic is up for grabs on the next traverse
	
	string selectTopic(CloudsAct* act, CloudsClip& clip, vector<string>& topicHistory, string topic, string& log);
	float scoreForClip(vector<CloudsClip>& history, CloudsClip& clip, string topic,string& log, bool visualSystemRunning, bool isPresetIndefinite ); //queue based
	float scoreForTopic(vector<string>& topicHistory, vector<CloudsClip>& history, string currentTopic, string newTopic, string& log);
	float scoreForVisualSystem(CloudsVisualSystemPreset& preset, vector<string>& presetHistory, string currentTopic, vector<string>& seconardyTopics, string& log);
	
	bool historyContainsClip(CloudsClip& m, vector<CloudsClip>& history);
	int occurrencesOfPerson(string person, int stepsBack, vector<CloudsClip>& history);
    CloudsVisualSystemPreset getVisualSystemPreset(string keyword, CloudsClip& currentClip, vector<string>& presetHistory, string& log);
	
    //Act Builder Parameters
    float actLength;
    float maxTimeWithoutQuestion;
    float gapLengthMultiplier;
    float minClipDurationForStartingOffset;
    float preRollDuration;
	
    //VS Story Engine Parameters
	float visualSystemPrimaryTopicBoost;
	float visualSystemSecondaryTopicBoost;
    float systemMaxRunTime;
    float maxVisualSystemGapTime;
    float longClipThreshold;
    float longClipFadeInPercent;

	
	float getHandleForClip(CloudsClip& clip);
    
    //Story engine decision making parameters
    float topicsInCommonMultiplier;
    float topicsinCommonWithPreviousMultiplier;
    float samePersonOccurrenceSuppressionFactor;
    float dichotomyWeight;
    float linkFactor;
	
	//Topic selection parameters
	float topicRelevancyMultiplier;
	float lastClipSharesTopicBoost;
	float twoClipsAgoSharesTopicBoost;
	
};
