#pragma once

#include "ofMain.h"
typedef struct{
    string line1;
    string line2;
} CloudsVisualSystemCredit;

class CloudsVisualSystem;
class CloudsVisualSystemPreset {
  public:
	
	CloudsVisualSystemPreset();
	string presetName;
	string systemName;
    CloudsVisualSystemCredit credits;
    
	CloudsVisualSystem* system;
	float introDuration;
	float outroDuration;
	float duration;

	bool systemIsRegistered;
	bool indefinite;
	void loadTimeInfo();
	void eraseFiles();
	void checkHasFiles();
    bool hasSound();
    bool isHeavy();
    
	bool hasFiles;
	
	string getID();
	string comments;
	string grade;
    
	bool enabledScreen;
	bool enabledOculus;
    bool bIsHeavy;
    bool enabled();
    
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