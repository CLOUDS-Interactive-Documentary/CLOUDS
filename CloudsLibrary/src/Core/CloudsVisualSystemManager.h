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
    
    void populateEnabledSystemIndeces();
	CloudsVisualSystemPreset getRandomVisualSystem();
	CloudsVisualSystemPreset& getPresetWithID(const string& presetID);
	bool systemHasPreset(const string& systemName, const string& presetName);
    bool hasPresetWithID(const string& ID);
    bool hasPresetWithID(const string& ID, int& index);
	CloudsVisualSystemPreset& getPresetForSystem(const string& systemName, const string& presetName);


	vector<int> getFilteredPresetIndeces(bool enabledScreen = false, bool oculus = false, bool gradeA = false, string systemName = "");
	vector<CloudsVisualSystemPreset>& getPresets();
	
	bool presetHasKeywords(CloudsVisualSystemPreset& preset);
	vector<string> keywordsForPreset(int index);
	vector<string> keywordsForPreset(CloudsVisualSystemPreset& preset);
	vector<string> keywordsForPreset(const string& systemName, const string& presetName);
	vector<CloudsVisualSystemPreset> getPresetsForKeyword(const string& keyword);
	vector<CloudsVisualSystemPreset> getPresetsForKeywords(vector<string>& keywords, string clipName = "", bool isInterlude = false);
	vector<CloudsVisualSystemPreset> getAllInterludes();
    
    static bool generateCloudsDataCopyScript();

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
    map<string, CloudsVisualSystemCredit>& getVisualSystemCredits();
    
  protected:

	CloudsVisualSystemPreset dummyPreset;
    vector<int> getPresetIndicesForSystem(const string& systemName);
    vector<int> enabledPresetsIndex;
	
    int indexForPreset(const string& presetID);
    
	//preset ID to list of clip link names
	map< string, vector<string> > linkedClips;
    map< string, vector<string> > suppressedClips;
	map< string, vector<string> > keywords;
	
    map< string, CloudsVisualSystemCredit> visualSystemCredits;
    
	//computed after linkedClips is populated
	void updateClipPresetLinks();
	map<string, vector<string> > clipToPresetLinks;
	
	string getKeywordFilePath();
    string getCreditsFilePath();

	void addDefaultPresetForSystem(const string& systemName);
    
    void parseVisualSystemCredits();

	bool allSystemsPopulated;
	float lastBackupTime;
	float backupTimeInterval;

};
