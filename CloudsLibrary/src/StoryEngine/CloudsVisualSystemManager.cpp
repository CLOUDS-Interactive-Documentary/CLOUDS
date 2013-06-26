
#include "CloudsVisualSystemManager.h"

#ifndef CLOUDS_NO_VS
#include "CloudsVisualSystemRezanator.h"
#include "CloudsVisualSystemComputationTicker.h"
#include "CloudsVisualSystemLSystems.h"
#include "CloudsVisualSystemVoro.h"
#include "CloudsVisualSystemVerletForm.h"
#include "CloudsVisualSystemCities.h"
#include "CloudsVisualSystemAmber.h"
#include "CloudsVisualSystemCollaboration1.h"
#include "CloudsVisualSystemAmber.h"
#include "CloudsVisualSystemVectorFlow.h"

#endif

CloudsVisualSystemManager::CloudsVisualSystemManager(){

}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::populateVisualSystems(){
#ifndef CLOUDS_NO_VS
	nameToVisualSystem.clear();
	systems.clear();
	presets.clear();
	
	registerVisualSystem( new CloudsVisualSystemComputationTicker() );
	registerVisualSystem( new CloudsVisualSystemLSystems() );
	registerVisualSystem( new CloudsVisualSystemVoro() );
	registerVisualSystem( new CloudsVisualSystemCollaboration1() );
	registerVisualSystem( new CloudsVisualSystemCities() );
	registerVisualSystem( new CloudsVisualSystemVerletForm() );
//	registerVisualSystem( new CloudsVisualSystemVectorFlow() );
	
	//REZA: Adding this makes it so the pointclouds don't show..
//	registerVisualSystem( new CloudsVisualSystemAmber() );
#endif
    
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::registerVisualSystem(CloudsVisualSystem* system){
#ifndef CLOUDS_NO_VS	
	ofLogVerbose() << "Registering system " << system->getSystemName();
	
	system->setup();
	
	systems.push_back( system );
	nameToVisualSystem[system->getSystemName()] = system;
	refreshPresets();
	
#endif
}

//--------------------------------------------------------------------
CloudsVisualSystemPreset& CloudsVisualSystemManager::getRandomVisualSystem(){
	return presets[ ofRandom(presets.size()) ];
}

//--------------------------------------------------------------------
const vector<CloudsVisualSystemPreset>& CloudsVisualSystemManager::getPresets(){
	return presets;
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::refreshPresets(){
#ifndef CLOUDS_NO_VS	
	presets.clear();
	for(int i = 0; i < systems.size(); i++){
		vector<string> systemPresets = systems[i]->getPresets();
		
		if(systemPresets.size() == 0){
			cout << "NO PRESETS for SYSTEM " <<  systems[i]->getSystemName() << endl;
			CloudsVisualSystemPreset preset;
			preset.presetName = "no-preset";
			preset.system = systems[i];
			presets.push_back(preset);
		}
		else {
			for(int p = 0; p < systemPresets.size(); p++){
				CloudsVisualSystemPreset preset;
				preset.presetName = systemPresets[p];
				preset.system = systems[i];
				
				//TODO: get keywords
				
				presets.push_back(preset);
			}
		}
	}
   #endif
}

