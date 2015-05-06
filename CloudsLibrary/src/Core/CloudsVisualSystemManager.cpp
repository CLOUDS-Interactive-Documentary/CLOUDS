
#include "CloudsVisualSystemManager.h"
#include "ofxXmlSettings.h"
#include "CloudsGlobal.h"

//ALL PRESETS
static vector<CloudsVisualSystemPreset> presets;
static vector<CloudsVisualSystem*> systems;

#ifndef CLOUDS_NO_VS

//DISABLED
//#include "CloudsVisualSystemOpenP53DIntro.h"
//#include "CloudsVisualSystemOpenP5Caustics.h"
//#include "CloudsVisualSystemOpenP5DrawingMachine10.h"
//#include "CloudsVisualSystemOpenP5Sixteen.h"
//#include "CloudsVisualSystemPaintBrush.h"
//#include "CloudsVisualSystemThingsInTheDark.h"
//#include "CloudsVisualSystemUextrude.h"
//#include "CloudsVisualSystemWebHistory.h"

#include "CloudsVisualSystem2DVideo.h"
#include "CloudsVisualSystem3DModelLoader.h"
#include "CloudsVisualSystemAstrolabe.h"
#include "CloudsVisualSystemAutomata.h"
#include "CloudsVisualSystemBallDroppings.h"
#include "CloudsVisualSystemBalloons.h"

//#if !defined(OCULUS_RIFT) && defined(TARGET_OSX)
//#include "CloudsVisualSystemCandyMountains.h"
//#endif

#include "CloudsVisualSystemChromogram.h"
#include "CloudsVisualSystemCirclePacking.h"
#include "CloudsVisualSystemCircuit.h"
#include "CloudsVisualSystemCities.h"
#include "CloudsVisualSystemCode.h"
#include "CloudsVisualSystemColony.h"
#include "CloudsVisualSystemConnectors.h"
#include "CloudsVisualSystemCosmic.h"
#include "CloudsVisualSystemCubeCraft.h"
#include "CloudsVisualSystemDataCascade.h"
#include "CloudsVisualSystemDrawnLine.h"
#include "CloudsVisualSystemEmpty.h"
#include "CloudsVisualSystemExampleBox2D.h"
#include "CloudsVisualSystemExampleMPMFluid.h"
#include "CloudsVisualSystemExampleOpenSteer.h"
#include "CloudsVisualSystemExampleVectorMath.h"
#include "CloudsVisualSystemFireworks.h"
#include "CloudsVisualSystemFlocking.h"
#include "CloudsVisualSystemFlying.h"
#include "CloudsVisualSystemForkingPaths.h"
#include "CloudsVisualSystemGesturePaint.h"
#include "CloudsVisualSystemHistogram.h"
#include "CloudsVisualSystemLIA.h"
#include "CloudsVisualSystemLSystem.h"
#include "CloudsVisualSystemLaplacianTunnel.h"
#include "CloudsVisualSystemMandala.h"
#include "CloudsVisualSystemMarchingCubes.h"
#include "CloudsVisualSystemMazeGenerator.h"
#include "CloudsVisualSystemMazeGenerator2.h"
#include "CloudsVisualSystemMemory.h"
#include "CloudsVisualSystemMetaballs.h"
#include "CloudsVisualSystemNeurons.h"
#include "CloudsVisualSystemOcean.h"
#include "CloudsVisualSystemOpenP5Fifteen.h"
#include "CloudsVisualSystemOpenP5Hackpact.h"
#include "CloudsVisualSystemOpenP5Machine.h"
#include "CloudsVisualSystemOpenP5NoiseSphere.h"
#include "CloudsVisualSystemOpenP5SeaOfLines.h"
#include "CloudsVisualSystemOpenP5SpaceJunk.h"
#include "CloudsVisualSystemOpenP5Spaghetti.h"
#include "CloudsVisualSystemOpenP5SpinningSolids.h"
#include "CloudsVisualSystemOpenP5TextUniverse.h"
#include "CloudsVisualSystemOrbit.h"
#include "CloudsVisualSystemOscillations.h"
#include "CloudsVisualSystemPages.h"
#include "CloudsVisualSystemParadox.h"
#include "CloudsVisualSystemPhotoGlitch.h"
#include "CloudsVisualSystemProcess11.h"
#include "CloudsVisualSystemProcess18.h"
#include "CloudsVisualSystemRGBDVideo.h"
#include "CloudsVisualSystemRandomDigits2.h"
#include "CloudsVisualSystemReplicator.h"
#include "CloudsVisualSystemRipples.h"
#include "CloudsVisualSystemRulez.h"
#include "CloudsVisualSystemSatoruhiga.h"
#include "CloudsVisualSystemScrape.h"
#include "CloudsVisualSystemSwim.h"
#include "CloudsVisualSystemTerrain.h"
#include "CloudsVisualSystemTunnelDrawing.h"
#include "CloudsVisualSystemTwitter.h"
#include "CloudsVisualSystemVectorFlow.h"
#include "CloudsVisualSystemVerletForm.h"
#include "CloudsVisualSystemVision.h"
#include "CloudsVisualSystemVoro.h"
#include "CloudsVisualSystemWorld.h"
#include "CloudsVisualSystemWormHole.h"
#include "CloudsVisualSystemXstatic.h"
#include "CloudsVisualSystemYellowTail.h"
#include "CloudsVisualSystemSchlabberbox.h"

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
	
	{ "2DVideo",				&fCreate<CloudsVisualSystem2DVideo> },
	{ "3DModelLoader",			&fCreate<CloudsVisualSystem3DModelLoader> },
	{ "Astrolabe",				&fCreate<CloudsVisualSystemAstrolabe> },
	{ "Automata",				&fCreate<CloudsVisualSystemAutomata> },
	{ "BallDroppings",			&fCreate<CloudsVisualSystemBallDroppings> },
	{ "Balloons",				&fCreate<CloudsVisualSystemBalloons> },
