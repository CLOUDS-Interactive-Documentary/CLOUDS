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
	const vector<CloudsVisualSystemPreset>& getPresets();
	
	vector<string> keywordsForPreset(CloudsVisualSystemPreset& preset);
	vector<string> keywordsForPreset(CloudsVisualSystem* system, string presetName);
	vector<string> keywordsForPreset(string systemName, string presetName);

	
	void refreshPresets();
	void savePresets();
	
	void saveKeywords();
  protected:
	
	vector<CloudsVisualSystem*> systems;
	map<string, CloudsVisualSystem*> nameToVisualSystem;
	vector<CloudsVisualSystemPreset> presets;
	
	void loadKeywords();
	
	//this instantiates and registers all the visual systems, called once at setup
	void registerVisualSystem(CloudsVisualSystem* system);

};