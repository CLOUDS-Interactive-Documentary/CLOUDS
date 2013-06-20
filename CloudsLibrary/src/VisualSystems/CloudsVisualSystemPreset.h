#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"

class CloudsVisualSystemPreset {
  public:
	CloudsVisualSystemPreset();
	string presetName;
	CloudsVisualSystem* system;
	vector<string> keywords;
};