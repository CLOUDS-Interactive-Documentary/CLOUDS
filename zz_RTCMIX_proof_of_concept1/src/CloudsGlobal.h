//
//  CloudsGlobal.h
//  CloudsStoryEngine
//
//  Created by James George on 7/11/13.
//
//

#pragma once

#include "ofMain.h"

static bool confirmedDataPath = false;
static bool usingDevelopmentFolder = false;

//--------------------------------------------------------------------
static string GetCloudsDataPath()
{
	if(!confirmedDataPath){
		usingDevelopmentFolder = ofDirectory("../../../CloudsData/").exists();
		if(!usingDevelopmentFolder){
			ofDirectory("CloudsData/").create();
		}
		confirmedDataPath = true;
	}
    return usingDevelopmentFolder ? "../../../CloudsData/" : "CloudsData/";
}

static string GetCloudsVisualSystemDataPath(string systemName, bool ignoredFolder = false){
	//  building from src project file
	string datapath;
	if(ofDirectory("../../../CloudsData/").exists()){
		datapath = string("../../../CloudsData/visualsystems") + (ignoredFolder ? "_ignored" : "") + "/" + systemName + "/";
	}
	//  stand alone full app
	else if(ofDirectory("CloudsData/").exists()){
		datapath =  string("CloudsData/visualsystems") + (ignoredFolder ? "_ignored" : "") + "/" + systemName + "/";
	}
	//  stand alone single app
	else{
		datapath =  "../../../data/";
	}
	
	return datapath;
}

//--------------------------------------------------------------------
static string relinkFilePath(string filePath){
	
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

//--------------------------------------
static inline std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}
