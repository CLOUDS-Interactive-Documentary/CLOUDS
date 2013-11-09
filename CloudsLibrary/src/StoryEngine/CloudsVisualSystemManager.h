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
	
	static ofPtr<CloudsVisualSystem> InstantiateSystem(string systemName);
	static vector< ofPtr<CloudsVisualSystem> > InstantiateSystems(vector<CloudsVisualSystemPreset>& systemPresets);
	
	void populateVisualSystems();
    void populateEnabledSystemIndeces();
	CloudsVisualSystemPreset getRandomVisualSystem();
	vector<CloudsVisualSystemPreset>& getPresetsForSystem(string systemName);
	CloudsVisualSystemPreset& getPresetWithID(string presetID);
	bool systemHasPreset(string systemName, string presetName);
	CloudsVisualSystemPreset& getPresetForSystem(string systemName, string presetName);
	
	ofPtr<CloudsVisualSystem> visualSystemWithName(string systemName);
	vector<CloudsVisualSystemPreset>& getPresets();
	
	bool presetHasKeywords(CloudsVisualSystemPreset& preset);
	vector<string> keywordsForPreset(int index);
	vector<string> keywordsForPreset(CloudsVisualSystemPreset& preset);
	vector<string> keywordsForPreset(string systemName, string presetName);
	vector<CloudsVisualSystemPreset> getPresetsForKeyword(string keyword);
	vector<CloudsVisualSystemPreset> getPresetsForKeywords(vector<string>& keywords);
	
	//resets all presets from file
	void updatePresetsForSystem(ofPtr<CloudsVisualSystem> system);
	
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
    
	vector< ofPtr<CloudsVisualSystem> > systems;
	map<string, ofPtr<CloudsVisualSystem> > nameToVisualSystem;
	vector<CloudsVisualSystemPreset> presets;
    vector<int> enabledPresetsIndex;
	map<string, vector<CloudsVisualSystemPreset> > nameToPresets;
    map<string, vector<string> > suppressedClips;
	string getKeywordFilePath();

	
	map<string,vector<string> > keywords;

	bool allSystemsPopulated;
	//this instantiates and registers all the visual systems, called once at setup
	void registerVisualSystem(ofPtr<CloudsVisualSystem> system);
	float lastBackupTime;
	float backupTimeInterval;

};
