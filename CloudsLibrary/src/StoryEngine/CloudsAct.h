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


typedef enum {
    Clip =0,
    VS,
    Gap,
    Question
}ActItemType;

struct ActTimeItem{
    ActItemType type;
    string key;
    float startTime;
    float endTime;
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
	}
	
    CloudsAct();
	~CloudsAct();
	
    void populateTime();
    void play();
    void clear();

    vector<CloudsClip>& getAllClips();
    vector<CloudsVisualSystemPreset>& getAllVisualSystems();
    vector<string>& getAllTopics();
    
    CloudsClip& getClipInAct(int index);
    CloudsClip& getClipForQuestion(string question);
    CloudsClip& getClipAtTime(float time);
    
    CloudsVisualSystemPreset& getVisualSystemInAct(int index);
    
    void addClipToAct(CloudsClip clip, string topic, float startTime);
    void addGapForVisualSystem(float startTime);
    void addQuestionToAct(CloudsClip clip, float startTime, float duration);
    void addVisualSystem(CloudsVisualSystemPreset preset, float startTime, float duration);

    ActTimeItem& getItemForClip(CloudsClip& clip);
    ActTimeItem& getItemForVisualSystem(CloudsVisualSystemPreset& preset);
    
    void setTopicForClip(string topic, string clipName);
    string getTopicForClip(CloudsClip& clip);

    
    void drawDebug();
    

    bool timeToPlayVisualSystem(); // decide when to play VS based in clips
    CloudsEvents& getEvents();
    
protected:

    ofxTimeline timeline;
    ActItemType itemType;
	
	bool timelinePopulated;
    CloudsClip currentClip;
    string currentTopic;
    CloudsEvents events;
    vector<CloudsClip> clips;
    vector<CloudsVisualSystemPreset> visualSystems;

    vector<ActTimeItem> actItems;
    

    void timelineEventFired(ofxTLBangEventArgs& bang);
    ActTimeItem dummy;
    CloudsClip dummyClip;
    
    map<string, CloudsClip>clipMap;
    map<string, ActTimeItem> clipItems;
    
    map<string, CloudsVisualSystemPreset> visualSystemsMap;
    map<string, ActTimeItem> visualSystemItems;
    map< string, string> topicMap;
    map<string, CloudsClip> questionsMap;
  
    
    float visualSystemDuration;
    
    ofxTLFlags* visualSystemsTrack;
    ofxTLFlags* clipsTrack;
    ofxTLFlags* topicsTrack;
    ofxTLFlags* questionsTrack;
    
    bool waitingForNextClip;
    bool actPlaying;
    float startTime;
    float clipEndTime;
    float nextClipTime;
    
    float visualSystemStartTime;
    float visualSystemEndTime;
    
    float duration;
    bool timeLineActive;
    int currentPlayIndex;
    void loadNextClip();
    float getActDuration();
};

#endif /* defined(__CloudsStoryEngine__CloudsAct__) */
