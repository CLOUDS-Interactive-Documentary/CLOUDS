//
//  CloudsGlobal.h
//  CloudsStoryEngine
//
//  Created by James George on 7/11/13.
//
//

#pragma once

#include "ofMain.h"

enum CloudsVisualLevel { FAST, PRETTY };
//--------------------------------------------------------------------
string GetCloudsDataPath(bool ignored = false);
//--------------------------------------------------------------------
string GetEnvVar( string key );
//--------------------------------------------------------------------
string GetCloudsMediaPath();
//--------------------------------------------------------------------
string GetCloudsDataRootPath();
//--------------------------------------------------------------------
string GetCloudsVisualSystemDataPath(string systemName, bool ignoredFolder = false);
//--------------------------------------------------------------------
string GetLanguage();
//--------------------------------------------------------------------
void SetLanguage(string language);
//--------------------------------------------------------------------
string relinkFilePath(string filePath);
//--------------------------------------------------------------------
CloudsVisualLevel getVisualLevel();

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