#if !defined(OCULUS_RIFT) && defined(TARGET_OSX)
//	{ "CandyMountains",			&fCreate<CloudsVisualSystemCandyMountains> },
#endif
	{ "Chromogram",				&fCreate<CloudsVisualSystemChromogram> },
	{ "CirclePacking",			&fCreate<CloudsVisualSystemCirclePacking> },
	{ "Circuit",				&fCreate<CloudsVisualSystemCircuit> },
	{ "Cities",					&fCreate<CloudsVisualSystemCities> },
	{ "Code",					&fCreate<CloudsVisualSystemCode> },
	{ "Colony",					&fCreate<CloudsVisualSystemColony> },
	{ "Connectors",				&fCreate<CloudsVisualSystemConnectors> },
	{ "Cosmic",					&fCreate<CloudsVisualSystemCosmic> },
	{ "CubeCraft",				&fCreate<CloudsVisualSystemCubeCraft> },
	{ "DataCascade",			&fCreate<CloudsVisualSystemDataCascade> },
	{ "DrawnLine",				&fCreate<CloudsVisualSystemDrawnLine> },
	{ "Empty",					&fCreate<CloudsVisualSystemEmpty> },
	{ "ExampleBox2D",			&fCreate<CloudsVisualSystemExampleBox2D> },
	{ "ExampleMPMFluid",		&fCreate<CloudsVisualSystemExampleMPMFluid> },
	{ "ExampleOpenSteer",		&fCreate<CloudsVisualSystemExampleOpenSteer> },
	{ "ExampleVectorMath",		&fCreate<CloudsVisualSystemExampleVectorMath> },
	{ "Fireworks",				&fCreate<CloudsVisualSystemFireworks> },
	{ "Flocking",				&fCreate<CloudsVisualSystemFlocking> },
	{ "Flying",					&fCreate<CloudsVisualSystemFlying> },
	{ "ForkingPaths",			&fCreate<CloudsVisualSystemForkingPaths> },
	{ "GesturePaint",			&fCreate<CloudsVisualSystemGesturePaint> },
	{ "Histogram",				&fCreate<CloudsVisualSystemHistogram> },
	{ "LIA",					&fCreate<CloudsVisualSystemLIA> },
	{ "LSystem",				&fCreate<CloudsVisualSystemLSystem> },
	{ "LaplacianTunnel",		&fCreate<CloudsVisualSystemLaplacianTunnel> },
	{ "Mandala",				&fCreate<CloudsVisualSystemMandala> },
	{ "MarchingCubes",			&fCreate<CloudsVisualSystemMarchingCubes> },
	{ "MazeGenerator",			&fCreate<CloudsVisualSystemMazeGenerator> },
    { "MazeGenerator2",			&fCreate<CloudsVisualSystemMazeGenerator2> },
	{ "Memory",					&fCreate<CloudsVisualSystemMemory> },
	{ "Neurons",				&fCreate<CloudsVisualSystemNeurons> },
	{ "Ocean",					&fCreate<CloudsVisualSystemOcean> },
