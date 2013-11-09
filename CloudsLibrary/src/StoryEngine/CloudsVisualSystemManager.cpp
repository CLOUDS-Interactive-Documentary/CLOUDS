
#include "CloudsVisualSystemManager.h"
#include "ofxXmlSettings.h"
#include "CloudsGlobal.h"

#ifndef CLOUDS_NO_VS

//JAMES SYSTEMS
#include "CloudsVisualSystemDataCascade.h"
#include "CloudsVisualSystemVectorFlow.h"
#include "CloudsVisualSystemForkingPaths.h"
#include "CloudsVisualSystemOcean.h"
#include "CloudsVisualSystemRGBDVideo.h"
#include "CloudsVisualSystemConnectors.h"

//CONTRACTOR SYSTEMS
//pati
#include "CloudsVisualSystemLSystem.h"
#include "CloudsVisualSystemVoro.h"
#include "CloudsVisualSystemCities.h"
#include "CloudsVisualSystemWorld.h"
#include "CloudsVisualSystemMemory.h"
#include "CloudsVisualSystemPaintBrush.h"

//reza
#include "CloudsVisualSystemCosmic.h"
#include "CloudsVisualSystemFlocking.h"

//lars
#include "CloudsVisualSystemFireworks.h"
#include "CloudsVisualSystemMarchingCubes.h"
//#include "3DModelVisualSystem.h"
#include "CloudsVisualSystemMandala.h"
#include "CloudsVisualSystemMazeGenerator.h"
#include "CloudsVisualSystemRandomDigits.h"

//CODE STORM
#include "CloudsVisualSystemOscillations.h"
#include "CloudsVisualSystemScrape.h"
#include "CloudsVisualSystemWebHistory.h"
#include "CloudsVisualSystemPages.h"
//#include "QuineVisualSystem.h"

//COMMISSIONS
#include "CloudsVisualSystemVerletForm.h"
#include "CloudsVisualSystemNeurons.h"
#include "CloudsVisualSystemLaplacianTunnel.h"
#include "CloudsVisualSystemReplicator.h"
#include "CloudsVisualSystemLIA.h"
#include "CloudsVisualSystemDrawnLine.h"
//#include "CloudsVisualSystemGameOfLife.h"
#include "CloudsVisualSystemYellowTail.h"

//EXAMPLES
#include "CloudsVisualSystemExampleVectorMath.h"
#include "CloudsVisualSystemExampleBox2D.h"
#include "CloudsVisualSystemExampleMPMFluid.h"
#include "CloudsVisualSystemExampleOpenSteer.h"
#include "CloudsVisualSystemVision.h"

//OPENP5
//#include "CloudsVisualSystemOpenP53DIntro.h"
#include "CloudsVisualSystemOpenP5DrawingMachine10.h"
#include "CloudsVisualSystemOpenP5Hackpact.h"
#include "CloudsVisualSystemOpenP5Machine.h"
#include "CloudsVisualSystemOpenP5NoiseSphere.h"
#include "CloudsVisualSystemOpenP5SeaOfLines.h"
#include "CloudsVisualSystemOpenP5Sixteen.h"
#include "CloudsVisualSystemOpenP5SpaceJunk.h"
#include "CloudsVisualSystemOpenP5Spaghetti.h"
#include "CloudsVisualSystemOpenP5SpinningSolids.h"
#include "CloudsVisualSystemOpenP5TextUniverse.h"

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
	
//	{ "3DModel", &fCreate<CloudsVisualSystem3DModelVisualSystem> },
	{ "Cities", &fCreate<CloudsVisualSystemCities> },
//	{ "Colony", &fCreate<CloudsVisualSystemColony> },
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
	{ "ForkingPaths", &fCreate<CloudsVisualSystemForkingPaths> },
//	{ "GameOfLife", &fCreate<CloudsVisualSystemGameOfLife> },
	{ "LIA", &fCreate<CloudsVisualSystemLIA> },
	{ "LSystem", &fCreate<CloudsVisualSystemLSystem> },
	{ "LaplacianTunnel", &fCreate<CloudsVisualSystemLaplacianTunnel> },
	{ "MandalaVisualSystem", &fCreate<CloudsVisualSystemMandala> },
	{ "MazeGenerator", &fCreate<CloudsVisualSystemMazeGenerator> },
	{ "Memory", &fCreate<CloudsVisualSystemMemory> },
	{ "Metaballs", &fCreate<CloudsVisualSystem> },
	{ "Neurons", &fCreate<CloudsVisualSystemNeurons> },
	{ "Ocean", &fCreate<CloudsVisualSystemOcean> },
