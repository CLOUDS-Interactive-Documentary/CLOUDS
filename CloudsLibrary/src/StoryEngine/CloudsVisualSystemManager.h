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
#include <set>

class CloudsVisualSystemManager {
  public:
	CloudsVisualSystemManager();
	
	
	static vector< ofPtr<CloudsVisualSystem> > InstantiateSystems(vector<string> systemNames);
	
	void populateVisualSystems();
    void populateEnabledSystemIndeces();
	CloudsVisualSystemPreset getRandomVisualSystem();
	vector<CloudsVisualSystemPreset>& getPresetsForSystem(string systemName);
	CloudsVisualSystemPreset& getPresetWithID(string presetID);
	CloudsVisualSystemPreset& getPresetForSystem(string systemName, string presetName);
	
	CloudsVisualSystem* visualSystemWithName(string systemName);
	vector<CloudsVisualSystemPreset>& getPresets();
	
	bool presetHasKeywords(CloudsVisualSystemPreset& preset);
	vector<string> keywordsForPreset(int index);
	vector<string> keywordsForPreset(CloudsVisualSystemPreset& preset);
	vector<string> keywordsForPreset(string systemName, string presetName);
	vector<CloudsVisualSystemPreset> getPresetsForKeyword(string keyword);
	vector<CloudsVisualSystemPreset> getPresetsForKeywords(vector<string>& keywords);
	
	set<string> getAllKeywords();
	
	void setKeywordsForPreset(CloudsVisualSystemPreset& preset, vector<string>& keywords );
	
	void loadPresets();
	void saveKeywords();
    void suppressClip(string presetID, string clipName);
	vector<string>& getSuppressionsForPreset(string presetID);
    bool isClipSuppressed(string presetID,string clip);
    bool isClipSuppressed(string presetID,string clip, int& index);
    void unsuppressClip(string presetID, string clip);
    void unsuppressClip(string presetID, int presetIndex);
	
    CloudsVisualSystemPreset getRandomEnabledPreset();
  	void exportStandalonePresets();
    
  protected:

	CloudsVisualSystemPreset dummyPreset;
    
	vector<CloudsVisualSystem*> systems;
	map<string, CloudsVisualSystem*> nameToVisualSystem;
	vector<CloudsVisualSystemPreset> presets;
    vector<int> enabledPresetsIndex;
	map<string, vector<CloudsVisualSystemPreset> > nameToPresets;
    map<string, vector<string> > suppressedClips;
	string getKeywordFilePath();

	
	map<string,vector<string> > keywords;


	//this instantiates and registers all the visual systems, called once at setup
	void registerVisualSystem(CloudsVisualSystem* system);
	float lastBackupTime;
	float backupTimeInterval;

};
