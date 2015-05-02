
#pragma once

#include "ofMain.h"
#include "ofxUI.h"

#include "CloudsEvents.h"
#include "CloudsGlobal.h"
#include "CloudsFCPParser.h"
#include "CloudsRun.h"
#include "CloudsDichotomy.h"

/**
 * The Clouds story engine generates sequences of clips
 * based on the keyword connectivity and rules.
 * It has state history for each story sequence
 */

typedef struct {
	
	CloudsAct* act;
	int run;
	string topic;
	CloudsVisualSystemPreset preset;
	CloudsClip* clip;
	
	vector<string> topicHistory;
	vector<string> presetHistory;
	vector<CloudsClip*> clipHistory;

	bool freeTopic;
	
	float duration;
    int clipNum;
	int topicNum;
	int timesOnCurrentTopic;

	bool visualSystemRunning;
	float visualSystemStartTime;
	float visualSystemEndTime;	
	int moreMenThanWomen;
	
    bool forcingTopic;
    bool forcingPerson;
    
	stringstream log;
	
} CloudsStoryState;

typedef struct {
    
    CloudsRun* run;
    CloudsClip* seed;
    string topic;
    string person;
    bool playSeed;
    bool forceTopic;
    bool forceSpeaker;
    bool allowVisuals;
} CloudsActSettings;

class CloudsAct;
class CloudsVisualSystemManager;
class CloudsStoryEngine {
  public:
	CloudsStoryEngine();
	~CloudsStoryEngine();
	
	CloudsFCPParser* parser;
	CloudsVisualSystemManager* visualSystems;

	void setup();
	
    vector<CloudsClip*> getStartingQuestions();
    
	CloudsAct* buildAct(CloudsRun& run);
	CloudsAct* buildAct(CloudsRun& run, CloudsClip* seed);
	CloudsAct* buildAct(CloudsRun& run, CloudsClip* seed, string topic, bool playSeed = true);
    CloudsAct* buildActWithTopic(CloudsRun& run, string forceTopic);
    CloudsAct* buildActWithPerson(CloudsRun& run, string speakerId);

    CloudsAct* buildAct(CloudsActSettings settings);

	bool getPresetIDForInterlude(CloudsRun& run, CloudsVisualSystemPreset& preset, bool forceCredits = false);
	bool getRandomInterlude(CloudsRun& run, CloudsVisualSystemPreset& preset);

    void initGui();
    void saveGuiSettings();
    void toggleGuis(bool actOnly = false);
	void positionGuis();
	
    //after this many clips the topic opens up again
	bool printDecisions;
	bool atDeadEnd();
	
	//for use in the main clouds repository
	bool combinedClipsOnly;
	
	CloudsStoryEvents& getEvents();

    vector<CloudsDichotomy> getCurrentDichotomyBalance();
    
    //this is a test
    CloudsRun runTest;

	bool screeningQuestionsAdded;
	vector<CloudsClip*> screeningQuestionClips;
	void populateScreeningQuestionsPart1();
	void populateScreeningQuestionsPart2();

 protected:
	
	ofxUISuperCanvas *actGui;
    ofxUISuperCanvas *gui;
    ofxUISuperCanvas *clipGui;
    ofxUISuperCanvas *vsGui;
	ofxUISuperCanvas *topicGui;
	ofxUISuperCanvas *runGui;
	ofxUISuperCanvas *logGui;
	
    void guiEvent(ofxUIEventArgs &e);
	
	CloudsStoryEvents events;
	bool isSetup;
	CloudsAct* customAct;
	
	bool bCreateLog;
	bool bLogTopicDetails;
	bool bLogClipDetails;
	bool bLogVisualSystemDetails;
	
    bool showOnlyStartQuestions;
    vector<CloudsClip*> startingQuestions;
    
	string log;
    vector<string> runTopicCount;
    int dichotomyThreshold;
	int minTimesOnTopic;
	
	string selectTopic(CloudsStoryState& currentState);
	float scoreForTopic(CloudsStoryState& currentState, string potentialNextTopic);
	
    CloudsVisualSystemPreset selectVisualSystem(CloudsStoryState& currentState, bool allowSound);
	float scoreForVisualSystem(CloudsStoryState& currentState, CloudsVisualSystemPreset& potentialNextPreset);

	CloudsClip* selectClip(CloudsStoryState& currentState, vector<CloudsClip*>& questionClips);
    float scoreForClip(CloudsStoryState& currentState, CloudsClip* potentialNextClip, stringstream& cliplog);
    
	bool historyContainsClip(CloudsClip* m, vector<CloudsClip*>& history);
	int occurrencesOfPerson(string person, int stepsBack, vector<CloudsClip*>& history);
	
	void addQuestions(CloudsStoryState& currentState, vector<CloudsClip*>& questionClips);
    void updateDichotomies(CloudsClip* clip);
	void clearDichotomiesBalance();
	vector<CloudsDichotomy> dichotomies;
	
    //Act Builder Parameters
    float actLength;
    float maxTimeWithoutQuestion;
    float gapLengthMultiplier;
    float minClipDurationForStartingOffset;
    float preRollDuration;
	
    //VS Story Engine Parameters
	float visualSystemPrimaryTopicBoost;
	float visualSystemSecondaryTopicBoost;
	
    float longClipThreshold;
    float longClipFadeInPercent;
    float cadenceForTopicChangeMultiplier;
	float clipGapTime;
    float voClipGapTime;
	
	//max time to watch visuals
    float maxVisualSystemRunTime;
    //min time to watch indefinites
    float minVisualSystemRunTime;
	//max time between visuals
    float maxVisualSystemGapTime;

	//how long to extend visual systems over the end of topics
	float visualSystemTopicEndExtend;

	//TIMING PARAMS
	int maxTopicsPerAct;
	int maxTimesOnTopic;
	int maxClipsPerAct;
    
    //Story engine decision making parameters
    float topicsInCommonMultiplier;
    float topicsinCommonWithPreviousMultiplier;
    float samePersonOccurrenceSuppressionFactor;
    float dichotomyWeight;
    float linkFactor;
	float genderBalanceFactor;
    float goldClipFactor;
    float easyClipScoreFactor;
	float seriesBoostFactor;
	bool shouldAddScreeningQuestionsToAct;
	bool shouldGotoCredits;
	float distantClipSuppressionFactor; // no longer using
    
	//Topic selection parameters
	float topicRelevancyMultiplier;
	float lastClipSharesTopicBoost;
	float twoClipsAgoSharesTopicBoost;
	
};
