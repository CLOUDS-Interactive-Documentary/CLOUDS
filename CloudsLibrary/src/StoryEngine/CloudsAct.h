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



class CloudsAct{
  
    public:
    CloudsAct();

    void update(); //you can use this to skip if it's waiting
    void playAct();

    void clearAct();

    CloudsClip& getClipInAct(int index);
    void addClipToAct(CloudsClip clip);
    vector<CloudsClip>& getAllClips();
    
    string getTopicInHistory(int index);
    void setTopicInHistory(string topic);
    vector<string>& getAllTopics();
    
	bool clipEnded(); //call this when the clip is done!
    void drawActDebug();
    
    
    CloudsEvents& getEvents();
    
protected:
    ofxMSATimer timer;
    CloudsClip currentClip;
    string currentTopic;
    CloudsEvents events;
    vector<CloudsClip> clips;
    vector<string> topicHistory;
    
    vector<CloudsVisualSystemPreset> visualSystems;
    vector<string> questions;

    bool waitingForNextClip;
    bool actPlaying;
    float currentTime;
    float clipEndTime;
    float nextClipTime;
    
    int currentPlayIndex;
    void loadNextClip();
    float getActDuration();
};

#endif /* defined(__CloudsStoryEngine__CloudsAct__) */