//	{ "OpenP53DIntro", &fCreate<CloudsVisualSystemOpenP53DIntro> },
	{ "OpenP5DrawingMachine10", &fCreate<CloudsVisualSystemOpenP5DrawingMachine10> },
	{ "OpenP5Hackpact", &fCreate<CloudsVisualSystemOpenP5Hackpact> },
	{ "OpenP5Machine", &fCreate<CloudsVisualSystemOpenP5Machine> },
	{ "OpenP5NoiseSphere", &fCreate<CloudsVisualSystemOpenP5NoiseSphere> },
	{ "OpenP5SeaOfLines", &fCreate<CloudsVisualSystemOpenP5SeaOfLines> },
	{ "OpenP5Sixteen", &fCreate<CloudsVisualSystemOpenP5Sixteen> },
	{ "OpenP5SpaceJunk", &fCreate<CloudsVisualSystemOpenP5SpaceJunk> },
	{ "OpenP5Spaghetti", &fCreate<CloudsVisualSystemOpenP5Spaghetti> },
	{ "OpenP5SpinningSolids", &fCreate<CloudsVisualSystemOpenP5SpinningSolids> },
	{ "OpenP5TextUniverse", &fCreate<CloudsVisualSystemOpenP5TextUniverse> },
	{ "Oscillations", &fCreate<CloudsVisualSystemOscillations> },
	//TODO: Rename
//	{ "Pages", &fCreate<CloudsVisualSystemPages> },
	{ "PaintBrush", &fCreate<CloudsVisualSystemPaintBrush> },
	//TODO: Rename
//	{ "QuineVisualSystem", &fCreate<CloudsVisualSystemQuineVisualSystem> },
	{ "RGBDVideo", &fCreate<CloudsVisualSystemRGBDVideo> },
	{ "RandomDigits", &fCreate<CloudsVisualSystemRandomDigits> },
	{ "Replicator", &fCreate<CloudsVisualSystemReplicator> },
//TODO: Rename	
//	{ "Scrape", &fCreate<CloudsVisualSystemScrape> },
//	{ "ThingsInTheDark", &fCreate<CloudsVisualSystemThingsInTheDark> },
	{ "VectorFlow", &fCreate<CloudsVisualSystemVectorFlow> },
	{ "VerletForm", &fCreate<CloudsVisualSystemVerletForm> },
	//TODO: Rename
	{ "Vision", &fCreate<CloudsVisualSystemVision> },
	{ "Voro", &fCreate<CloudsVisualSystemVoro> },
	//TODO: rename class
