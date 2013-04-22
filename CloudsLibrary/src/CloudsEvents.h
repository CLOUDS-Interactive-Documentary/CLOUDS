
#pragma once

#include "ofMain.h"
#include "CloudsClip.h"

class CloudsStoryEventArgs : public ofEventArgs {
  public:
	string currentTopic;
	CloudsClip* chosenClip;
	vector<CloudsClip>* clipOptions;
};

class CloudsEvents {
  public:
	ofEvent<CloudsStoryEventArgs> storyBegan;
	ofEvent<CloudsStoryEventArgs> clipChanged;
	ofEvent<CloudsStoryEventArgs> systemChanged;
};

