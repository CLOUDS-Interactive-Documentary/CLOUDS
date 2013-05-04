
#pragma once

#include "ofMain.h"
#include "CloudsClip.h"

class CloudsStoryEventArgs : public ofEventArgs {
  public:
	CloudsStoryEventArgs(CloudsClip& chosenClip, vector<CloudsClip>& clipOptions, string currentTopic)
		: chosenClip(chosenClip), clipOptions(clipOptions), currentTopic(currentTopic)
	{
		//
	}
	
	CloudsClip& chosenClip;
	vector<CloudsClip>& clipOptions;
	string currentTopic;
};

class CloudsEvents {
  public:
	ofEvent<CloudsStoryEventArgs> storyBegan;
	ofEvent<CloudsStoryEventArgs> clipChanged;
	ofEvent<CloudsStoryEventArgs> systemChanged;
};

