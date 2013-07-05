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
#include "CloudsFCPParser.h"
#include "CloudsVisualSystemManager.h"
#include "CloudsEvents.h"

class CloudsAct{
  
    public:
    CloudsAct();
    vector<CloudsClip> clips;
    vector<CloudsVisualSystemPreset> visualSystems;
    vector<string> questions;
    
    
    void update(); //you can use this to skip if it's waiting
    void playAct();

    
	bool clipEnded(); //call this when the clip is done!
    void drawActDebug();

    
    CloudsEvents& getEvents();
    
protected:
    CloudsClip currentClip;
    CloudsEvents events;
    
    bool actPlaying;
    float currentTime;
    float clipEndTime;

    int currentPlayIndex;
    void playNextClip();
    float getActDuration();
};

#endif /* defined(__CloudsStoryEngine__CloudsAct__) */
