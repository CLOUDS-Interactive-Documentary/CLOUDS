
#pragma once

#include "ofMain.h"
#include "CloudsClip.h"

class CloudsStoryEventArgs : public ofEventArgs {
  public:
	CloudsStoryEventArgs(CloudsClip& chosenClip, vector<CloudsClip>& clipOptions, string currentTopic)
		: chosenClip(chosenClip), clipOptions(clipOptions), currentTopic(currentTopic)
	{
		timeUntilNextClip = 0;

	}
	
	CloudsClip& chosenClip;
	vector<CloudsClip>& clipOptions;
	string currentTopic;
	float timeUntilNextClip;
};

class CloudsEvents {
  public:
	ofEvent<CloudsStoryEventArgs> storyBegan;
	ofEvent<CloudsStoryEventArgs> storyEnded;
	
	ofEvent<CloudsStoryEventArgs> clipBegan;
	ofEvent<CloudsStoryEventArgs> clipEnded;

	ofEvent<CloudsStoryEventArgs> visualSystemBegan;
	ofEvent<CloudsStoryEventArgs> visualSystemEnded;

};

