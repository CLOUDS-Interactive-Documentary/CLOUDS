//
//  CloudsGlobal.h
//  CloudsStoryEngine
//
//  Created by James George on 7/11/13.
//
//



#include "ofMain.h"
#include "CloudsGlobal.h"
#include "Poco/Environment.h"
bool confirmedDataPath = false;
bool usingDevelopmentFolder = false;
CloudsVisualLevel visualLevel = FAST;
string dataRootPath = "";
string mediaRootPath = "";
//--------------------------------------------------------------------
string GetCloudsDataPath(bool ignored)
{
    if(!ignored){
        return GetCloudsDataRootPath();
    }else{
        return GetCloudsMediaPath();
    }
}

string GetEnvVar( string  key ) {
    string val;
    val =  Poco::Environment::get(key, "");
    if(val == ""){
#ifdef TARGET_OSX
        cout<<"loading system env"<<endl;
        ofSystem("eval $(launchctl export)");
#endif
    }
    
    return val;
}

string GetCloudsMediaPath(){
    
    string path;
    if(mediaRootPath == "" && ofFile::doesFileExist("/Library/Application Support/CLOUDS/mediaRoot.txt")){
        ofFile file;
        file.open("/Library/Application Support/CLOUDS/mediaRoot.txt", ofFile::ReadOnly);
        ofBuffer buff = file.readToBuffer();
        mediaRootPath = buff.getFirstLine();
       
    }
    if(ofDirectory::doesDirectoryExist("../../../CloudsData_ignored/")){
        mediaRootPath ="../../../CloudsData_ignored/";
        return mediaRootPath;
        
    }
    ofLog(OF_LOG_VERBOSE)<<mediaRootPath<<endl;
    return mediaRootPath;
}

string GetCloudsDataRootPath(){
    
    string path;
    if(dataRootPath == "" && ofFile::doesFileExist("/Library/Application Support/CLOUDS/dataRoot.txt")){
        ofFile file;
        file.open("/Library/Application Support/CLOUDS/dataRoot.txt", ofFile::ReadOnly);
        ofBuffer buff = file.readToBuffer();
        dataRootPath = buff.getFirstLine();
    }
    if(ofDirectory::doesDirectoryExist("../../../CloudsData")){
        dataRootPath = "../../../CloudsData/";
        
    }
    ofLog(OF_LOG_VERBOSE)<<dataRootPath<<endl;
    return dataRootPath;
}

//--------------------------------------------------------------------
string GetCloudsVisualSystemDataPath(string systemName, bool ignoredFolder){
    //  building from src project file
    string datapath;
    //	if(ofDirectory("../../../CloudsData/").exists()){
    //		datapath = string("../../../CloudsData") + (ignoredFolder ? "_ignored" : "") + "/visualsystems/" + systemName + "/";
    //	}
    //	//  stand alone full app
    //	else if(ofDirectory("CloudsData/").exists()){
    //		datapath =  string("CloudsData") + (ignoredFolder ? "_ignored" : "") + "/visualsystems/" + systemName + "/";
    //	}
    //	//  stand alone single app
    //	else{
    //		datapath =  "../../../data/";
    //	}
    
    
    
    return GetCloudsDataPath(ignoredFolder)+"visualsystems/" + systemName + "/";
    
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

//--------------------------------------------------------------------
CloudsVisualLevel getVisualLevel(){
    return visualLevel;
}
