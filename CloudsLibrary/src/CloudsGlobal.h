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


