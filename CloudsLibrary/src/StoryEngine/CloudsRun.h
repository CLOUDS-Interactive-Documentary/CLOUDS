
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
    map<string, int> timesClipPlayed;
    vector<string> presetHistory;
	vector<string> topicHistory;
	set<string> questionTopicHistory;
	
    map<string, int> timesOnCurrentTopicHistory;
    map<string, int> timesQuestionAsked;
    
    void actBegan(CloudsActEventArgs& args);
	void actEnded(CloudsActEventArgs& args);
	void clipBegan(CloudsClipEventArgs& args);
	void visualSystemBegan(CloudsVisualSystemEventArgs& args);
	void visualSystemEnded(CloudsVisualSystemEventArgs& args);
	void questionAsked(CloudsQuestionEventArgs& args);
	void topicChanged(CloudsTopicEventArgs& args);
    void preRollRequested(CloudsPreRollEventArgs& args);

	//TODO: acts can be persisted
//	void save();
//	void load();
};