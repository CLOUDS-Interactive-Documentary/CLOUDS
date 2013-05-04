
#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsStoryEngine.h"
#include "CloudsRGBDCombinedRender.h"

class CloudsPlaybackController {
  public:
	CloudsPlaybackController();
	~CloudsPlaybackController();

	void setup(CloudsStoryEngine& storyEngine);
	
	void update();
	void draw();
	
  protected:
	
	ofVideoPlayer player;
	CloudsStoryEngine* storyEngine;
	CloudsRGBDCombinedRender renderer;
	CloudsClip currentClip;
	
	bool eventsRegistered;
	void storyBegan(CloudsStoryEventArgs& args);
	void clipChanged(CloudsStoryEventArgs& args);

	void playClip(CloudsClip& clip);
	
	string relinkMovieFilepath(string filePath);
	
	bool playingCombinedVideo;
};