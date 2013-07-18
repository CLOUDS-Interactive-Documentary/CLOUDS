
#pragma once

#include "ofMain.h"
#include "CloudsClip.h"
#include "CloudsVisualSystemPreset.h"


class CloudsAct;
class CloudsActEventArgs : public ofEventArgs {
  public:
	CloudsActEventArgs(CloudsAct* act){
		this->act = act;
	}
	CloudsAct* act;
};

class CloudsClipEventArgs : public ofEventArgs {
  public:
	CloudsClipEventArgs(CloudsClip& chosenClip, string currentTopic)
		: chosenClip(chosenClip), currentTopic(currentTopic)
	{
		timeUntilNextClip = 0;
	}
	
	CloudsClip& chosenClip;
	vector<CloudsClip> clipOptions;
	string currentTopic;
	float timeUntilNextClip;
};

class CloudsVisualSystemEventArgs : public ofEventArgs {
  public:
	CloudsVisualSystemEventArgs(CloudsVisualSystemPreset& preset)
		: preset(preset)
	{
		duration = 0;
	}

	CloudsVisualSystemPreset& preset;
	
	float duration;
};

class CloudsQuestionEventArgs : public ofEventArgs{
   public:
    CloudsQuestionEventArgs(CloudsClip& questionClip) :questionClip(questionClip)
    {

    }

    CloudsClip& questionClip;
};

class CloudsPreRollEventArgs : public ofEventArgs{
public:
    CloudsPreRollEventArgs(CloudsClip& preRollClip) :preRollClip(preRollClip)
    {
        
    }
    
    CloudsClip& preRollClip;
};

class CloudsEvents {
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

	ofEvent<string> topicChanged;
	
};

