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
	
    //Visual Systems Factory
    static bool HasSystemRegistered(CloudsVisualSystem* system);
	static CloudsVisualSystem* InstantiateSystem(const string& systemName);
	static vector< CloudsVisualSystem* > InstantiateSystems(vector<CloudsVisualSystemPreset>& systemPresets);
    static void DeallocateSystems();
    ///VSF
    
	void populateVisualSystems();
    void populateEnabledSystemIndeces();
	CloudsVisualSystemPreset getRandomVisualSystem();
	CloudsVisualSystemPreset& getPresetWithID(const string& presetID);
	bool systemHasPreset(const string& systemName, const string& presetName);
    bool hasPresetWithID(const string& ID);
    bool hasPresetWithID(const string& ID, int& index);
	CloudsVisualSystemPreset& getPresetForSystem(const string& systemName, const string& presetName);
	
	vector<int> getFilteredPresetIndeces(bool enabled, bool oculus, bool gradeA, string systemName = "");
//	vector<int> getFilteredPresetIndecesForSystem(,bool enabled, bool oculus, bool gradeA);
		
	vector<CloudsVisualSystemPreset>& getPresets();
	
	bool presetHasKeywords(CloudsVisualSystemPreset& preset);
	vector<string> keywordsForPreset(int index);
	vector<string> keywordsForPreset(CloudsVisualSystemPreset& preset);
	vector<string> keywordsForPreset(const string& systemName, const string& presetName);
	vector<CloudsVisualSystemPreset> getPresetsForKeyword(const string& keyword);

	vector<CloudsVisualSystemPreset> getPresetsForKeywords(vector<string>& keywords, string clipName = "", bool isInterlude = false);

	CloudsVisualSystem* getEmptySystem(const string& mainKeyword, vector<string> keywords);
	
    void pastePresetToIndex(CloudsVisualSystemPreset preset, int index);
    
	//resets all presets from file
	void updatePresetsForSystem(CloudsVisualSystem* system);
	void deletePreset(int i);
	
	void setKeywordsForPreset(CloudsVisualSystemPreset& preset, vector<string>& keywords );

	void linkClip(const string& presetID, const string& clipName);
	vector<string>& getLinksForPreset(const string& presetID);
    bool isClipLinked(const string& presetID,const string& clip);
    bool isClipLinked(const string& presetID,const string& clip, int& index);
    void unlinkClip(const string& presetID, string clip);
    void unlinkClip(const string& presetID, int presetIndex);

    void suppressClip(const string& presetID, const string& clipName);
	vector<string>& getSuppressionsForPreset(const string& presetID);
    bool isClipSuppressed(const string& presetID,const string& clip);
    bool isClipSuppressed(const string& presetID,const string& clip, int& index);
    void unsuppressClip(const string& presetID, const string& clip);
    void unsuppressClip(const string& presetID, int presetIndex);
	
	void loadPresets();
	void savePresets();
	
    void loadCachedDataForSystems();
	set<string> getAllKeywords();	
    CloudsVisualSystemPreset getRandomEnabledPreset();
//  	void exportStandalonePresets();
    
  protected:

	CloudsVisualSystemPreset dummyPreset;
//    vector<CloudsVisualSystemPreset> getPresetsForSystem(string systemName);
    vector<int> getPresetIndicesForSystem(const string& systemName);
//	vector< ofPtr<CloudsVisualSystem> > systems;
//	map<string, ofPtr<CloudsVisualSystem> > nameToVisualSystem;

    vector<int> enabledPresetsIndex;
	
    int indexForPreset(const string& presetID);
    
	//preset ID to list of clip link names
	map<string, vector<string> > linkedClips;
    map<string, vector<string> > suppressedClips;
	map<string, vector<string> > keywords;
	
	//computed after linkedClips is populated
	void updateClipPresetLinks();
	map<string, vector<string> > clipToPresetLinks;
	
	string getKeywordFilePath();

	void addDefaultPresetForSystem(const string& systemName);
	

	bool allSystemsPopulated;
	//this instantiates and registers all the visual systems, called once at setup
//	void registerVisualSystem(ofPtr<CloudsVisualSystem> system);
	float lastBackupTime;
	float backupTimeInterval;

};
