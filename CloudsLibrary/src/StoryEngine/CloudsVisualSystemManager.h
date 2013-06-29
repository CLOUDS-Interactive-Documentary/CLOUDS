//
//  CloudsVisualSystemManager.h
//  CLOUDS
//
//  Created by James George on 6/19/13.
//
//

#pragma once

#include "ofMain.h"
#include "CloudsVisualSystemPreset.h"

class CloudsVisualSystemManager {
  public:
	CloudsVisualSystemManager();
	
	void populateVisualSystems();
	CloudsVisualSystemPreset& getRandomVisualSystem();
	
	CloudsVisualSystem* visualSystemWithName(string systemName);
	vector<CloudsVisualSystemPreset>& getPresets();

	vector<string> keywordsForPreset(int index);
	vector<string> keywordsForPreset(CloudsVisualSystemPreset& preset);
	vector<string> keywordsForPreset(string systemName, string presetName);
	
	void setKeywordsForPreset(CloudsVisualSystemPreset& preset, vector<string>& keywords );
	
	void loadPresets();
	void saveKeywords();
    void suppressClip(string presetID, string clipName);
	vector<string>& getSuppressionsForPreset(string presetID);
    bool isClipSuppressed(string presetID,string clip);
  protected:
	
	vector<CloudsVisualSystem*> systems;
	map<string, CloudsVisualSystem*> nameToVisualSystem;
	vector<CloudsVisualSystemPreset> presets;

    map<string, vector<string> > suppressedClips;
	string getKeywordFilePath();
	
	float lastBackupTime;
	float backupTimeInterval;
	map<string,vector<string> > keywords;
	
	//this instantiates and registers all the visual systems, called once at setup
	void registerVisualSystem(CloudsVisualSystem* system);

};