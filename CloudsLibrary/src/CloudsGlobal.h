//
//  CloudsGlobal.h
//  CloudsStoryEngine
//
//  Created by James George on 7/11/13.
//
//

#ifndef CloudsStoryEngine_CloudsGlobal_h
#define CloudsStoryEngine_CloudsGlobal_h

#include "ofMain.h"

static bool confirmedDataPath = false;
static bool usingDevelopmentFolder = false;

//--------------------------------------------------------------------
static string getDataPath()
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

//--------------------------------------------------------------------
static string relinkFilePath(string filePath){
	
	vector<string> drives;
	
	drives.push_back("Seance");
	drives.push_back("Nebula");
	drives.push_back("Supernova");
	drives.push_back("Nebula_helper");
    
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

#endif
