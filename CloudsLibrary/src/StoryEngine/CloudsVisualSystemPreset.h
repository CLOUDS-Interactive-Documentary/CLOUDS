#pragma once

#include "ofMain.h"

class CloudsVisualSystem;
class CloudsVisualSystemPreset {
  public:
	
	CloudsVisualSystemPreset();
	string presetName;
	string systemName;
	CloudsVisualSystem* system;
	
	string getID();
};