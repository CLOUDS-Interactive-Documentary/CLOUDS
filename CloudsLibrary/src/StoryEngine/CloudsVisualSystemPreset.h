#pragma once

#include "ofMain.h"

class CloudsVisualSystem;
class CloudsVisualSystemPreset {
  public:
	
	CloudsVisualSystemPreset();
	string presetName;
	string systemName;
	CloudsVisualSystem* system;
	
	float introDuration;
	float outroDuration;
	float duration;
	bool indefinite;
	void loadTimeInfo();
	string getID();
	string comments;
	string grade;
	bool enabled;

	float currentScore;
};