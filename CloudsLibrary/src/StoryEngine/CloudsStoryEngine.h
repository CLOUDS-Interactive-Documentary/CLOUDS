
#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsGlobal.h"
#include "CloudsFCPParser.h"
#include "CloudsVisualSystemManager.h"
#include "ofxUI.h"
#include "keywordDichotomy.h"
//typedef struct {
//    string left;
//    string right;
//    int balance; //pos/neg
//} KeywordDichotomy;

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
	
    void initGui();
    void saveGuiSettings();
    void displayGui(bool display);
	
	//after this many clips the topic opens up again
	int maxTimesOnTopic;
	bool printDecisions;
	bool printCriticalDecisions;
	bool atDeadEnd();
	
	//for use in the main clouds repository
	bool combinedClipsOnly;
	
	CloudsEvents& getEvents();
	

//	float fixedClipDelay;
	
    void updateDichotomies(CloudsClip& clip);
	void clearDichotomiesBalance();

    vector<keywordDichotomy> getCurrentDichotomyBalance();

//    CloudsAct& getAct();
    
protected:

    ofxUISuperCanvas *gui;
    ofxUISuperCanvas *clipGui;
    ofxUISuperCanvas *vsGui;
    
    void guiEvent(ofxUIEventArgs &e);
    ofBuffer scoreBuffer;
    stringstream scoreStream;
    stringstream topicScoreStream;
	CloudsEvents events;
	bool isSetup;
	
    float soloPointCloudTime;
    float minLengthToShowPointCloudInClip;
	float totalPoints;
	int nextClipTopScore;
    int dichotomyThreshold;
	vector<keywordDichotomy> dichotomies;
    
	string currentTopic;
	int timesOnTopic; //how many times have we heard about this specific topic
	bool freeTopic; //means the topic is up for grabs on the next traverse
	
	string selectTopic(CloudsAct* act, CloudsClip& clip, vector<string>& topicHistory, string topic, string& log);
	float scoreForClip(vector<CloudsClip>& history, CloudsClip& clip, string topic,string& log, bool visualSystemRunning, bool isPresetIndefinite ); //queue based
	float scoreForTopic(vector<string>& topicHistory, vector<CloudsClip>& history, string currentTopic, string newTopic,string&z log);
		
	bool historyContainsClip(CloudsClip& m, vector<CloudsClip>& history);
	int occurrencesOfPerson(string person, int stepsBack, vector<CloudsClip>& history);
    CloudsVisualSystemPreset getVisualSystemPreset(string keyword);
private:
    //Act Builder Parameters
    float actLength;
    float maxTimeWithoutQuestion;
    float gapLengthMultiplier;
    float minClipDurationForStartingOffset;
    float preRollDuration;
    
    //VS Story Engine Parameters
    float systemMaxRunTime;
    float maxVisualSystemGapTime;
    float longClipThreshold;
    float longClipFadeInPercent;
    
	float getHandleForClip(CloudsClip& clip);
    
    //Story engine decision making parameters
    int topicsInCommonMultiplier;
    int topicsinCommonWithPreviousMultiplier;
    int samePersonOccuranceSuppressionFactor;
    int dichomoiesFactor;
    int linkFactor;
};
