#pragma once

#include "ofMain.h"

class CloudsVisualSystem;
class CloudsVisualSystemPreset {
  public:
	CloudsVisualSystemPreset();
	string presetName;
	CloudsVisualSystem* system;
	vector<string> keywords;
	ofVec2f mapPosition;
	float radius;
};