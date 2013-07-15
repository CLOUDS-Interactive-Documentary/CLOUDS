
#pragma once

#include "ofMain.h"
#include "CloudsClip.h"
#include "CloudsVisualSystemPreset.h"

class CloudsStoryEventArgs : public ofEventArgs {
  public:
	CloudsStoryEventArgs(CloudsClip& chosenClip, string currentTopic)
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


class CloudsEvents {
  public:
	ofEvent<CloudsStoryEventArgs> storyBegan;
	ofEvent<CloudsStoryEventArgs> storyEnded;
	
    ofEvent<CloudsStoryEventArgs> actBegan;
    ofEvent<CloudsStoryEventArgs> actEnded;
    
	ofEvent<CloudsStoryEventArgs> clipBegan;
	ofEvent<CloudsStoryEventArgs> clipEnded;

	ofEvent<CloudsVisualSystemEventArgs> visualSystemBegan;
	ofEvent<CloudsVisualSystemEventArgs> visualSystemEnded;
    
    ofEvent<CloudsQuestionEventArgs> askQuestion;

	ofEvent<CloudsStoryEventArgs> topicChanged;
	
};

