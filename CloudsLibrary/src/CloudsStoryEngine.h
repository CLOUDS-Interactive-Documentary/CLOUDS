
#pragma once

#include "ofMain.h"
#include "CloudsFCPVisualizer.h"

class CloudsStoryEngine {
  public:
	CloudsStoryEngine();
	~CloudsStoryEngine();
	
	CloudsFCPVisualizer* visualizer;
	
	void setup();

	void selectNewClip();
	
  protected:
	
	string currentTopic;
	int timesOnTopic;
	
	
};