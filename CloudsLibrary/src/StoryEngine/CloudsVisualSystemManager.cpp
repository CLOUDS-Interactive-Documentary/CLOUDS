
#include "CloudsVisualSystemManager.h"
#include "ofxXmlSettings.h"
#include "CloudsGlobal.h"

#ifndef CLOUDS_NO_VS

#include "CloudsVisualSystem3DModelLoader.h"
#include "CloudsVisualSystemAutomata.h"
#include "CloudsVisualSystemBallDroppings.h"
#include "CloudsVisualSystemChromogram.h"
#include "CloudsVisualSystemCities.h"
#include "CloudsVisualSystemCode.h"
#include "CloudsVisualSystemColony.h"
#include "CloudsVisualSystemConnectors.h"
#include "CloudsVisualSystemCosmic.h"
#include "CloudsVisualSystemDataCascade.h"
#include "CloudsVisualSystemDrawnLine.h"
#include "CloudsVisualSystemExampleBox2D.h"
#include "CloudsVisualSystemExampleMPMFluid.h"
#include "CloudsVisualSystemExampleOpenSteer.h"
#include "CloudsVisualSystemExampleVectorMath.h"
#include "CloudsVisualSystemFireworks.h"
#include "CloudsVisualSystemFlocking.h"
#include "CloudsVisualSystemFlying.h"
#include "CloudsVisualSystemForkingPaths.h"
#include "CloudsVisualSystemGPUParticles.h"
//#include "CloudsVisualSystemGameOfLife.h"
#include "CloudsVisualSystemHistogram.h"
#include "CloudsVisualSystemLIA.h"
#include "CloudsVisualSystemLSystem.h"
#include "CloudsVisualSystemLaplacianTunnel.h"
#include "CloudsVisualSystemMandala.h"
#include "CloudsVisualSystemMarchingCubes.h"
#include "CloudsVisualSystemMazeGenerator.h"
#include "CloudsVisualSystemMemory.h"
#include "CloudsVisualSystemMetaballs.h"
#include "CloudsVisualSystemNbody.h"
#include "CloudsVisualSystemNeurons.h"
#include "CloudsVisualSystemOcean.h"
//#include "CloudsVisualSystemOpenP53DIntro.h"
#include "CloudsVisualSystemOpenP5Caustics.h"
#include "CloudsVisualSystemOpenP5DrawingMachine10.h"
#include "CloudsVisualSystemOpenP5Fifteen.h"
#include "CloudsVisualSystemOpenP5Hackpact.h"
#include "CloudsVisualSystemOpenP5Machine.h"
#include "CloudsVisualSystemOpenP5NoiseSphere.h"
#include "CloudsVisualSystemOpenP5SeaOfLines.h"
#include "CloudsVisualSystemOpenP5Sixteen.h"
#include "CloudsVisualSystemOpenP5SpaceJunk.h"
#include "CloudsVisualSystemOpenP5Spaghetti.h"
#include "CloudsVisualSystemOpenP5SpinningSolids.h"
#include "CloudsVisualSystemOpenP5TextUniverse.h"
#include "CloudsVisualSystemOrbit.h"
#include "CloudsVisualSystemOscillations.h"
#include "CloudsVisualSystemPages.h"
#include "CloudsVisualSystemPaintBrush.h"
//#include "CloudsVisualSystemQuineVisualSystem.h"
#include "CloudsVisualSystemRGBDVideo.h"
#include "CloudsVisualSystemRandomDigits.h"
#include "CloudsVisualSystemReplicator.h"
#include "CloudsVisualSystemRipples.h"
#include "CloudsVisualSystemRulez.h"
#include "CloudsVisualSystemScrape.h"
#include "CloudsVisualSystemSatoruhiga.h"
#include "CloudsVisualSystemSwim.h"
//#include "CloudsVisualSystemThingsInTheDark.h"
#include "CloudsVisualSystemTwitter.h"
#include "CloudsVisualSystemVectorFlow.h"
#include "CloudsVisualSystemVerletForm.h"
#include "CloudsVisualSystemVision.h"
#include "CloudsVisualSystemVoro.h"
#include "CloudsVisualSystemWebHistory.h"
#include "CloudsVisualSystemWorld.h"
#include "CloudsVisualSystemXstatic.h"
#include "CloudsVisualSystemYellowTail.h"