//64	{ "OpenP5Caustics",			&fCreate<CloudsVisualSystemOpenP5Caustics> },
//64	{ "OpenP5DrawingMachine10", &fCreate<CloudsVisualSystemOpenP5DrawingMachine10> },
	{ "OpenP5Fifteen",			&fCreate<CloudsVisualSystemOpenP5Fifteen> },
	{ "OpenP5Hackpact",			&fCreate<CloudsVisualSystemOpenP5Hackpact> },
	{ "OpenP5Machine",			&fCreate<CloudsVisualSystemOpenP5Machine> },
	{ "OpenP5NoiseSphere",		&fCreate<CloudsVisualSystemOpenP5NoiseSphere> },
	{ "OpenP5SeaOfLines",		&fCreate<CloudsVisualSystemOpenP5SeaOfLines> },
//64	{ "OpenP5Sixteen",			&fCreate<CloudsVisualSystemOpenP5Sixteen> },
	{ "OpenP5SpaceJunk",		&fCreate<CloudsVisualSystemOpenP5SpaceJunk> },
	{ "OpenP5Spaghetti",		&fCreate<CloudsVisualSystemOpenP5Spaghetti> },
	{ "OpenP5SpinningSolids",	&fCreate<CloudsVisualSystemOpenP5SpinningSolids> },
	{ "OpenP5TextUniverse",		&fCreate<CloudsVisualSystemOpenP5TextUniverse> },
	{ "Orbit",					&fCreate<CloudsVisualSystemOrbit> },
	{ "Oscillations",			&fCreate<CloudsVisualSystemOscillations> },
	{ "Paradox",				&fCreate<CloudsVisualSystemParadox> },
	{ "Pages",					&fCreate<CloudsVisualSystemPages> },
//64	{ "PaintBrush",				&fCreate<CloudsVisualSystemPaintBrush> },
	{ "PhotoGlitch",			&fCreate<CloudsVisualSystemPhotoGlitch> },
	{ "Process11",				&fCreate<CloudsVisualSystemProcess11> },
	{ "Process18",				&fCreate<CloudsVisualSystemProcess18> },
	{ "RGBDVideo",				&fCreate<CloudsVisualSystemRGBDVideo> },
    { "RandomDigits2",			&fCreate<CloudsVisualSystemRandomDigits2> },
	{ "Replicator",				&fCreate<CloudsVisualSystemReplicator> },
	{ "Ripples",				&fCreate<CloudsVisualSystemRipples> },
	{ "Rulez",					&fCreate<CloudsVisualSystemRulez> },
	{ "Satoruhiga",				&fCreate<CloudsVisualSystemSatoruhiga> },
	{ "Scrape",					&fCreate<CloudsVisualSystemScrape> },
    { "Schlabberbox",			&fCreate<CloudsVisualSystemSchlabberbox> },
	{ "Swim",					&fCreate<CloudsVisualSystemSwim> },
	{ "Terrain",				&fCreate<CloudsVisualSystemTerrain> },
//64	{ "ThingsInTheDark",		&fCreate<CloudsVisualSystemThingsInTheDark> },
	{ "TunnelDrawing",			&fCreate<CloudsVisualSystemTunnelDrawing> },
	{ "Twitter",				&fCreate<CloudsVisualSystemTwitter> },
//64	{ "Uextrude",				&fCreate<CloudsVisualSystemUextrude> },
	{ "VectorFlow",				&fCreate<CloudsVisualSystemVectorFlow> },
	{ "VerletForm",				&fCreate<CloudsVisualSystemVerletForm> },
	{ "Vision",					&fCreate<CloudsVisualSystemVision> },
	{ "Voro",					&fCreate<CloudsVisualSystemVoro> },
//64	{ "WebHistory",				&fCreate<CloudsVisualSystemWebHistory> },
	{ "World",					&fCreate<CloudsVisualSystemWorld> },
	{ "WormHole",				&fCreate<CloudsVisualSystemWormHole> },
	{ "Xstatic",				&fCreate<CloudsVisualSystemXstatic> },
	{ "YellowTail",				&fCreate<CloudsVisualSystemYellowTail> }
};

map<string, tConstructor> constructors(mapping,mapping + ARRAY_SIZE(mapping));

CloudsVisualSystem* CloudsVisualSystemManager::InstantiateSystem(const string& systemName){
	CloudsVisualSystem* system;
	if(constructors.find(systemName) != constructors.end()){
        system = constructors[systemName]();
	}
	else{
		system = new CloudsVisualSystemEmpty();
	    ofLogError("CloudsVisualSystemManager::InstantiateSystem") << systemName << " Doesn't Exist. Returning empty system." << endl;
	}
	systems.push_back( system );
	return system;
}

