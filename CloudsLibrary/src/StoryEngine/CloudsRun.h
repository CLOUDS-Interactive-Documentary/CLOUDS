
#pragma once

#include "ofMain.h"
#include "CloudsClip.h"
#include "CloudsVisualSystemPreset.h"
#include "CloudsEvents.h"

#include <set>

// the long game
class CloudsRun
{
  public:
	CloudsRun(){
		actCount = 0;
	};

	int actCount;
    
    vector<CloudsClip> clipHistory;
    vector<string> presetHistory;
	vector<string> topicHistory;

    //only for the last run
	map<string, int> accumuluatedTopics;
    
    void clear();
    
    void actBegan(CloudsActEventArgs& args);
	void actEnded(CloudsActEventArgs& args);
	void clipBegan(CloudsClipEventArgs& args);
	void visualSystemBegan(CloudsVisualSystemEventArgs& args);
	void visualSystemEnded(CloudsVisualSystemEventArgs& args);
	void questionProposed(CloudsQuestionEventArgs& args);
	void questionSelected(CloudsQuestionEventArgs& args);
	void topicChanged(CloudsTopicEventArgs& args);
    void preRollRequested(CloudsPreRollEventArgs& args);

	//TODO: acts can be persisted
//	void save();
//	void load();
};