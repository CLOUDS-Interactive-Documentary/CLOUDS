
#pragma once

#include "ofMain.h"
#include "CloudsFCPVisualizer.h"
#include "CloudsFCPParser.h"

class CloudsStoryEngine {
  public:
	CloudsStoryEngine();
	~CloudsStoryEngine();
	
	CloudsFCPVisualizer* visualizer;
	
	void setup();

	void selectNewClip();
	
	int maxTimesOnTopic;
	bool printDecisions;
	
  protected:
	
	int scoreForClip(ClipMarker& clip);
	
	set<string> topicsVisited;
	string currentTopic;
	string currentPerson;
	
	
	int timesOnTopic;

};