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
    Gap
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
    
    void update(); //you can use this to skip if it's waiting
    void playAct();

    void clearAct();

    CloudsClip& getClipInAct(int index);
    void addClipToAct(CloudsClip clip, float startTime);
    vector<CloudsClip>& getAllClips();
    
    void addVisualSystemDuringClip(CloudsVisualSystemPreset preset, float startTime, float clipDuration);
    void addVisualSystemAfterClip(CloudsVisualSystemPreset preset, float startTime);
    
    void addGapForVisualSystem(float startTime);
    
    string getTopicInHistory(int index);
    void setTopicInHistory(string topic);
    vector<string>& getAllTopics();
    
	bool clipEnded(); //call this when the clip is done!
    void drawActDebug();
    
    bool timeToPlayVisualSystem(); // decide when to play VS based in clips
    CloudsEvents& getEvents();
    
protected:

    ofxTimeline timeLine;
    ActItemType itemType;
    ofxMSATimer timer;
    CloudsClip currentClip;
    string currentTopic;
    CloudsEvents events;
    vector<CloudsClip> clips;
    vector<CloudsVisualSystemPreset> visualSystems;
    vector<string> topicHistory;
    vector<ActTimeItem> actItems;
    
    map<string,CloudsClip>clipMap;
    map<string,CloudsVisualSystemPreset>visualSystemsMap;


    vector<string> questions;
  
    void populateTime();
    
    float visualSystemDuration;
    
    bool waitingForNextClip;
    bool actPlaying;
    float startTime;
    float clipEndTime;
    float nextClipTime;
    
    float visualSystemStartTime;
    float visualSystemEndTime;
   
    


    int currentPlayIndex;
    void loadNextClip();
    float getActDuration();
};

#endif /* defined(__CloudsStoryEngine__CloudsAct__) */
