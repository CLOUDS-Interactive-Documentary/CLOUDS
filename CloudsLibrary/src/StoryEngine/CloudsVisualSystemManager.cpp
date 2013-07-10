
#include "CloudsVisualSystemManager.h"

#ifndef CLOUDS_NO_VS

#include "CloudsVisualSystemComputationTicker.h"
#include "CloudsVisualSystemLSystems.h"
#include "CloudsVisualSystemVoro.h"
#include "CloudsVisualSystemVerletForm.h"
#include "CloudsVisualSystemCities.h"
#include "CloudsVisualSystemAmber.h"
#include "CloudsVisualSystemCollaboration1.h"
#include "CloudsVisualSystemAmber.h"
#include "CloudsVisualSystemVectorFlow.h"
#include "CloudsVisualSystemWorld.h"
#include "CloudsVisualSystemLaplacianTunnel.h"
#include "CloudsVisualSystemHiga.h"
#include "CloudsVisualSystemForkingPaths.h"
#include "CloudsVisualSystemMemory.h"
#include "CloudsVisualSystemPaintBrush.h"
#include "CloudsVisualSystemOcean.h"

#endif

CloudsVisualSystemManager::CloudsVisualSystemManager(){
	backupTimeInterval = 60*2;
	lastBackupTime = ofGetElapsedTimef() - backupTimeInterval;
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::populateVisualSystems(){
#ifndef CLOUDS_NO_VS
	nameToVisualSystem.clear();
	systems.clear();
	presets.clear();

	//	registerVisualSystem( new CloudsVisualSystemCollaboration1() );
	//	registerVisualSystem( new CloudsVisualSystemLaplacianTunnel() );
	//	registerVisualSystem( new CloudsVisualSystemVerletForm() );

	registerVisualSystem( new CloudsVisualSystemComputationTicker() );
	registerVisualSystem( new CloudsVisualSystemLSystems() );
	registerVisualSystem( new CloudsVisualSystemVoro() );
	registerVisualSystem( new CloudsVisualSystemCities() );
	registerVisualSystem( new CloudsVisualSystemVectorFlow() );
	registerVisualSystem( new CloudsVisualSystemWorld() );
	registerVisualSystem( new CloudsVisualSystemMemory() );
	registerVisualSystem( new CloudsVisualSystemHiga() );
	registerVisualSystem( new CloudsVisualSystemForkingPaths() );
	registerVisualSystem( new CloudsVisualSystemPaintBrush() );
	registerVisualSystem( new CloudsVisualSystemOcean() );
	
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
				presets.push_back(preset);
			}
		}
	}
	
	keywords.clear();
	ofxXmlSettings keywordXml;
	string keywordsFile = getKeywordFilePath();
	
	if(!keywordXml.loadFile( keywordsFile )){
		ofSystemAlertDialog("UNABLE TO LOAD KEYWORD FILE! " + keywordsFile + " Do not proceed");
		return;
	}
	
	int numSystems = keywordXml.getNumTags("system");
	for(int i = 0; i < numSystems; i++){
		string name = keywordXml.getAttribute("system", "name", "no-name", i);
		keywordXml.pushTag( "system", i );
		keywords[ name ] = ofSplitString( keywordXml.getValue("keywords", "") , "|", true, true );
		
		if(keywordXml.tagExists("suppressions")){
			keywordXml.pushTag("suppresions");
			int numSuppresions = keywordXml.getNumTags("clip");
			for(int i=0; i<numSuppresions;i++){
				suppressedClips[name].push_back(keywordXml.getValue("clip", "", i));
			}
			keywordXml.popTag(); //suppressions
		}
		
        keywordXml.popTag(); //system
	}
	
