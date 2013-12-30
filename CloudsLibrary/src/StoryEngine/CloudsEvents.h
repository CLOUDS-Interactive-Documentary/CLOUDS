
#pragma once

#include "ofMain.h"
#include "CloudsClip.h"
#include "CloudsVisualSystemPreset.h"
#include "CloudsDichotomy.h"


class CloudsAct;
class CloudsActEventArgs : public ofEventArgs {
  public:
	CloudsActEventArgs(CloudsAct* act)
		: act(act){}
	
	CloudsAct* act;
//	CloudsClip startClip;
//	vector<CloudsSoundCue> soundCues;	
//	string startQuestion;
//	string startTopic;
//	string soundQuestionKey;
	
};

class CloudsClipEventArgs : public ofEventArgs {
  public:
	CloudsClipEventArgs(CloudsClip& chosenClip, string currentTopic, vector<CloudsDichotomy>& currentDichotomiesBalance)
		: chosenClip(chosenClip), currentTopic(currentTopic),currentDichotomiesBalance(currentDichotomiesBalance)
	{
		timeUntilNextClip = 0;
	}
	
	CloudsClip& chosenClip;
	vector<CloudsDichotomy>& currentDichotomiesBalance;
	string currentTopic;
	float timeUntilNextClip;
};

class CloudsVisualSystemEventArgs : public ofEventArgs {
  public:
	CloudsVisualSystemEventArgs(CloudsVisualSystemPreset& preset)
		: preset(preset){}

	CloudsVisualSystemPreset& preset;
};

class CloudsQuestionEventArgs : public ofEventArgs{
   public:
    CloudsQuestionEventArgs(CloudsClip& questionClip, string question, string topic)
		: questionClip(questionClip), question(question), topic(topic){}
	
    CloudsClip& questionClip;
	string question;
	string topic;
};

class CloudsPreRollEventArgs : public ofEventArgs{
public:
    CloudsPreRollEventArgs(CloudsClip& preRollClip, float clipStartTimeOffset)
		: preRollClip(preRollClip)
    {
        clipStartTimeOffset = 0;
    }
    
    CloudsClip& preRollClip;
    float handleLength;
};

class CloudsTopicEventArgs : public ofEventArgs{
public:
	CloudsTopicEventArgs(string topic, float duration)
		: topic(topic), duration(duration){}
	
	string topic;
	float duration;
};

class CloudsStoryEvents {
  public:
	 //sent by story engine
    ofEvent<CloudsActEventArgs> actCreated;
	
	//sent by act
    ofEvent<CloudsActEventArgs> actBegan;
    ofEvent<CloudsActEventArgs> actEnded;
	ofEvent<CloudsClipEventArgs> clipBegan;

	ofEvent<CloudsVisualSystemEventArgs> visualSystemBegan;
	ofEvent<CloudsVisualSystemEventArgs> visualSystemEnded;

	ofEvent<CloudsPreRollEventArgs> preRollRequested;
    ofEvent<CloudsQuestionEventArgs> questionAsked;
	ofEvent<CloudsTopicEventArgs> topicChanged;

};


