
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
#include "CloudsVisualSystemLaplacianTunnel.h"

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
	registerVisualSystem( new CloudsVisualSystemVectorFlow() );
	registerVisualSystem( new CloudsVisualSystemLaplacianTunnel() );
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
	
	loadPresets();
	
#endif
}

//--------------------------------------------------------------------
CloudsVisualSystemPreset& CloudsVisualSystemManager::getRandomVisualSystem(){
	return presets[ ofRandom(presets.size()) ];
}

//--------------------------------------------------------------------
vector<CloudsVisualSystemPreset>& CloudsVisualSystemManager::getPresets(){
	return presets;
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::loadPresets(){
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
	
	keywords.clear();
	ofxXmlSettings keywordXml;
	keywordXml.loadFile( CloudsVisualSystem::getDataPath() + "/visualsystems/keywords.xml" );
	int numSystems = keywordXml.getNumTags("system");
	for(int i = 0; i < numSystems; i++){
		string name = keywordXml.getAttribute("system", "name", "no-name", i);
		keywordXml.pushTag( "system", i );
		keywords[ name ] = ofSplitString( keywordXml.getValue("keywords", "") , "|", true, true );
		keywordXml.popTag(); //system
	}
	
   #endif
}

void CloudsVisualSystemManager::saveKeywords(){
	map<string,vector<string> >::iterator it;
	
	ofxXmlSettings keywordXml;
	int systemIndex = 0;
	for(it = keywords.begin(); it != keywords.end(); it++){
		string presetName = it->first;
		string keywordString = ofJoinString(it->second, "|");
		
		cout << "saving " << presetName << " -> (" << keywordString << ")" << endl;
		
		keywordXml.addTag("system");
		keywordXml.addAttribute("system", "name", presetName, systemIndex);
		
		keywordXml.pushTag("system",systemIndex);
		keywordXml.addValue("keywords", keywordString);
		keywordXml.popTag();
		
		systemIndex++;
	}
	
	keywordXml.saveFile( CloudsVisualSystem::getDataPath() + "/visualsystems/keywords.xml" );
}

vector<string> CloudsVisualSystemManager::keywordsForPreset(int index){
	return keywords[ presets[index].getID() ];
}

vector<string> CloudsVisualSystemManager::keywordsForPreset(CloudsVisualSystemPreset& preset){
	if(keywords.find( preset.getID() ) == keywords.end()){
		ofLogError() << "no keywords found for preset " << preset.getID() << endl;
	}
	
	return keywords[ preset.getID() ];
}

void CloudsVisualSystemManager::setKeywordsForPreset(CloudsVisualSystemPreset& preset, vector<string>& newKeywords ){
	keywords[ preset.getID() ] = newKeywords;
}


