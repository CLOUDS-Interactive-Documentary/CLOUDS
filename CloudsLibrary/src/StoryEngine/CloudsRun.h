
#pragma once

#include "ofMain.h"
#include "CloudsClip.h"
#include "CloudsVisualSystemPreset.h"
#include "CloudsEvents.h"

#include <set>

// the long game
class CloudsFCPParser;
class CloudsRun
{
  public:
	CloudsRun(){
		actCount = 0;
		questionsAsked = 0;
	};


	bool load(CloudsFCPParser* parser);
	void save();
	void saveAs(string filePath);
    void archive();
    
	int actCount;
    int questionsAsked;
    vector<CloudsClip*> clipHistory;
    vector<string> presetHistory;
	vector<string> topicHistory;

    set<string> getVisitedTopics();
    set<string> getVisitedPeople();
    set<string> getVisitedVisuals();
    
    bool historyContainsClip(CloudsClip* clip);
    
    //only for the last act
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

};