// register a mapping of visual system constructors
// so it's easy to instantiate them in groups when needed
template< typename T > CloudsVisualSystem* fCreate() {
    return new T();
}
typedef CloudsVisualSystem* (*tConstructor)();
#define ARRAY_SIZE(a)                             \
((sizeof(a) / sizeof(*(a))) /                     \
 static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

typedef CloudsVisualSystem* (*tConstructor)();
struct Mapping {
	const char *classname;
	tConstructor constructor;
	operator map<string,tConstructor>::value_type() const {
		return map<string,tConstructor>::value_type(classname, constructor);
	}
} mapping[] = {

	{ "3DModelLoader", &fCreate<CloudsVisualSystem3DModelLoader> },
	{ "Automata", &fCreate<CloudsVisualSystemAutomata> },
	{ "BallDroppings", &fCreate<CloudsVisualSystemBallDroppings> },
	{ "Chromogram", &fCreate<CloudsVisualSystemChromogram> },
	{ "Cities", &fCreate<CloudsVisualSystemCities> },
	{ "Code", &fCreate<CloudsVisualSystemCode> },
	{ "Colony", &fCreate<CloudsVisualSystemColony> },
	{ "Connectors", &fCreate<CloudsVisualSystemConnectors> },
	{ "Cosmic", &fCreate<CloudsVisualSystemCosmic> },
	{ "DataCascade", &fCreate<CloudsVisualSystemDataCascade> },
	{ "DrawnLine", &fCreate<CloudsVisualSystemDrawnLine> },
	{ "ExampleBox2D", &fCreate<CloudsVisualSystemExampleBox2D> },
	{ "ExampleMPMFluid", &fCreate<CloudsVisualSystemExampleMPMFluid> },
	{ "ExampleOpenSteer", &fCreate<CloudsVisualSystemExampleOpenSteer> },
	{ "ExampleVectorMath", &fCreate<CloudsVisualSystemExampleVectorMath> },
	{ "Fireworks", &fCreate<CloudsVisualSystemFireworks> },
	{ "Flocking", &fCreate<CloudsVisualSystemFlocking> },
	{ "Flying", &fCreate<CloudsVisualSystemFlying> },
	{ "ForkingPaths", &fCreate<CloudsVisualSystemForkingPaths> },
	{ "GPUParticles", &fCreate<CloudsVisualSystemGPUParticles> },
//	{ "GameOfLife", &fCreate<CloudsVisualSystemGameOfLife> },
	{ "Histogram", &fCreate<CloudsVisualSystemHistogram> },
	{ "LIA", &fCreate<CloudsVisualSystemLIA> },
	{ "LSystem", &fCreate<CloudsVisualSystemLSystem> },
	{ "LaplacianTunnel", &fCreate<CloudsVisualSystemLaplacianTunnel> },
	{ "Mandala", &fCreate<CloudsVisualSystemMandala> },
	{ "MarchingCubes", &fCreate<CloudsVisualSystemMarchingCubes> },
	{ "MazeGenerator", &fCreate<CloudsVisualSystemMazeGenerator> },
	{ "Memory", &fCreate<CloudsVisualSystemMemory> },
	{ "Metaballs", &fCreate<CloudsVisualSystemMetaballs> },
	{ "Nbody", &fCreate<CloudsVisualSystemNbody> },
	{ "Neurons", &fCreate<CloudsVisualSystemNeurons> },
	{ "Ocean", &fCreate<CloudsVisualSystemOcean> },
//	{ "OpenP53DIntro", &fCreate<CloudsVisualSystemOpenP53DIntro> },
	{ "OpenP5Caustics", &fCreate<CloudsVisualSystemOpenP5Caustics> },
	{ "OpenP5DrawingMachine10", &fCreate<CloudsVisualSystemOpenP5DrawingMachine10> },
	{ "OpenP5Fifteen", &fCreate<CloudsVisualSystemOpenP5Fifteen> },
	{ "OpenP5Hackpact", &fCreate<CloudsVisualSystemOpenP5Hackpact> },
	{ "OpenP5Machine", &fCreate<CloudsVisualSystemOpenP5Machine> },
	{ "OpenP5NoiseSphere", &fCreate<CloudsVisualSystemOpenP5NoiseSphere> },
	{ "OpenP5SeaOfLines", &fCreate<CloudsVisualSystemOpenP5SeaOfLines> },
	{ "OpenP5Sixteen", &fCreate<CloudsVisualSystemOpenP5Sixteen> },
	{ "OpenP5SpaceJunk", &fCreate<CloudsVisualSystemOpenP5SpaceJunk> },
	{ "OpenP5Spaghetti", &fCreate<CloudsVisualSystemOpenP5Spaghetti> },
	{ "OpenP5SpinningSolids", &fCreate<CloudsVisualSystemOpenP5SpinningSolids> },
	{ "OpenP5TextUniverse", &fCreate<CloudsVisualSystemOpenP5TextUniverse> },
	{ "Orbit", &fCreate<CloudsVisualSystemOrbit> },
	{ "Oscillations", &fCreate<CloudsVisualSystemOscillations> },
	{ "Pages", &fCreate<CloudsVisualSystemPages> },
	{ "PaintBrush", &fCreate<CloudsVisualSystemPaintBrush> },
//	{ "QuineVisualSystem", &fCreate<CloudsVisualSystemQuineVisualSystem> },
//	{ "RGBD", &fCreate<CloudsVisualSystemRGBD> },
	{ "RGBDVideo", &fCreate<CloudsVisualSystemRGBDVideo> },
	{ "RandomDigits", &fCreate<CloudsVisualSystemRandomDigits> },
	{ "Replicator", &fCreate<CloudsVisualSystemReplicator> },
	{ "Ripples", &fCreate<CloudsVisualSystemRipples> },
	{ "Rulez", &fCreate<CloudsVisualSystemRulez> },
	{ "Scrape", &fCreate<CloudsVisualSystemScrape> },
	{ "Satoruhiga", &fCreate<CloudsVisualSystemSatoruhiga> },
	{ "Swim", &fCreate<CloudsVisualSystemSwim> },
//	{ "ThingsInTheDark", &fCreate<CloudsVisualSystemThingsInTheDark> },
	{ "Twitter", &fCreate<CloudsVisualSystemTwitter> },
	{ "VectorFlow", &fCreate<CloudsVisualSystemVectorFlow> },
	{ "VerletForm", &fCreate<CloudsVisualSystemVerletForm> },
	{ "Vision", &fCreate<CloudsVisualSystemVision> },
	{ "Voro", &fCreate<CloudsVisualSystemVoro> },
	{ "WebHistory", &fCreate<CloudsVisualSystemWebHistory> },
	{ "World", &fCreate<CloudsVisualSystemWorld> },
	{ "Xstatic", &fCreate<CloudsVisualSystemXstatic> },
	{ "YellowTail", &fCreate<CloudsVisualSystemYellowTail> }
};

map<string, tConstructor> constructors(mapping,mapping + ARRAY_SIZE(mapping));

//--------------------------------------------------------------------
ofPtr<CloudsVisualSystem> CloudsVisualSystemManager::InstantiateSystem(string systemName){
	if(constructors.find(systemName) != constructors.end()){
		return ofPtr<CloudsVisualSystem>( constructors[systemName]() );
	}
	return ofPtr<CloudsVisualSystem>();
}

//--------------------------------------------------------------------
vector< ofPtr<CloudsVisualSystem> > CloudsVisualSystemManager::InstantiateSystems(vector<CloudsVisualSystemPreset>& systemPresets){
	
	vector< ofPtr<CloudsVisualSystem> > systems;
	for(int i = 0; i < systemPresets.size(); i++){
		if(constructors.find(systemPresets[i].systemName) != constructors.end()){
			cout << "INSTANTIATING SYSTEM " << systemPresets[i].systemName << " WITH PRESET " << systemPresets[i].presetName << endl;
			systemPresets[i].system = InstantiateSystem( systemPresets[i].systemName );
			cout << "CloudsVisualSystemManager::InstantiateSystems - SYSTEM NULL? " << (systemPresets[i].system == NULL ? "YES" : "NO") << endl;
			
			systems.push_back( systemPresets[i].system );
		}
		else{
			ofLogError("CloudsVisualSystemManager::InstantiateSystems") << "Couldn't find system " << systemPresets[i].systemName;
		}
	}
	return systems;
}

#endif //end coupling guard


bool preset_sort(CloudsVisualSystemPreset a, CloudsVisualSystemPreset b){
	int comp = a.systemName.compare( b.systemName );
	if( comp != 0 ){
		return comp < 0;
	}
	return a.presetName.compare( b.presetName ) < 0;
}

//--------------------------------------------------------------------
CloudsVisualSystemManager::CloudsVisualSystemManager(){
	backupTimeInterval = 60*2;
	lastBackupTime = ofGetElapsedTimef() - backupTimeInterval;
	allSystemsPopulated = false;
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::populateVisualSystems(){

#ifndef CLOUDS_NO_VS
	nameToVisualSystem.clear();
	allSystemsPopulated = true;
	
	systems.clear();
	presets.clear();
    vector<string> systemNames;
	for(map<string, tConstructor>::iterator it = constructors.begin(); it != constructors.end(); ++it) {
		registerVisualSystem( InstantiateSystem(it->first) );
	}

    loadPresets();
#endif
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::updatePresetsForSystem(ofPtr<CloudsVisualSystem> system){
	
	if(system == NULL) return;

	#ifndef CLOUDS_NO_VS
	vector<CloudsVisualSystemPreset>& currentPresets = getPresetsForSystem(system->getSystemName());
	for(int i = 0; i < currentPresets.size(); i++){
		currentPresets[i].stillPresent = false;
	}

	//load all actual presets
	vector<string> systemPresets = system->getPresets();
	for(int p = 0; p < systemPresets.size(); p++){
		if( !systemHasPreset( system->getSystemName(), systemPresets[p] ) ){
			cout << "FOUND NEW PRESET " << system->getSystemName() << " " << systemPresets[p]  << endl;
			CloudsVisualSystemPreset preset;
			preset.presetName = systemPresets[ p ];
			preset.systemName = system->getSystemName();
			preset.loadTimeInfo();
			preset.stillPresent = true;
			presets.push_back(preset);
			nameToPresets[preset.systemName].push_back( preset );
		}
		else{
			cout << "FOUND EXISTING PRESET " << system->getSystemName() << " " << systemPresets[p]  << endl;
			CloudsVisualSystemPreset& preset = getPresetForSystem( system->getSystemName(), systemPresets[p] );
			preset.stillPresent = true;
		}
	}
	
	for(int i = currentPresets.size()-1;  i >= 0; i--){
		if(!currentPresets[i].stillPresent){
			cout << "ERASING " << system->getSystemName() << " " << currentPresets[i].presetName << endl;
			for(int p = presets.size()-1; p >= 0; p--){
				if(presets[p].systemName == system->getSystemName() &&
				   presets[p].presetName == currentPresets[i].presetName)
				{
					cout << "found side-by-side preset " << endl;
					presets.erase(presets.begin() + p);
					break;
				}
			}
			currentPresets.erase(currentPresets.begin() + i);			
		}
	}

	//if we cleaned it out make sure there is a default to get started
//	if(currentPresets.size() == 0){
	addDefaultPresetForSystem( system->getSystemName() );
	
//	CloudsVisualSystemPreset newPreset;
//	newPreset.systemName = system->getSystemName();
//	newPreset.presetName = "+New Preset";
//	newPreset.enabled = false;
//	nameToPresets[newPreset.systemName].push_back(newPreset);
//	presets.push_back(newPreset);
//	
//	CloudsVisualSystemPreset currentPreset;
//	currentPreset.systemName = system->getSystemName();
//	currentPreset.presetName = "+Current State";
//	currentPreset.enabled = false;
//	nameToPresets[currentPreset.systemName].push_back(currentPreset);
//	presets.push_back(currentPreset);
//	
	sort(presets.begin(), presets.end(), preset_sort);
	populateEnabledSystemIndeces();
	
	savePresets();
	
	#endif
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::deletePreset(int i){
	
//	cout << "ERASING " << system->getSystemName() << " " << currentPresets[i].presetName << endl;
	
	if(i >= presets.size()) return;

	CloudsVisualSystemPreset& preset = presets[i];
	vector<CloudsVisualSystemPreset>& presetMap = getPresetsForSystem( preset.systemName );
	
	for(int p = 0; p < presetMap.size(); p++){
		if(presetMap[p].presetName == preset.presetName){
			presetMap.erase(presetMap.begin() + p);
			cout << "Found side by side to delete" << endl;
			break;
		}
	}
	preset.eraseFiles();
	presets.erase(presets.begin() + i);
	
	sort(presets.begin(), presets.end(), preset_sort);
	populateEnabledSystemIndeces();

	savePresets();
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::registerVisualSystem(ofPtr<CloudsVisualSystem> system){
#ifndef CLOUDS_NO_VS
	nameToVisualSystem[system->getSystemName()] = system;
#endif
	systems.push_back( system );
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::loadPresets(){
	cout << "Loading presets " << endl;
	presets.clear();
	nameToPresets.clear();
	keywords.clear();
	suppressedClips.clear();
	
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
		
		vector<string> splitName = ofSplitString(name, "_",true,true);
		string systemName = splitName[0];
		splitName.erase(splitName.begin()); //delete the system name
		string presetName = ofJoinString(splitName, "_"); //join up with the rest of the characters
		CloudsVisualSystemPreset* preset;
		CloudsVisualSystemPreset newPreset;
		bool existingPreset = systemHasPreset(systemName, presetName);
		if(existingPreset){
			preset = &getPresetForSystem(systemName, presetName);
		}
		else {
			preset = &newPreset;
			preset->systemName = systemName;
			preset->presetName = presetName;
			preset->loadTimeInfo();
		}
		
		vector<string> presetKeywords = ofSplitString( keywordXml.getValue("keywords", "") , "|", true, true );		
		if(!existingPreset){
			keywords[ preset->getID() ] = presetKeywords;
		}

		if(keywordXml.tagExists("suppressions")){
			keywordXml.pushTag("suppressions");
			int numSuppressions = keywordXml.getNumTags("clip");
			for(int i=0; i<numSuppressions;i++){
				string suppressedLinkName = keywordXml.getValue("clip", "", i);
                //				cout << "found suppression " << suppressedLinkName << endl;
				suppressedClips[preset->getID()].push_back(suppressedLinkName);
			}
			keywordXml.popTag(); //suppressions
		}
		
		preset->comments = keywordXml.getValue("comments","");
		preset->grade = keywordXml.getValue("grade", "");
		preset->enabled = keywordXml.getValue("enabled", true );
		preset->oculusCompatible = keywordXml.getValue("oculus", false );
		preset->checkHasFiles();
		preset->systemIsRegistered = false;
#ifndef CLOUDS_NO_VS
		preset->systemIsRegistered = constructors.find(systemName) != constructors.end();
#endif
		if(existingPreset){
			for(int i = 0; i < presets.size(); i++){
				//replace the existing preset with the updated one
				if(presets[i].getID() == preset->getID()){
					presets[i] = *preset;
				}
			}
		}
		else{
			presets.push_back(*preset);
			nameToPresets[preset->systemName].push_back(*preset);
		}
        keywordXml.popTag(); //system
	}
	
#ifndef CLOUDS_NO_VS
	for(map<string, tConstructor>::iterator it = constructors.begin(); it != constructors.end(); ++it) {
		addDefaultPresetForSystem(it->first);
//		if(nameToPresets[it->first].size() == 0){
//			CloudsVisualSystemPreset preset;
//			preset.systemName = it->first;
//			preset.presetName = "_default";
//			preset.enabled = false;
//			nameToPresets[preset.systemName].push_back(preset);
//			presets.push_back(preset);
//		}
	}
	
#endif
	sort(presets.begin(), presets.end(), preset_sort);
	populateEnabledSystemIndeces();
    cout << "** LOADED PRESETS " << presets.size() << endl;
}

void CloudsVisualSystemManager::addDefaultPresetForSystem(string systemName){
	
	CloudsVisualSystemPreset newPreset;
	newPreset.systemName = systemName;
	newPreset.presetName = "+New Preset";
	newPreset.enabled = false;
	nameToPresets[newPreset.systemName].push_back(newPreset);
	presets.push_back(newPreset);

	CloudsVisualSystemPreset currentState;
	currentState.systemName = systemName;
	currentState.presetName = "+Current State";
	currentState.enabled = false;
	nameToPresets[currentState.systemName].push_back(currentState);
	presets.push_back(currentState);
}



//--------------------------------------------------------------------
void CloudsVisualSystemManager::populateEnabledSystemIndeces(){
    enabledPresetsIndex.clear();
    for(int i = 0; i < presets.size(); i++){
#ifdef OCULUS_RIFT
        if(presets[i].enabled && presets[i].oculusCompatible){
            enabledPresetsIndex.push_back(i);
        }
#else
        if(presets[i].enabled && !presets[i].oculusCompatible){
            enabledPresetsIndex.push_back(i);
        }
#endif
    }
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::savePresets(){
	
	string keywordsFile = getKeywordFilePath();
	
	
	if( (ofGetElapsedTimef() - lastBackupTime) >= backupTimeInterval ){
		char backup[1024];
		sprintf(backup, "%s_backup_Y.%02d_MO.%02d_D.%02d_H.%02d_MI.%02d.xml",
				ofFilePath::removeExt(keywordsFile).c_str(),
				ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes());
		
		lastBackupTime = ofGetElapsedTimef();
		
		if(!ofFile(keywordsFile).copyTo(backup)){
			ofSystemAlertDialog("UNABLE TO CREATE KEYWORD ASSOCIATION BACK UP");
			return;
		}
		
		cout << "BACKUPING UP FILE FROM " << keywordsFile << " to " << backup << endl;
	}
	
	ofxXmlSettings keywordXml;
	int systemIndex = 0;
	for(int i = 0; i < presets.size(); i++){
		CloudsVisualSystemPreset& preset = presets[i];
		string presetName = preset.presetName;
		ofStringReplace(presetName, " ", "_");
		string presetIdentifier = preset.systemName + "_" + presetName;
		string keywordString = ofJoinString(keywords[presetIdentifier], "|");
		if(presetName == "default"  ||
		   presetName == "_default" ||
		   presetName.at(0) == '+')
		{
			continue;
		}
		
		//cout << "saving " << presetIdentifier << " -> (" << keywordString << ")" << endl;
		
		keywordXml.addTag("system");
		keywordXml.addAttribute("system", "name", presetIdentifier, systemIndex);
		
		keywordXml.pushTag("system",systemIndex);
		
		//KEYWORDS
		keywordXml.addValue("keywords", keywordString);
        
		//SUPPRESSIONS
		keywordXml.addTag("suppressions");
        keywordXml.pushTag("suppressions");
        vector<string>& clips =  getSuppressionsForPreset( preset.getID() );
        for (int i =0; i<clips.size(); i++) {
            keywordXml.addValue("clip",clips[i]);
        }
        keywordXml.popTag(); // suppressions
		
		//COMMENT
		keywordXml.addValue("comments", preset.comments);
		keywordXml.addValue("grade", preset.grade);
		keywordXml.addValue("enabled", preset.enabled);
		keywordXml.addValue("oculus", preset.oculusCompatible);
		
		keywordXml.popTag(); // pop system
		
		systemIndex++;
	}
	
	if(!keywordXml.saveFile( keywordsFile )){
		ofSystemAlertDialog("UNABLE TO SAVE KEYWORD FILE " + keywordsFile );
	}
}

//--------------------------------------------------------------------
CloudsVisualSystemPreset CloudsVisualSystemManager::getRandomVisualSystem(){
    if(presets.size() == 0){
        ofLogError() << "No Visual System presets";
        return dummyPreset;
    }
	return presets[ ofRandom(presets.size()) ];
}

//--------------------------------------------------------------------
vector<CloudsVisualSystemPreset> CloudsVisualSystemManager::getPresetsForKeyword(string keyword){
	vector<string> keys;
	keys.push_back(keyword);
    return getPresetsForKeywords(keys);
}

//--------------------------------------------------------------------
vector<CloudsVisualSystemPreset> CloudsVisualSystemManager::getPresetsForKeywords(vector<string>& keys){
	vector<CloudsVisualSystemPreset> presetsWithKeywords;
    
	for(int i = 0; i < presets.size(); i++){
		vector<string> presetKeywords = keywordsForPreset(i);
		for(int k = 0; k < keys.size(); k++){
			if( ofContains(presetKeywords, keys[k]) ){
				presetsWithKeywords.push_back(presets[i]);
				continue;
			}
		}
	}
	return presetsWithKeywords;
}

//--------------------------------------------------------------------
vector<CloudsVisualSystemPreset>& CloudsVisualSystemManager::getPresetsForSystem(string systemName){
//	if( nameToPresets.find(systemName) == nameToPresets.end() ){
//		ofLogError() << "Couldn't find presets for system " << systemName << endl;
//	}
	return nameToPresets[systemName];
}

//--------------------------------------------------------------------
bool CloudsVisualSystemManager::systemHasPreset(string systemName, string presetName){
	vector<CloudsVisualSystemPreset>& presets = getPresetsForSystem(systemName);
	for(int i = 0; i < presets.size(); i++){
		if(presets[i].presetName == presetName){
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------
CloudsVisualSystemPreset& CloudsVisualSystemManager::getPresetForSystem(string systemName, string presetName){
	vector<CloudsVisualSystemPreset>& presets = getPresetsForSystem(systemName);
	for(int i = 0; i < presets.size(); i++){
		if(presets[i].presetName == presetName){
			return presets[i];
		}
	}
	ofLogError("CloudsVisualSystemManager::getPresetForSystem") << "Couldn't find preset " << systemName << " " << presetName;
	return dummyPreset;
}

//--------------------------------------------------------------------
CloudsVisualSystemPreset& CloudsVisualSystemManager::getPresetWithID(string presetID){
	for(int i = 0; i < presets.size(); i++){
		if(presets[i].getID() == presetID){
			return presets[i];
		}
	}
	ofLogError("CloudsVisualSystemManager::getPresetWithID") << "Couldn't find preset with ID " << presetID;
	return dummyPreset;
}

//--------------------------------------------------------------------
set<string> CloudsVisualSystemManager::getAllKeywords(){
	set<string> allKeywords;
	for(int i = 0; i < presets.size(); i++){
		if(presetHasKeywords(presets[i])){
			for(int k = 0; k < keywords[presets[i].getID()].size(); k++){
				allKeywords.insert( keywords[presets[i].getID()][k] );
			}
		}
	}
	return allKeywords;
}

//--------------------------------------------------------------------
vector<CloudsVisualSystemPreset>& CloudsVisualSystemManager::getPresets(){
	return presets;
}

//--------------------------------------------------------------------
string CloudsVisualSystemManager::getKeywordFilePath(){
	return GetCloudsDataPath() + "/links/visualsystems_keywords_db.xml";
}

//--------------------------------------------------------------------
vector<string> CloudsVisualSystemManager::keywordsForPreset(int index){
	return keywords[ presets[index].getID() ];
}

//--------------------------------------------------------------------
bool CloudsVisualSystemManager::presetHasKeywords(CloudsVisualSystemPreset& preset){
	return keywords.find( preset.getID() ) != keywords.end();
}

//--------------------------------------------------------------------
vector<string> CloudsVisualSystemManager::keywordsForPreset(CloudsVisualSystemPreset& preset){
	if(keywords.find( preset.getID() ) == keywords.end()){
		ofLogError() << "no keywords found for preset " << preset.getID() << endl;
	}
	
	return keywords[ preset.getID() ];
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::suppressClip(string presetID, string clipName){
    if( ! isClipSuppressed(presetID,clipName)){
        suppressedClips[presetID].push_back(clipName);
        cout<<"Suppressed Clip: "<<clipName<<" for Visual System: "<<presetID<<endl;
    }
}

//--------------------------------------------------------------------
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
//--------------------------------------------------------------------
CloudsVisualSystemPreset CloudsVisualSystemManager::getRandomEnabledPreset(){
    
    return presets[enabledPresetsIndex[ofRandom(enabledPresetsIndex.size())]];
}
//--------------------------------------------------------------------
void CloudsVisualSystemManager::unsuppressClip(string presetID, int presetIndex){
    if(suppressedClips.find(presetID) != suppressedClips.end()){
		suppressedClips[presetID].erase(suppressedClips[presetID].begin() +presetIndex);
    }
    else{
        ofLogError()<<"Visual System Preset :" <<presetID<<" suppression not foun!"<<endl;
    }
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::exportStandalonePresets(){
	string standaloneExportFolder = GetCloudsDataPath() + "standalonePresets/";
	ofDirectory(standaloneExportFolder).create();
	
	cout << "COPYING PRESETS!" << endl;
	set< string > systemsWithPresets;
	for(int i = 0; i < presets.size(); i++){
		if(presets[i].enabled){
            
			string presetSourceDirectory = GetCloudsVisualSystemDataPath( presets[i].systemName ) + "Presets/" + presets[i].presetName;
			string presetTargetDirectory = standaloneExportFolder + "VisualSystems/" + presets[i].systemName + "/Presets/";
            
			cout << "COPYING " << presetSourceDirectory << " to " << presetTargetDirectory << endl;
			
			ofDirectory(presetTargetDirectory).create(true);
			ofDirectory(presetSourceDirectory).copyTo(presetTargetDirectory, true);
			
			systemsWithPresets.insert(presets[i].systemName);
		}
	}
    
	cout << "COPYING SUPPORTING FILES" << endl;
	set< string >::iterator it;
	for(it = systemsWithPresets.begin(); it != systemsWithPresets.end(); it++){
//		ofPtr<CloudsVisualSystem> sys = *it;
		string systemName = GetCloudsVisualSystemDataPath(*it);
		ofDirectory otherFiles( systemName );
		otherFiles.listDir();
		for(int f = 0; f < otherFiles.size(); f++){
			if(otherFiles.getName(f) != "Presets"){
				cout << "copying file " << otherFiles.getName(f);
				otherFiles.getFile(f).copyTo( standaloneExportFolder + "VisualSystems/" + *it );
			}
		}
	}
}

//--------------------------------------------------------------------
bool CloudsVisualSystemManager::isClipSuppressed(string presetID,string clip){
    int deadIndex;
    return isClipSuppressed(presetID, clip, deadIndex);
}

//--------------------------------------------------------------------
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

//--------------------------------------------------------------------
vector<string>& CloudsVisualSystemManager::getSuppressionsForPreset(string presetID){
    return suppressedClips[presetID];
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::setKeywordsForPreset(CloudsVisualSystemPreset& preset, vector<string>& newKeywords ){
	keywords[ preset.getID() ] = newKeywords;
}


