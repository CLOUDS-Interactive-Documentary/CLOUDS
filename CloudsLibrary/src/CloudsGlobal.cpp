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

static CloudsVisualLevel visualLevel = PRETTY;
static bool loadedVisualLevel = false;

string dataRootPath = "";
string dataRootPathIgnored = "";
string mediaRootPath = "";

#ifdef TARGET_OSX
#include <wordexp.h>
#endif
//--------------------------------------------------------------------
string GetCloudsDataPath(bool ignored)
{
	if(dataRootPath == ""){

        if(ofDirectory("../../../CloudsData").exists()){
            dataRootPath = "../../../CloudsData/";
            dataRootPathIgnored = "../../../CloudsDataIgnored/";
        }
    }
    
	if(dataRootPath == ""){
        #ifdef TARGET_OSX
        wordexp_t exp_result;
        wordexp("~/Library/Application\\ Support/CLOUDS/CloudsData/", &exp_result, 0);
        string expandedPath = exp_result.we_wordv[0];
        wordfree(&exp_result);
        if(ofFile(expandedPath).exists()){
            //old way
            //dataRootPath = ofFilePath::addTrailingSlash( ofBufferFromFile("/Library/Application Support/CLOUDS/dataRoot.txt").getFirstLine() );
            dataRootPath = expandedPath;
            dataRootPathIgnored = dataRootPath;
        }
        #else
        if(ofFile("C:/Program Files (x86)/CLOUDS/dataRoot.txt").exists()){
            dataRootPath = ofFilePath::addTrailingSlash( ofBufferFromFile("C:/Program Files (x86)/CLOUDS/dataRoot.txt").getFirstLine() );
            dataRootPathIgnored = dataRootPath;
        }
        #endif
	}

	return ignored ? dataRootPathIgnored : dataRootPath;
}



string GetCloudsMediaPath(){

#ifdef VHX_MEDIA
	ofLogError("GetCloudsMediaPath") << "VHX Should never use Media Path!";
#endif
    
	if(mediaRootPath == ""){
		//Prioritize development build over media roots so we don't confuse ourselves if CLOUDS is installed on development machine
		if(ofDirectory("../../../CloudsDataMedia").exists()){
			mediaRootPath = "../../../CloudsDataMedia/";
		}

    }
    
    //not development ...
    if(mediaRootPath == ""){
        string thumbDrive = FindCloudsThumbDrive();

#ifdef TARGET_OSX
		if(thumbDrive == "" && ofFile("/Library/Application Support/CLOUDS/mediaRoot.txt").exists()){
			mediaRootPath = ofFilePath::addTrailingSlash( ofBufferFromFile("/Library/Application Support/CLOUDS/mediaRoot.txt").getFirstLine() );
		}else if(thumbDrive != ""){
			mediaRootPath = thumbDrive;
		}
#else
		if(thumbDrive == "" && ofFile("C:/Program Files (x86)/CLOUDS/mediaRoot.txt").exists()){
			mediaRootPath = ofFilePath::addTrailingSlash( ofBufferFromFile("C:/Program Files (x86)/CLOUDS/mediaRoot.txt").getFirstLine() );
		}else if(thumbDrive != ""){
			mediaRootPath = thumbDrive;
		}
#endif
	}

	ofLogVerbose("GetCloudsMediaPath") << mediaRootPath <<endl;

	return mediaRootPath;

}

void TrimVHXId(string& str){
    ofStringReplace(str, "VO-", "");
    ofStringReplace(str, ".mov", "");
    ofStringReplace(str, ".mp3", "");
    ofStringReplace(str, ".mp4", "");
    ofStringReplace(str, "%2B", "");
    ofStringReplace(str, "%3C", "");
    ofStringReplace(str, "%3F", "");
    ofStringReplace(str, " ", "");
    ofStringReplace(str, "-", "");
    ofStringReplace(str, "_", "");
    ofStringReplace(str, "+", "");
    ofStringReplace(str, ",", "");
    ofStringReplace(str, "?", "");
    ofStringReplace(str, "'", "");
    ofStringReplace(str, "\"", "");
}

//--------------------------------------------------------------------
void ParseVHXIds(const string& path, map<string, string>& idMap){
    ofBuffer buffer = ofBufferFromFile(path);
    
    while(!buffer.isLastLine()){
        
        string line = buffer.getNextLine();
        if(line == ""){
            continue;
        }
        
        vector<string> split = ofSplitString(line,",", true,true);
        string idstring = split[0];
        split.erase(split.begin());
        
        string key = ofJoinString(split, "");
        TrimVHXId(key);
        
        idMap[key] = idstring;
    }
}


//--------------------------------------------------------------------
string GetCloudsVisualSystemDataPath(string systemName, bool ignoredFolder){
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


string FindCloudsThumbDrive(){

#ifdef TARGET_OSX
	ofDirectory dir("/Volumes");
    dir.listDir();
	for(int i = 0; i < dir.size(); i++){
		if(dir.getFile(i).isDirectory()){
			if(ofDirectory::doesDirectoryExist(dir.getPath(i)+"/.cloudsmedia.noindex")){ //JG changed this for mac
				return dir.getPath(i)+"/CloudsDataMedia";
			}
		}
	}
#else
	const int BUFSIZE = MAX_PATH;
	char buffer[ BUFSIZE ];
	DWORD n = GetLogicalDriveStringsA( BUFSIZE+1, buffer);
	DWORD i =  0;

	string drive_name = "";
	while( i < n )   {
		stringstream drive_name_ss;
		int t = GetDriveTypeA( &buffer[i] );
		ofLog() << &buffer[i] << " type is " << t << endl;

		drive_name_ss << &buffer[i];
		drive_name_ss >> drive_name;
		i += strlen( &buffer[i] ) + 1 ;

		if(ofDirectory::doesDirectoryExist(drive_name+"/CloudsDataMedia")){
			ofLog()<<drive_name+"/CloudsDataMedia/"<<endl;
			return drive_name+"/CloudsDataMedia/";
		}
	}
#endif
    
    ofLogError("FindCloudsThumbDrive") << "Couldn't find thumb drive";
    
    return "";
    
}


//bool CheckForUpdates(){
//	ofHttpResponse resp = ofLoadURL("http://www.cloudsdocumentary.com/images/winmacvr.png");
//	if(resp.status <= 200){
//		return false;
//	}else{
//		return true;
//	}
//}

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
	if(!loadedVisualLevel){

		auto visualConfig = ofFile( GetCloudsDataPath()+"/visual_quality_config.txt" );
		auto quality = visualConfig.readToBuffer().getText();

		if( quality == "FAST"){
			visualLevel = FAST;
		}else if( quality == "PRETTY" ){
			visualLevel = PRETTY;
		}else{
			visualLevel = PRETTY;
		}

		loadedVisualLevel = true;
	}
	return visualLevel;
}
