//
//  CloudsVisualSystemManager.h
//  CLOUDS
//
//  Created by James George on 6/19/13.
//
//

#pragma once

#include "ofMain.h"
//#include "CloudsVisualSystem.h"
#include "CloudsVisualSystemPreset.h"

class CloudsVisualSystemManager {
  public:
	CloudsVisualSystemManager();
	
	void populateVisualSystems();
	CloudsVisualSystemPreset& getRandomVisualSystem();
	
	CloudsVisualSystem* visualSystemWithName(string systemName);
	
  protected:
	
	vector<CloudsVisualSystem*> systems;
	map<string, CloudsVisualSystem*> nameToVisualSystem;

	vector<CloudsVisualSystemPreset> presets;
	
	//this instantiates and registers all the visual systems, called once at setup
	void registerVisualSystem(CloudsVisualSystem* system);

};