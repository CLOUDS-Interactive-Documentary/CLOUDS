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

CloudsVisualLevel visualLevel = PRETTY;

string dataRootPath = "";
string dataRootPathIgnored = "";
string mediaRootPath = "";

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

//--------------------------------------------------------------------
string GetCloudsDataPath(bool ignored)
{

    if(dataRootPath == ""){
        //Prioritize development build over media roots so we don't confuse ourselves if CLOUDS is installed on development machine
        if(ofDirectory("../../../CloudsData").exists()){
            dataRootPath = "../../../CloudsData/";
            dataRootPathIgnored = "../../../CloudsDataIgnored/";
        }
        //temporary build folder
        else if(ofDirectory("data/CloudsData").exists()){
            dataRootPath = "CloudsData/";
            dataRootPathIgnored = "CloudsDataIgnored/";
        }
        //installed
        #ifdef TARGET_OSX
        else if(ofFile("/Library/Application Support/CLOUDS/dataRoot.txt").exists()){
            //JG TO DM: Note thate these are teh same. Let's combine ignored data into the main data folder for release.
            dataRootPath = ofFilePath::addTrailingSlash( ofBufferFromFile("/Library/Application Support/CLOUDS/dataRoot.txt").getFirstLine() );
            dataRootPathIgnored = dataRootPath;
        }
        #else
        //TODO: Windows data path
        #endif
    }
    
    return ignored ? dataRootPathIgnored : dataRootPath;
}

string GetCloudsMediaPath(){
#ifdef VHX_MEDIA
    ofLogError("GetCloudsMediaPath") << "VHX Should never use Media Path!";
#else
    if(mediaRootPath == ""){
        //Prioritize development build over media roots so we don't confuse ourselves if CLOUDS is installed on development machine
        if(ofDirectory("../../../CloudsDataMedia").exists()){
            mediaRootPath = "../../../CloudsDataMedia/";
        }
        //temporary build folder
        else if(ofDirectory("CloudsDataMedia/").exists()){
            mediaRootPath = "CloudsDataMedia/";
        }
#ifdef TARGET_OSX
        else if(ofFile("/Library/Application Support/CLOUDS/mediaRoot.txt").exists()){
            mediaRootPath = ofFilePath::addTrailingSlash( ofBufferFromFile("/Library/Application Support/CLOUDS/mediaRoot.txt").getFirstLine() );
        }
#else
        //TODO Windows Media Path Root
#endif
    }
    
    ofLogVerbose("GetCloudsMediaPath") << mediaRootPath <<endl;
    
    return mediaRootPath;
#endif //end VHX
}

//string GetCloudsDataRootPath(bool ignored){
//    
//
//}

//--------------------------------------------------------------------
string GetCloudsVisualSystemDataPath(string systemName, bool ignoredFolder){
    //  building from src project file
    //string datapath;
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
    
    return GetCloudsDataPath(ignoredFolder) + "visualsystems/" + systemName + "/";
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