#endif
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::saveKeywords(){
	
	string keywordsFile = getKeywordFilePath();
	
	map<string,vector<string> >::iterator it;
	
	if( (ofGetElapsedTimef() - lastBackupTime) >= backupTimeInterval){
		char backup[1024];
		sprintf( backup, "%s_backup_Y.%02d_MO.%02d_D.%02d_H.%02d_MI.%02d.xml", ofFilePath::removeExt(keywordsFile).c_str(), ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes() );
		lastBackupTime = ofGetElapsedTimef();
		if(!ofFile(keywordsFile).copyTo(backup)){
			ofSystemAlertDialog("UNABLE TO CREATE KEYWORD ASSOCIATION BACK UP");
			return;
		}
		
		cout << "BACKUPING UP FILE FROM " << keywordsFile << " to " << backup << endl;
	}
	
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
        keywordXml.addTag("suppresions");
        keywordXml.pushTag("suppresions");
        vector<string>& clips =  getSuppressionsForPreset(presetName);
        for (int i =0; i<clips.size(); i++) {
            keywordXml.addValue("clip",clips[i]);
        }
        keywordXml.popTag();//suppressions
		keywordXml.popTag();
		
		systemIndex++;
	}
	
	if(!keywordXml.saveFile( keywordsFile )){
		ofSystemAlertDialog("UNABLE TO SAVE KEYWORD FILE " + keywordsFile );
	}
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
string CloudsVisualSystemManager::getKeywordFilePath(){
	return CloudsVisualSystem::getDataPath() + "/visualsystems/_keywordAssociations/keywords.xml";
}

//--------------------------------------------------------------------
vector<string> CloudsVisualSystemManager::keywordsForPreset(int index){
	return keywords[ presets[index].getID() ];
}

//--------------------------------------------------------------------
vector<string> CloudsVisualSystemManager::keywordsForPreset(CloudsVisualSystemPreset& preset){
	if(keywords.find( preset.getID() ) == keywords.end()){
		ofLogError() << "no keywords found for preset " << preset.getID() << endl;
	}
	
	return keywords[ preset.getID() ];
}
void CloudsVisualSystemManager::suppressClip(string presetID, string clipName){
    if( ! isClipSuppressed(presetID,clipName)){
        suppressedClips[presetID].push_back(clipName);
        cout<<"Suppressed Clip: "<<clipName<<" for Visual System: "<<presetID<<endl;
    }
    
    
}

void CloudsVisualSystemManager::unsuppressClip(string presetID, string clip){
    int suppressionIndex;
    if(isClipSuppressed( presetID, clip,suppressionIndex)){
        cout<<"Unsuppressing connection for Preset: "<<presetID<<" and "<<clip<<endl;
        unsuppressClip(presetID, suppressionIndex);
    }
    else{
        cout<<"Suppression not found for Preset: "<<presetID<<" and "<<clip<<endl;
    }
    
}

void CloudsVisualSystemManager::unsuppressClip(string presetID, int presetIndex){
    if(suppressedClips.find(presetID) != suppressedClips.end()){
    suppressedClips[presetID].erase(suppressedClips[presetID].begin() +presetIndex);
    }
    else{
        ofLogError()<<"Visual System Preset :" <<presetID<<" suppression not foun!"<<endl;
    }
    
}

bool CloudsVisualSystemManager::isClipSuppressed(string presetID,string clip){
    int deadIndex;
    return isClipSuppressed(presetID, clip,deadIndex);
}


bool CloudsVisualSystemManager::isClipSuppressed(string presetID,string clip, int& index){
    vector<string>& clips = suppressedClips[presetID];
    for(int i=0;i<clips.size();i++){
        if(clips[i]==clip){
            index = i;
            return true;
        }
    }
    return false;
}



vector<string>& CloudsVisualSystemManager::getSuppressionsForPreset(string presetID){
    return suppressedClips[presetID];
}
//--------------------------------------------------------------------
void CloudsVisualSystemManager::setKeywordsForPreset(CloudsVisualSystemPreset& preset, vector<string>& newKeywords ){
	keywords[ preset.getID() ] = newKeywords;
}


