//
//  CloudsAct.h
//  CloudsStoryEngine
//
//  Created by Surya Mattu on 04/07/13.
//
//

#ifndef __CloudsStoryEngine__CloudsAct__
#define __CloudsStoryEngine__CloudsAct__

#include "ofMain.h"
#include "CloudsEvents.h"

#include "ofxTimeline.h"
#include "CloudsDichotomy.h"


typedef enum {
    Clip =0,
    VS,
    Gap,
    Question,
    PreRoll
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

class CloudsAct{
  public:
	
	template<class ListenerClass>
	void registerEvents(ListenerClass * listener){
		ofAddListener(events.actBegan, listener, &ListenerClass::actBegan);
		ofAddListener(events.actEnded, listener, &ListenerClass::actEnded);
		ofAddListener(events.clipBegan, listener, &ListenerClass::clipBegan);
		ofAddListener(events.visualSystemBegan, listener, &ListenerClass::visualSystemBegan);
		ofAddListener(events.visualSystemEnded, listener, &ListenerClass::visualSystemEnded);
        ofAddListener(events.questionAsked, listener, &ListenerClass::questionAsked);
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
        ofRemoveListener(events.questionAsked, listener, &ListenerClass::questionAsked);
		ofRemoveListener(events.topicChanged, listener, &ListenerClass::topicChanged);
        ofRemoveListener(events.preRollRequested, listener, &ListenerClass::preRollRequested);
	}

    CloudsAct();
	~CloudsAct();
	
    void populateTime();
    void play();
    void clear();
	
    vector<CloudsClip>& getAllClips();
    vector<CloudsVisualSystemPreset>& getAllVisualSystemPresets();
	vector<CloudsVisualSystem*> getAllVisualSystems();
	
    vector<string>& getAllTopics();
    
    CloudsClip& getClip(int index);
//    CloudsClip& getClipForQuestion(string question);
    CloudsClip& getClipAtTime(float time);

    CloudsVisualSystemPreset& getVisualSystemInAct(int index);
    void addClip(CloudsClip clip, string topic, float startTime);
    void addClip(CloudsClip clip, string topic, float startTime, float handleLength,vector<CloudsDichotomy> currentDichotomiesBalance);
    void addGapForCadence(CloudsVisualSystemPreset preset,float startTime, float duration);
    void updateClipStartTime(CloudsClip clip, float startTime, float handleLength,string topic);
    void addQuestion(CloudsClip clip, float startTime);

    void addVisualSystem(CloudsVisualSystemPreset preset, float startTime, float duration);
    void addClipPreRollFlag(float preRollFlagTime, float clipHandleLength, string clipName);

    vector<CloudsDichotomy>& getDichotomiesForClip(string clipName);
    
//    void removeQuestionAtTime(float startTime, float endTime);
    void removeActItem(ActTimeItem item);
    void updateVsEndTime(CloudsVisualSystemPreset preset, float newEndTime);
    ActTimeItem& getItemForClip(CloudsClip& clip);
    ActTimeItem& getItemForVisualSystem(CloudsVisualSystemPreset& preset);
    
    void setTopicForClip(string topic, string clipName);
    string getTopicForClip(CloudsClip& clip);
	
	ofxTimeline& getTimeline(){ return timeline; }
    
    void drawDebug();

    bool timeToPlayVisualSystem(); // decide when to play VS based in clips
    CloudsEvents& getEvents();
    
protected:

    ofxTimeline timeline;
    ofxTLFlags* visualSystemsTrack;
    ofxTLFlags* clipsTrack;
    ofxTLFlags* clipPreRollTrack;
    ofxTLFlags* topicsTrack;
    ofxTLFlags* questionsTrack;
    ofxTLFlags* difficultyTrack;
    ofxTLFlags* vsGapsTrack;
	
    ActItemType itemType;
	
	bool timelinePopulated;
    string currentTopic;
    CloudsEvents events;
    vector<CloudsClip> clips;
    vector<CloudsVisualSystemPreset> visualSystems;

    vector<ActTimeItem> actItems;
    map<string,ActTimeItem> actItemsMap;

    void timelineEventFired(ofxTLBangEventArgs& bang);
	void timelineStopped(ofxTLPlaybackEventArgs& event);
	
    ActTimeItem dummy;
    CloudsClip dummyClip;
    vector<CloudsDichotomy> dummyDichotomies;
    
    map<string, CloudsClip>clipMap;
    map<string, ActTimeItem> clipItems;
    map<string, string> clipDifficultyMap;

	
    map<string, CloudsVisualSystemPreset> visualSystemsMap;
    map<string, ActTimeItem> visualSystemItems;
    map<string, string> topicMap;
	vector<string> topicHistory;
	map<string, float> topicDurationMap;
	
    map<string, CloudsClip> questionsMap;
    map<string, vector<CloudsDichotomy> > dichotomiesMap;
    float visualSystemDuration;

    
    float visualSystemStartTime;
    float visualSystemEndTime;
    
    float duration;
    int currentPlayIndex;
    void loadNextClip();
    float getActDuration();
};

#endif /* defined(__CloudsStoryEngine__CloudsAct__) */
