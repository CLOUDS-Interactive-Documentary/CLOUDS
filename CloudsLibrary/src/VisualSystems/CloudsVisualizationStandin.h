
#pragma once

#include "ofMain.h"
#include "CloudsVisualization.h"
#include <set>

typedef struct {
	vector<string> keywords;
	string name;
	string filePath;
} VisualSystemStandIn;

class CloudsVisualizationStandin : public CloudsVisualization {
  public:
	
	virtual void setup();
	virtual void update();
	virtual void draw();
	
	virtual bool isReleventToKeyword(string keyword);

  protected:
	set<string> allKeywords;
	
};