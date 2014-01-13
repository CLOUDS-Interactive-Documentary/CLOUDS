//
//  CloudsAct.h
//  CloudsStoryEngine
//
//  Created by Surya Mattu on 04/07/13.
//
//

#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"

#include "ofxTimeline.h"
#include "CloudsDichotomy.h"


typedef enum {
    Clip =0,
    VS,
//    Gap,
    Question,
    PreRoll,
	Note
}ActItemType;

struct ActTimeItem{
    ActItemType type;
    string key;
    float startTime;
    float endTime;
    float introEndTime;
    float outroStartTime;
    float handleLength;
};

struct CloudsSoundCue
{
	float startTime;
	float duration;
	int mixLevel; //0, 1, 2;
	string riggedPresetName;
	string soundQuestionKey;
	vector<CloudsDichotomy> dichotomies;
};

class CloudsAct{
  public:
	
	template<class ListenerClass>
	void registerEvents(ListenerClass * listener){
		ofAddListener(events.actBegan, listener, &ListenerClass::actBegan);
		ofAddListener(events.actEnded, listener, &ListenerClass::actEnded);
		ofAddListener(events.clipBegan, listener, &ListenerClass::clipBegan);
		ofAddListener(events.visualSystemBegan, listener, &ListenerClass::visualSystemBegan);
		ofAddListener(events.visualSystemEnded, listener, &ListenerClass::visualSystemEnded);
        ofAddListener(events.questionProposed, listener, &ListenerClass::questionProposed);
		ofAddListener(events.topicChanged, listener, &ListenerClass::topicChanged);
        ofAddListener(events.preRollRequested, listener, &ListenerClass::preRollRequested);
	}
	
	template<class ListenerClass>
	void unregisterEvents(ListenerClass * listener){
		ofRemoveListener(events.actBegan, listener, &ListenerClass::actBegan);
		ofRemoveListener(events.actEnded, listener, &ListenerClass::actEnded);
		ofRemoveListener(events.clipBegan, listener, &ListenerClass::clipBegan);
		ofRemoveListener(events.visualSystemBegan, listener, &ListenerClass::visualSystemBegan);
		ofRemoveListener(events.visualSystemEnded, listener, &ListenerClass::visualSystemEnded);
        ofRemoveListener(events.questionProposed, listener, &ListenerClass::questionProposed);
		ofRemoveListener(events.topicChanged, listener, &ListenerClass::topicChanged);
        ofRemoveListener(events.preRollRequested, listener, &ListenerClass::preRollRequested);
	}

    CloudsAct();
	~CloudsAct();
	
    void populateTime();
    void play();
    void clear();
	
	bool isClipEnergyShift(CloudsClip& clip);
	
    bool startsWithVisualSystem();
    
    vector<CloudsClip>& getAllClips();
    vector<CloudsVisualSystemPreset>& getAllVisualSystemPresets();
	vector< ofPtr<CloudsVisualSystem> > getAllVisualSystems();
	vector<CloudsDichotomy>& getDichotomiesForClip(CloudsClip& clip);
	vector<CloudsDichotomy>& getDichotomiesForClip(string clipName);
	vector<CloudsSoundCue>& getSoundCues();
	
    vector<string>& getAllTopics();
    
    CloudsClip& getClip(int index);
    CloudsClip& getClipAtTime(float time);

    CloudsVisualSystemPreset& getVisualSystemInAct(int index);
    float addClip(CloudsClip& clip, string topic, float startTime);
    float addClip(CloudsClip& clip, string topic, float startTime, vector<CloudsDichotomy> currentDichotomiesBalance);
    float addVisualSystem(CloudsVisualSystemPreset& preset, float startTime, float endTime);
    void addQuestion(CloudsClip& clip, string topic, float startTime);
	void addNote(string note, float time);
    void addSilenceRange(ofRange range);
    
    ActTimeItem& getItemForClip(CloudsClip& clip);
    ActTimeItem& getItemForVisualSystem(CloudsVisualSystemPreset& preset);
    float getClipStartTime(CloudsClip& clip);
	
    void setTopicForClip(string topic, string clipName);
    string getTopicForClip(CloudsClip& clip);
	
	ofxTimeline& getTimeline(){ return timeline; }
    
    void drawDebug();
	//set via storyengine param
	float defaulPrerollDuration;
	
    bool timeToPlayVisualSystem(); // decide when to play VS based in clips
    CloudsStoryEvents& getEvents();
    
  protected:

    ofxTimeline timeline;
    ofxTLFlags* visualSystemsTrack;
    ofxTLFlags* clipsTrack;
    ofxTLFlags* clipPreRollTrack;
    ofxTLFlags* topicsTrack;
    ofxTLFlags* questionsTrack;
    ofxTLFlags* difficultyTrack;
    ofxTLFlags* silenceTrack;
    ofxTLFlags* notesTrack;
	
    ActItemType itemType;
    
	
	bool timelinePopulated;
    string currentTopic;
    CloudsStoryEvents events;
    vector<CloudsClip> clips;
    vector<CloudsVisualSystemPreset> visualSystems;
	
    vector<ActTimeItem> actItems;
    map<string,ActTimeItem> actItemsMap;

    void timelineEventFired(ofxTLBangEventArgs& bang);
	void timelineStopped(ofxTLPlaybackEventArgs& event);
	
    ActTimeItem dummy;
    CloudsClip dummyClip;
    vector<CloudsDichotomy> dummyDichotomies;
    vector<string> energyShiftClipIDs;
	
    map<string, CloudsClip> clipMap;
    map<string, ActTimeItem> clipItems;
    map<string, string> clipDifficultyMap;

    map<string, int> visualSystemIndeces;
    map<string, ActTimeItem> visualSystemItems;
    map<string, string> topicMap;
	vector<string> topicHistory;
	map<string, float> topicDurationMap;
	
    map<string, CloudsClip> questionsMap;
    map<string, vector<CloudsDichotomy> > dichotomiesMap;
	ofxTLFlags* dichotomyClips;
	map<string, ofxTLCurves*> dichotomyTracks;

	vector< pair<string,float> > notes;
	
    float defaultAudioFade;
    vector<ofRange> silenceCues;
	vector<CloudsSoundCue> cues;
    
    float duration;
    int currentPlayIndex;
    void loadNextClip();
    float getActDuration();
};


