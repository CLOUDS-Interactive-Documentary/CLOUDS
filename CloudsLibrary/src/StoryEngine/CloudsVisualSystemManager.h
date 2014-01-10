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
	
	vector<int> getFilteredPresetIndeces(bool enabled, bool oculus, bool gradeA);
	vector<int> getFilteredPresetIndecesForSystem(string systemName,bool enabled, bool oculus, bool gradeA);
	
	ofPtr<CloudsVisualSystem> visualSystemWithName(string systemName);
	vector<CloudsVisualSystemPreset>& getPresets();
	
	bool presetHasKeywords(CloudsVisualSystemPreset& preset);
	vector<string> keywordsForPreset(int index);
	vector<string> keywordsForPreset(CloudsVisualSystemPreset& preset);
	vector<string> keywordsForPreset(string systemName, string presetName);
	vector<CloudsVisualSystemPreset> getPresetsForKeyword(string keyword);
	vector<CloudsVisualSystemPreset> getPresetsForKeywords(vector<string>& keywords, string clipName = "");
	ofPtr<CloudsVisualSystem> getEmptySystem(string mainKeyword, vector<string> keywords);
	
	//resets all presets from file
	void updatePresetsForSystem(ofPtr<CloudsVisualSystem> system);
	void deletePreset(int i);
	
	void setKeywordsForPreset(CloudsVisualSystemPreset& preset, vector<string>& keywords );

	void linkClip(string presetID, string clipName);
	vector<string>& getLinksForPreset(string presetID);
    bool isClipLinked(string presetID,string clip);
    bool isClipLinked(string presetID,string clip, int& index);
    void unlinkClip(string presetID, string clip);
    void unlinkClip(string presetID, int presetIndex);

    void suppressClip(string presetID, string clipName);
	vector<string>& getSuppressionsForPreset(string presetID);
    bool isClipSuppressed(string presetID,string clip);
    bool isClipSuppressed(string presetID,string clip, int& index);
    void unsuppressClip(string presetID, string clip);
    void unsuppressClip(string presetID, int presetIndex);
	
	void loadPresets();
	void savePresets();
	
	
	
	set<string> getAllKeywords();	
    CloudsVisualSystemPreset getRandomEnabledPreset();
  	void exportStandalonePresets();
    
  protected:

	CloudsVisualSystemPreset dummyPreset;
    
	vector< ofPtr<CloudsVisualSystem> > systems;
	map<string, ofPtr<CloudsVisualSystem> > nameToVisualSystem;
	vector<CloudsVisualSystemPreset> presets;
    vector<int> enabledPresetsIndex;
	
	map<string, vector<CloudsVisualSystemPreset> > nameToPresets;
	//preset ID to list of clip link names
	map<string, vector<string> > linkedClips;
    map<string, vector<string> > suppressedClips;
	
	//computed after linkedClips is populated
	void updateClipPresetLinks();
	map<string, vector<string> > clipToPresetLinks;
	
	string getKeywordFilePath();

	void addDefaultPresetForSystem(string systemName);
	
	map<string,vector<string> > keywords;

	bool allSystemsPopulated;
	//this instantiates and registers all the visual systems, called once at setup
	void registerVisualSystem(ofPtr<CloudsVisualSystem> system);
	float lastBackupTime;
	float backupTimeInterval;

};
