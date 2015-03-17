//
//  CloudsGlobal.h
//  CloudsStoryEngine
//
//  Created by James George on 7/11/13.
//
//



#include "ofMain.h"
#include "CloudsGlobal.h"

bool confirmedDataPath = false;
bool usingDevelopmentFolder = false;

//--------------------------------------------------------------------
string GetCloudsDataPath(bool ignored)
{
	if(!confirmedDataPath){
		usingDevelopmentFolder = ofDirectory("../../../CloudsData/").exists();
		if(!usingDevelopmentFolder){
			ofDirectory("CloudsData/").create();
		}
		confirmedDataPath = true;
	}
    return string(usingDevelopmentFolder ? "../../../" : "") + "CloudsData"  + (ignored ? "_ignored" : "") + "/";
}

//--------------------------------------------------------------------
string GetCloudsVisualSystemDataPath(string systemName, bool ignoredFolder){
	//  building from src project file
	string datapath;
	if(ofDirectory("../../../CloudsData/").exists()){
		datapath = string("../../../CloudsData") + (ignoredFolder ? "_ignored" : "") + "/visualsystems/" + systemName + "/";
	}
	//  stand alone full app
	else if(ofDirectory("CloudsData/").exists()){
		datapath =  string("CloudsData") + (ignoredFolder ? "_ignored" : "") + "/visualsystems/" + systemName + "/";
	}
	//  stand alone single app
	else{
		datapath =  "../../../data/";
	}
	
	return datapath;
}

//--------------------------------------------------------------------
string language = "ENGLISH";
bool languageSet = false;
string GetLanguage(){
	if(!languageSet){
		string languageFile = GetCloudsDataPath() + "language.txt";
		if(ofFile(languageFile).exists()){
			language = ofBufferFromFile(languageFile).getText();
		}
		languageSet = true;
	}
	return language;
}

//--------------------------------------------------------------------
void SetLanguage(string newLanguage){
	language = newLanguage;
	languageSet = true;
}

//--------------------------------------------------------------------
string relinkFilePath(string filePath){
	
	vector<string> drives;
	
	drives.push_back("Seance");
	drives.push_back("Nebula");
	drives.push_back("Supernova");
	drives.push_back("WhiteDwarf");
    
	if( !ofFile(filePath).exists() ){
		for(int i = 0; i < drives.size(); i++){
			if(ofFile::doesFileExist("/Volumes/"+ drives[i]+"/")){
				for(int j = 0; j < drives.size(); j++){
					if(j != i){
						ofStringReplace(filePath, drives[j], drives[i]);
					}
				}
				break;
			}
		}
	}
	return filePath;
}
