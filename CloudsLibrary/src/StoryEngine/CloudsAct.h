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
#include "ofxMSATimer.h"
#include "CloudsEvents.h"
#include "CloudsFCPParser.h"
#include "CloudsVisualSystemManager.h"
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
    CloudsAct();
    
    
    void populateTime();
    void playAct();

    void clearAct();

    CloudsClip& getClipInAct(int index);
    void addClipToAct(CloudsClip clip, string topic, float startTime);
    vector<CloudsClip>& getAllClips();
    
    void addVisualSystem(CloudsVisualSystemPreset preset, float startTime, float duration);
//    void addVisualSystemAfterClip(CloudsVisualSystemPreset preset, float startTime);
    
    void addGapForVisualSystem(float startTime);
    
    void addQuestionToAct(CloudsClip clip, float startTime, float duration);
    CloudsClip& getClipForQuestion(string question);
    ActTimeItem& getItemForClip(CloudsClip& clip);
    
    //void update(); //you can use this to skip if it's waiting
    
//   string getTopicInHistory(int index);
//   void setTopicInHistory(string topic);
    void setTopicForClip(string topic, string clipName);
    string getTopicForClip(CloudsClip& clip);
    vector<string>& getAllTopics();
    
	bool clipEnded(); //call this when the clip is done!
    void drawActDebug();
    
    bool timeToPlayVisualSystem(); // decide when to play VS based in clips
    CloudsEvents& getEvents();
    
protected:

    ofxTimeline timeline;
    ActItemType itemType;
    ofxMSATimer timer;
    CloudsClip currentClip;
    string currentTopic;
    CloudsEvents events;
    vector<CloudsClip> clips;
    vector<CloudsVisualSystemPreset> visualSystems;
    vector<string> topicHistory;
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
