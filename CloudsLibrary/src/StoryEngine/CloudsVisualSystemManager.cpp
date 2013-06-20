
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
	
	vector<string> systemPresets = system->getPresets();
	if(systemPresets.size() == 0){
		cout << "NO PRESETS for SYSTEM " <<  system->getSystemName() << endl;
	}
	
	for(int i = 0; i < systemPresets.size(); i++){
		CloudsVisualSystemPreset preset;
		preset.presetName = systemPresets[i];
		preset.system = system;
		
		//TODO: get keywords
		
		presets.push_back(preset);
	}
   #endif
}

//--------------------------------------------------------------------
CloudsVisualSystemPreset& CloudsVisualSystemManager::getRandomVisualSystem(){
	return presets[ ofRandom(presets.size()) ];
}