//--------------------------------------------------------------------
vector< CloudsVisualSystem* > CloudsVisualSystemManager::InstantiateSystems(vector<CloudsVisualSystemPreset>& systemPresets){
	
	vector< CloudsVisualSystem* > newSystems;
	for(int i = 0; i < systemPresets.size(); i++){
		if(constructors.find(systemPresets[i].systemName) != constructors.end()){
			cout << "INSTANTIATING SYSTEM " << systemPresets[i].systemName << " WITH PRESET " << systemPresets[i].presetName << endl;
            if(systemPresets[i].system != NULL){
                ofLogError("CloudsVisualSystemManager::InstantiateSystems") << "SYSTEM POINTER NOT NULL. LEAKING MEMORY";
            }
			systemPresets[i].system = InstantiateSystem( systemPresets[i].systemName );
//			cout << "CloudsVisualSystemManager::InstantiateSystems - SYSTEM NULL? " << (systemPresets[i].system == NULL ? "YES" : "NO") << endl;
			newSystems.push_back( systemPresets[i].system );
		}
		else{
			ofLogError("CloudsVisualSystemManager::InstantiateSystems") << "Couldn't find system " << systemPresets[i].systemName;
		}
	}
	return newSystems;
}

void CloudsVisualSystemManager::DeallocateSystems(){
    for(int i = 0; i < systems.size(); i++){
        
        cout << "Deleting system " << systems[i]->getSystemName() << endl;
        
        if( systems[i]->isPlaying() ) {
            systems[i]->stopSystem();
            ofLogError("CloudsVisualSystemManager::FreeSystemPointers") << "System " << systems[i]->getSystemName() << " Was still playing!";
        }
        
        if(systems[i]->isSetup()){
            systems[i]->exit();
        }

        delete systems[i];
    }
    
    systems.clear();
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

//--------------------------------------------------------------------
bool CloudsVisualSystemManager::HasSystemRegistered(CloudsVisualSystem* system){
    return ofContains(systems,system);
}

//--------------------------------------------------------------------
CloudsVisualSystemManager::CloudsVisualSystemManager(){
	backupTimeInterval = 60*2;
	lastBackupTime = ofGetElapsedTimef() - backupTimeInterval;
	allSystemsPopulated = false;
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::pastePresetToIndex(CloudsVisualSystemPreset preset, int index){

    linkedClips[ presets[index].getID() ] = linkedClips[ preset.getID() ];
    suppressedClips[ presets[index].getID() ] = suppressedClips[ preset.getID() ];
    keywords[ presets[index].getID() ] = keywords[ preset.getID() ];
    
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::updatePresetsForSystem(CloudsVisualSystem* system){
	
	if(system == NULL) return;

	#ifndef CLOUDS_NO_VS
//	vector<CloudsVisualSystemPreset>& currentPresets = getPresetsForSystem(system->getSystemName());
    vector<int> currentPresets = getPresetIndicesForSystem( system->getSystemName() );
    
	for(int i = 0; i < currentPresets.size(); i++){
		presets[ currentPresets[i] ].stillPresent = false;
	}

	//load all actual presets
	vector<string> systemPresets = system->getPresets();
	for(int p = 0; p < systemPresets.size(); p++){
		if( !systemHasPreset( system->getSystemName(), systemPresets[p] ) ){
//			cout << "FOUND NEW PRESET " << system->getSystemName() << " " << systemPresets[p]  << endl;
			CloudsVisualSystemPreset preset;
			preset.presetName = systemPresets[ p ];
			preset.systemName = system->getSystemName();
			preset.hasFiles = true;
			preset.stillPresent = true;
            preset.loadTimeInfo();
			presets.push_back(preset);
//			nameToPresets[preset.systemName].push_back( preset );
		}
		else{
//			cout << "FOUND EXISTING PRESET " << system->getSystemName() << " " << systemPresets[p]  << endl;
			CloudsVisualSystemPreset& preset = getPresetForSystem( system->getSystemName(), systemPresets[p] );
            preset.loadTimeInfo();
			preset.stillPresent = true;
		}
	}

	for(int i = currentPresets.size()-1;  i >= 0; i--){
        int presetIndex = currentPresets[i];
        CloudsVisualSystemPreset& preset = presets[ presetIndex ];
		if(!preset.stillPresent){
			cout << "ERASING " << system->getSystemName() << " " << preset.presetName << endl;
			for(int p = presets.size()-1; p >= 0; p--){
				if(presets[p].systemName == system->getSystemName() &&
				   presets[p].presetName == preset.presetName)
				{
					presets.erase(presets.begin() + presetIndex);
					break;
				}
			}
		}
	}

	addDefaultPresetForSystem( system->getSystemName() );

	sort(presets.begin(), presets.end(), preset_sort);
    
	populateEnabledSystemIndeces();
	
	savePresets();
	
	#endif
}

//--------------------------------------------------------------------
bool CloudsVisualSystemManager::hasPresetWithID(const string& presetID){
    int dummy;
    return hasPresetWithID(presetID, dummy);
}

//--------------------------------------------------------------------
bool CloudsVisualSystemManager::hasPresetWithID(const string& presetID, int& index){
    for(int i = 0; i < presets.size(); i++){
        if(presets[i].getID() == presetID){
            return true;
        }
    }
    index = -1;
    return false;
}

//--------------------------------------------------------------------
int CloudsVisualSystemManager::indexForPreset(const string& presetID){
    int index;
    hasPresetWithID(presetID, index);
    return index;
}

//--------------------------------------------------------------------
CloudsVisualSystem* CloudsVisualSystemManager::getEmptySystem(const string& mainKeyword, vector<string> keywords){
	#ifdef CLOUDS_NO_VS
	return NULL;
	#else
	CloudsVisualSystem* ptr = new CloudsVisualSystemEmpty();
	ptr->setKeywords(mainKeyword, keywords);
	return ptr;
	#endif
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::deletePreset(int i){
	
//	cout << "ERASING " << system->getSystemName() << " " << currentPresets[i].presetName << endl;
	
	if(i < 0 || i >= presets.size()){
		return;	
	}

	CloudsVisualSystemPreset& preset = presets[i];
	if(preset.presetName.at(0) == '+'){
		return;
	}
  
	preset.eraseFiles();
    presets.erase( presets.begin() + i );
    
	//vector<CloudsVisualSystemPreset>& presetMap = getPresetsForSystem( preset.systemName );
//	vector<int> presetIndeces = getPresetIndicesForSystem( preset.systemName );
//	for(int p = 0; p < presetIndeces.size(); p++){
//		if(presets[ presetIndeces[p] ].presetName == preset.presetName){
////			presetMap.erase(presetMap.begin() + p);
//            presets.erase( presets.begin() + presetIndeces[p] );
//			break;
//		}
//	}
    
//	presets.erase(presets.begin() + i);
	
	sort(presets.begin(), presets.end(), preset_sort);
    
	populateEnabledSystemIndeces();

	savePresets();
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::loadPresets(){
	cout << "Loading presets " << endl;
	presets.clear();
	keywords.clear();
	suppressedClips.clear();
	
    parseVisualSystemCredits();

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

		if(keywordXml.tagExists("links")){
			keywordXml.pushTag("links");
			int numLinks = keywordXml.getNumTags("clip");
			for(int i = 0; i < numLinks; i++){
				string linkedClipName = keywordXml.getValue("clip", "", i);
				linkedClips[preset->getID()].push_back(linkedClipName);
			}
			keywordXml.popTag(); //link
		}
		
		if(keywordXml.tagExists("suppressions")){
			keywordXml.pushTag("suppressions");
			int numSuppressions = keywordXml.getNumTags("clip");
			for(int i = 0; i < numSuppressions;i++){
				string suppressedLinkName = keywordXml.getValue("clip", "", i);
                //				cout << "found suppression " << suppressedLinkName << endl;
				suppressedClips[preset->getID()].push_back(suppressedLinkName);
			}
			keywordXml.popTag(); //suppressions
		}
		
		preset->comments = keywordXml.getValue("comments","");
		preset->grade = keywordXml.getValue("grade", "");
		preset->enabledScreen = keywordXml.getValue("enabled", true );
		preset->enabledOculus = keywordXml.getValue("oculus", false );
        preset->bIsHeavy      = keywordXml.getValue("heavy", false );
        preset->soundAllowVO = keywordXml.getValue("sound_allow_vo", false );
        preset->soundExcludeVO = keywordXml.getValue("sound_exclude_vo", false );
        preset->interlude = keywordXml.getValue("interlude", false );
		preset->checkHasFiles();
		preset->systemIsRegistered = false;
        preset->credits = visualSystemCredits[systemName];
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
		}
        keywordXml.popTag(); //system
	}
	
#ifndef CLOUDS_NO_VS
	for(map<string, tConstructor>::iterator it = constructors.begin(); it != constructors.end(); ++it) {
		addDefaultPresetForSystem(it->first);
	}
#endif
	sort(presets.begin(), presets.end(), preset_sort);
	
    updateClipPresetLinks();
	populateEnabledSystemIndeces();
    
    cout << "** LOADED PRESETS " << presets.size() << endl;
}

void CloudsVisualSystemManager::parseVisualSystemCredits(){
	ofxXmlSettings creditsXml;
	string creditsFile = getCreditsFilePath();
	if(!creditsXml.loadFile( creditsFile )){
		ofLogError("CloudsVisualSystemManager::parseVisualSystemCredits") << "Unable to load credits file " << creditsFile;
		return;
	}
    
    visualSystemCredits.clear();
    creditsXml.pushTag("visuals");
    int numSystems = creditsXml.getNumTags("system");
    for(int i = 0; i < numSystems; i++){
        string systemId = creditsXml.getAttribute("system", "id", "", i);
        creditsXml.pushTag("system", i);
        visualSystemCredits[systemId].line1 = creditsXml.getValue("line1", "");
        visualSystemCredits[systemId].line2 = creditsXml.getValue("line2", "");
        creditsXml.popTag(); //system

    }
    
    creditsXml.popTag(); //visuals
}

void CloudsVisualSystemManager::addDefaultPresetForSystem(const string& systemName){
	
	CloudsVisualSystemPreset newPreset;
	newPreset.systemName = systemName;
	newPreset.presetName = "+New Preset";
    newPreset.enabledScreen = false;
	newPreset.enabledOculus = false;
    newPreset.bIsHeavy      = false;
	presets.push_back(newPreset);

	CloudsVisualSystemPreset currentState;
	currentState.systemName = systemName;
	currentState.presetName = "+Current State";
    currentState.enabledScreen = false;
	currentState.enabledOculus = false;
	presets.push_back(currentState);
}



//--------------------------------------------------------------------
void CloudsVisualSystemManager::populateEnabledSystemIndeces(){
    enabledPresetsIndex.clear();
    for(int i = 0; i < presets.size(); i++){
#ifdef OCULUS_RIFT
        if(presets[i].enabledOculus){
            enabledPresetsIndex.push_back(i);
        }
#else
        if(presets[i].enabledScreen){
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
        
		//LINKS
		keywordXml.addTag("links");
		keywordXml.pushTag("links");
        vector<string>& linkedClips =  getLinksForPreset( preset.getID() );
        for (int i = 0; i < linkedClips.size(); i++) {
            keywordXml.addValue("clip", linkedClips[i]);
        }
		keywordXml.popTag();//links
		
		//SUPPRESSIONS
		keywordXml.addTag("suppressions");
        keywordXml.pushTag("suppressions");
        vector<string>& suppressedClips =  getSuppressionsForPreset( preset.getID() );
        for (int i = 0; i < suppressedClips.size(); i++) {
            keywordXml.addValue("clip",suppressedClips[i]);
        }
        keywordXml.popTag(); // suppressions
		
		//COMMENT
		keywordXml.addValue("comments", preset.comments);
		keywordXml.addValue("grade", preset.grade);
		keywordXml.addValue("enabled", preset.enabledScreen);
		keywordXml.addValue("oculus", preset.enabledOculus);
        keywordXml.addValue("heavy", preset.bIsHeavy);
		keywordXml.addValue("sound_exclude_vo", preset.soundExcludeVO);
        keywordXml.addValue("sound_allow_vo", preset.soundAllowVO);
        keywordXml.addValue("interlude", preset.interlude);
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
vector<CloudsVisualSystemPreset> CloudsVisualSystemManager::getPresetsForKeyword(const string& keyword){
	vector<string> keys;
	keys.push_back(keyword);
    return getPresetsForKeywords(keys);
}

//--------------------------------------------------------------------
vector<CloudsVisualSystemPreset> CloudsVisualSystemManager::getPresetsForKeywords(vector<string>& keys, string clipName, bool isInterlude){
	vector<CloudsVisualSystemPreset> presetsWithKeywords;
    vector<string> presetIds;
    
    //JG RIG
    
//	for(int i = 0; i < presets.size(); i++){
//        if(presets[i].systemName == "Twitter"){
//            presetsWithKeywords.push_back(presets[i]);
//        }
//    }
//
//    return presetsWithKeywords;
    
	for(int i = 0; i < presets.size(); i++){
		vector<string> presetKeywords = keywordsForPreset(i);
		for(int k = 0; k < keys.size(); k++){
			if( ofContains(presetKeywords, keys[k]) ){
                
                if(!isInterlude || (isInterlude && presets[i].interlude) ){
                    presetsWithKeywords.push_back(presets[i]);
                    presetIds.push_back(presets[i].getID());
                    continue;
                }
                
			}
		}
	}
	
	if(clipName != ""){
		//add linked clips
		if(clipToPresetLinks.find(clipName) != clipToPresetLinks.end()){
			for(int i = 0; i < clipToPresetLinks[clipName].size(); i++){
				if( !ofContains(presetIds, clipToPresetLinks[clipName][i]) ){
					presetsWithKeywords.push_back( getPresetWithID(clipToPresetLinks[clipName][i]) );
				}
			}
		}
	}
	
	return presetsWithKeywords;
}

vector<CloudsVisualSystemPreset> CloudsVisualSystemManager::getAllInterludes(){
	vector<CloudsVisualSystemPreset> interludePresets;
	for(int i = 0; i < presets.size(); i++){
		if( presets[i].interlude ){
			interludePresets.push_back(presets[i]);
		}		
	}
	return interludePresets;
}

vector<int> CloudsVisualSystemManager::getPresetIndicesForSystem(const string& systemName){
    vector<int> indices;
    for(int i = 0; i < presets.size(); i++){
        if(presets[i].systemName == systemName){
            indices.push_back( i );
        }
    }
    return indices;
}

void CloudsVisualSystemManager::loadCachedDataForSystems(){
#ifndef CLOUDS_NO_VS
    //making a local copy just to init and cache the data.
//	CloudsVisualSystemTwitter::getOldTweeterData();
//	CloudsVisualSystemTwitter::getNewTweeterData();
#endif    
}

//--------------------------------------------------------------------
bool CloudsVisualSystemManager::systemHasPreset(const string& systemName, const string& presetName){
	for(int i = 0; i < presets.size(); i++){
		if(presets[i].presetName == presetName &&
           presets[i].systemName == systemName)
        {
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------
CloudsVisualSystemPreset& CloudsVisualSystemManager::getPresetForSystem(const string& systemName, const string& presetName){
//	vector<CloudsVisualSystemPreset>& presets = getPresetsForSystem(systemName);
	for(int i = 0; i < presets.size(); i++){
		if(presets[i].presetName == presetName &&
           presets[i].systemName == systemName)
        {
			return presets[i];
		}
	}
	
    ofLogError("CloudsVisualSystemManager::getPresetForSystem") << "Couldn't find preset " << systemName << " " << presetName;
    
	return dummyPreset;
}

//--------------------------------------------------------------------
CloudsVisualSystemPreset& CloudsVisualSystemManager::getPresetWithID(const string& presetID){
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
vector<int> CloudsVisualSystemManager::getFilteredPresetIndeces(bool enabledScreen, bool enabledOculus, bool gradeA, string systemName){
	vector<int> filtered;
	for(int i = 0; i < presets.size(); i++){
		if((!enabledScreen || (enabledScreen && presets[i].enabledScreen)) &&
		   (!enabledOculus || (enabledOculus  && presets[i].enabledOculus)) &&
		   (!gradeA  || (gradeA  && presets[i].grade == "A")) &&
           (systemName == "" || (systemName == presets[i].systemName)))
		{
			filtered.push_back( i );
		}
	}
	return filtered;
}

//--------------------------------------------------------------------
vector<CloudsVisualSystemPreset>& CloudsVisualSystemManager::getPresets(){
	return presets;
}

//--------------------------------------------------------------------
string CloudsVisualSystemManager::getKeywordFilePath(){
	return GetCloudsDataPath() + "links/visualsystems_keywords_db.xml";
}
//--------------------------------------------------------------------
string CloudsVisualSystemManager::getCreditsFilePath(){
	return GetCloudsDataPath() + "language/" + GetLanguage() +"/bio/visuals.xml";
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
void CloudsVisualSystemManager::linkClip(const string& presetID, const string& clipName){
	if(!isClipLinked(presetID, clipName)){
		if(isClipSuppressed(presetID, clipName)){
			unsuppressClip(presetID, clipName);
		}
		linkedClips[presetID].push_back(clipName);
		updateClipPresetLinks();
	}
}

//--------------------------------------------------------------------
vector<string>& CloudsVisualSystemManager::getLinksForPreset(const string& presetID){
	return linkedClips[presetID];
}

//--------------------------------------------------------------------
bool CloudsVisualSystemManager::isClipLinked(const string& presetID,const string& clip){
	int deadIndex;
	return isClipLinked(presetID, clip, deadIndex);
}

//--------------------------------------------------------------------
bool CloudsVisualSystemManager::isClipLinked(const string& presetID,const string& clip, int& index){
	
	vector<string>& clips = linkedClips[presetID];
    for(int i = 0; i < clips.size(); i++){
        if(clips[i] == clip){
            index = i;
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::unlinkClip(const string& presetID, string clip){
	int index;
	if(isClipLinked(presetID, clip, index)){
		unlinkClip(presetID, index);
	}
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::unlinkClip(const string& presetID, int presetIndex){
	if(linkedClips.find(presetID) != linkedClips.end() && presetIndex < linkedClips[presetID].size())
	{
		linkedClips[presetID].erase( linkedClips[presetID].begin() + presetIndex );
		updateClipPresetLinks();
	}
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::updateClipPresetLinks(){
	clipToPresetLinks.clear();
	map<string, vector<string> >::iterator it;
	for(it = linkedClips.begin(); it != linkedClips.end(); it++){
		for(int i = 0; i < it->second.size(); i++){
			clipToPresetLinks[it->second[i]].push_back(it->first);
		}
	}
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::suppressClip(const string& presetID, const string& clipName){
    if(!isClipSuppressed(presetID,clipName)){
		if(isClipLinked(presetID, clipName)){
			unlinkClip(presetID, clipName);
		}

        suppressedClips[presetID].push_back(clipName);
    }
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::unsuppressClip(const string& presetID, const string& clip){
    int suppressionIndex;
    if(isClipSuppressed( presetID, clip, suppressionIndex)){
        unsuppressClip(presetID, suppressionIndex);
    }
}

//--------------------------------------------------------------------
bool CloudsVisualSystemManager::isClipSuppressed(const string& presetID, const string& clip){
    int deadIndex;
    return isClipSuppressed(presetID, clip, deadIndex);
}

//--------------------------------------------------------------------
bool CloudsVisualSystemManager::isClipSuppressed(const string& presetID,const string& clip, int& index){
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
void CloudsVisualSystemManager::unsuppressClip(const string& presetID, int presetIndex){
    if(suppressedClips.find(presetID) != suppressedClips.end() ){
		suppressedClips[presetID].erase(suppressedClips[presetID].begin() +presetIndex);
    }
    else{
        ofLogError()<<"Visual System Preset :" <<presetID<<" suppression not foun!"<<endl;
    }
}

//--------------------------------------------------------------------
CloudsVisualSystemPreset CloudsVisualSystemManager::getRandomEnabledPreset(){
    return presets[enabledPresetsIndex[ofRandom(enabledPresetsIndex.size())]];
}


//--------------------------------------------------------------------
vector<string>& CloudsVisualSystemManager::getSuppressionsForPreset(const string& presetID){
    return suppressedClips[presetID];
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::setKeywordsForPreset(CloudsVisualSystemPreset& preset, vector<string>& newKeywords ){
	keywords[ preset.getID() ] = newKeywords;
}

//--------------------------------------------------------------------
//void CloudsVisualSystemManager::exportStandalonePresets(){
//	string standaloneExportFolder = GetCloudsDataPath() + "standalonePresets/";
//	ofDirectory(standaloneExportFolder).create();
//	
//	cout << "COPYING PRESETS!" << endl;
//	set< string > systemsWithPresets;
//	for(int i = 0; i < presets.size(); i++){
//		if(presets[i].enabled){
//            
//			string presetSourceDirectory = GetCloudsVisualSystemDataPath( presets[i].systemName ) + "Presets/" + presets[i].presetName;
//			string presetTargetDirectory = standaloneExportFolder + "VisualSystems/" + presets[i].systemName + "/Presets/";
//            
//			cout << "COPYING " << presetSourceDirectory << " to " << presetTargetDirectory << endl;
//			
//			ofDirectory(presetTargetDirectory).create(true);
//			ofDirectory(presetSourceDirectory).copyTo(presetTargetDirectory, true);
//			
//			systemsWithPresets.insert(presets[i].systemName);
//		}
//	}
//    
//	cout << "COPYING SUPPORTING FILES" << endl;
//	set< string >::iterator it;
//	for(it = systemsWithPresets.begin(); it != systemsWithPresets.end(); it++){
//		string systemName = GetCloudsVisualSystemDataPath(*it);
//		ofDirectory otherFiles( systemName );
//		otherFiles.listDir();
//		for(int f = 0; f < otherFiles.size(); f++){
//			if(otherFiles.getName(f) != "Presets"){
//				cout << "copying file " << otherFiles.getName(f);
//				otherFiles.getFile(f).copyTo( standaloneExportFolder + "VisualSystems/" + *it );
//			}
//		}
//	}
//}

