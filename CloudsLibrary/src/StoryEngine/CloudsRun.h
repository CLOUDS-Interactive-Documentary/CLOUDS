
#pragma once

#include "ofMain.h"
#include "CloudsClip.h"
#include "CloudsVisualSystemPreset.h"

class CloudsRun
{
  public:
	CloudsRun(){
		actCount = 0;
	};
	
	int actCount;
    vector<CloudsClip> clipHistory;
    vector<CloudsVisualSystemPreset> presetHistory;
	
	//TODO: acts can be persisted
//	void save();
//	void load();
};