//	{ "WebHistory", &fCreate<CloudsVisualSystemWebHistory> },
	{ "World", &fCreate<CloudsVisualSystemWorld> },
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
void CloudsVisualSystemManager::registerVisualSystem(ofPtr<CloudsVisualSystem> system){
#ifndef CLOUDS_NO_VS
	nameToVisualSystem[system->getSystemName()] = system;
#endif
	systems.push_back( system );
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::loadPresets(){
	
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
	
	//This is flagged if we don't want this app to be dependent on visual systems
	//they presets will then be loaded all from keywords file
//#ifndef CLOUDS_NO_VS
//
//	for(int i = 0; i < systems.size(); i++){
//		vector<string> systemPresets = systems[i]->getPresets();
//		
//		if(systemPresets.size() == 0){
//			cout << "NO PRESETS for SYSTEM " << systems[i]->getSystemName() << endl;
//			CloudsVisualSystemPreset preset;
//			preset.systemName = systems[i]->getSystemName();
//			preset.presetName = "no-preset";
//			preset.system = systems[i];
//			
//			nameToPresets[preset.systemName].push_back( preset );
//			presets.push_back(preset);
//		}
//		else {
//			for(int p = 0; p < systemPresets.size(); p++){
//				CloudsVisualSystemPreset preset;
//				preset.presetName = systemPresets[p];
//				preset.systemName = systems[i]->getSystemName();
//				preset.system = systems[i];
//				preset.loadTimeInfo();
//				presets.push_back(preset);
//				nameToPresets[preset.systemName].push_back( preset );
//			}
//		}
//	}
//#endif
	
	int numSystems = keywordXml.getNumTags("system");
	for(int i = 0; i < numSystems; i++){
		string name = keywordXml.getAttribute("system", "name", "no-name", i);
		keywordXml.pushTag( "system", i );
		vector<string> presetKeywords = ofSplitString( keywordXml.getValue("keywords", "") , "|", true, true );
		keywords[ name ] = presetKeywords;
		CloudsVisualSystemPreset preset;
		//if we have populated everything, get the preset it's already set
//		if(allSystemsPopulated){
//			preset = getPresetWithID(name);
//		}
//		else{
			vector<string> splitName = ofSplitString(name, "_",true,true);
			preset.systemName = splitName[0];
			splitName.erase(splitName.begin()); //delete the system name
			preset.presetName = ofJoinString(splitName, "_"); //join up with the rest of the characters
			preset.loadTimeInfo();
			presets.push_back(preset);
			nameToPresets[preset.systemName].push_back(preset);
//		}

		
		if(keywordXml.tagExists("suppressions")){
			keywordXml.pushTag("suppressions");
			int numSuppressions = keywordXml.getNumTags("clip");
			for(int i=0; i<numSuppressions;i++){
				string suppressedLinkName = keywordXml.getValue("clip", "", i);
                //				cout << "found suppression " << suppressedLinkName << endl;
				suppressedClips[name].push_back(suppressedLinkName);
			}
			keywordXml.popTag(); //suppressions
		}
		
		preset.comments = keywordXml.getValue("comments","");
		preset.grade = keywordXml.getValue("grade", "");
		preset.enabled = keywordXml.getValue("enabled", true );
		preset.oculusCompatible = keywordXml.getValue("oculus", false );
        keywordXml.popTag(); //system
	}
	
	sort(presets.begin(), presets.end(), preset_sort);
	populateEnabledSystemIndeces();
    cout << "** LOADED PRESETS " << presets.size() << endl;
}


//--------------------------------------------------------------------
void CloudsVisualSystemManager::populateEnabledSystemIndeces(){
    enabledPresetsIndex.clear();
    for(int i = 0; i<presets.size(); i++){
        if(presets[i].enabled){
            enabledPresetsIndex.push_back(i);
        }
    }
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::saveKeywords(){
	
	string keywordsFile = getKeywordFilePath();
	
	map<string,vector<string> >::iterator it;
	
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
	for(it = keywords.begin(); it != keywords.end(); it++){
		string presetName = it->first;
		string keywordString = ofJoinString(it->second, "|");
		
		CloudsVisualSystemPreset& preset = getPresetWithID(presetName);
		
		cout << "saving " << presetName << " -> (" << keywordString << ")" << endl;
		
		keywordXml.addTag("system");
		keywordXml.addAttribute("system", "name", presetName, systemIndex);
		
		keywordXml.pushTag("system",systemIndex);
		
		//KEYWORDS
		keywordXml.addValue("keywords", keywordString);
        
		//SUPPRESSIONS
		keywordXml.addTag("suppressions");
        keywordXml.pushTag("suppressions");
        vector<string>& clips =  getSuppressionsForPreset(presetName);
        for (int i =0; i<clips.size(); i++) {
            keywordXml.addValue("clip",clips[i]);
        }
        keywordXml.popTag();//suppressions
		
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
	vector<string> keywords;
	keywords.push_back(keyword);
    return getPresetsForKeywords(keywords);
}

//--------------------------------------------------------------------
vector<CloudsVisualSystemPreset> CloudsVisualSystemManager::getPresetsForKeywords(vector<string>& keywords){
	vector<CloudsVisualSystemPreset> presetsWithKeywords;
    
	for(int i = 0; i < presets.size(); i++){
		for(int k = 0; k < keywords.size(); k++){
			if( ofContains(keywordsForPreset(i), keywords[k]) ){
				presetsWithKeywords.push_back(presets[i]);
				continue;
			}
		}
	}
	return presetsWithKeywords;
}

//--------------------------------------------------------------------
vector<CloudsVisualSystemPreset>& CloudsVisualSystemManager::getPresetsForSystem(string systemName){
	if( nameToPresets.find(systemName) == nameToPresets.end() ){
		ofLogError() << "Couldn't find presets for system " << systemName << endl;
	}
	return nameToPresets[systemName];
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
	return getDataPath() + "/links/visualsystems_keywords_db.xml";
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

#ifndef CLOUDS_NO_VS
//--------------------------------------------------------------------
void CloudsVisualSystemManager::exportStandalonePresets(){
	string standaloneExportFolder = getDataPath() + "standalonePresets/";
	ofDirectory(standaloneExportFolder).create();
	
	cout << "COPYING PRESETS!" << endl;
	set< ofPtr<CloudsVisualSystem> > systemsWithPresets;
	for(int i = 0; i < presets.size(); i++){
		if(presets[i].enabled){
            
			string presetSourceDirectory = presets[i].system->getVisualSystemDataPath() + "Presets/" + presets[i].presetName;
			string presetTargetDirectory = standaloneExportFolder + "VisualSystems/" + presets[i].systemName + "/Presets/";
            
			cout << "COPYING " << presetSourceDirectory << " to " << presetTargetDirectory << endl;
			
			ofDirectory(presetTargetDirectory).create(true);
			ofDirectory(presetSourceDirectory).copyTo(presetTargetDirectory, true);
			
			systemsWithPresets.insert(presets[i].system);
		}
	}
    
	cout << "COPYING SUPPORTING FILES" << endl;
	set< ofPtr<CloudsVisualSystem> >::iterator it;
	for(it = systemsWithPresets.begin(); it != systemsWithPresets.end(); it++){
		ofPtr<CloudsVisualSystem> sys = *it;
		ofDirectory otherFiles( sys->getVisualSystemDataPath() );
		otherFiles.listDir();
		for(int f = 0; f < otherFiles.size(); f++){
			if(otherFiles.getName(f) != "Presets"){
				cout << "copying file " << otherFiles.getName(f);
				otherFiles.getFile(f).copyTo(standaloneExportFolder + "VisualSystems/" + sys->getSystemName());
			}
		}
	}
    
}
#endif

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


