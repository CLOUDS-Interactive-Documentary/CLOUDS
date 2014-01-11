#pragma once

#include "ofMain.h"

class CloudsVisualSystem;
class CloudsVisualSystemPreset {
  public:
	
	CloudsVisualSystemPreset();
	string presetName;
	string systemName;
	ofPtr<CloudsVisualSystem> system;
	
	float introDuration;
	float outroDuration;
	float duration;

	bool systemIsRegistered;
	bool indefinite;
	void loadTimeInfo();
	void eraseFiles();
	void checkHasFiles();
    bool hasSound();
    
	bool hasFiles;
	
	string getID();
	string comments;
	string grade;
	bool enabled;
	bool oculusCompatible;

    bool soundExcludeVO;
    bool soundAllowVO;
	bool interlude;
    
	bool defaultedToFamily;
	string conjureKeyword;
	vector<string> allKeywords; // only filled out when the preset is scheduled
	bool randomlySelected;
	bool missingContent;
	
	bool stillPresent;
	float currentScore;
};