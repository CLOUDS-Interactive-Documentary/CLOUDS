//
//  CloudsGlobal.h
//  CloudsStoryEngine
//
//  Created by James George on 7/11/13.
//
//

#pragma once

#include "ofMain.h"
#define CLOUDS_VERSION "1.0.0"

#ifdef TARGET_OSX
    #ifdef VHX_MEDIA
        #define CLOUDS_VERSION_KEY "mac.vhx"
    #else
        #define CLOUDS_VERSION_KEY "mac.usb"
    #endif
#else
    #ifdef VHX_MEDIA
        #define CLOUDS_VERSION_KEY "win.vhx"
    #else
        #define CLOUDS_VERSION_KEY "win.usb"
    #endif
#endif

//TEXT WITH NO BACKING
static ofColor CloudsColorTextDeactivated = ofColor::fromHex(0x5583AE, 255 * .75);
static ofColor CloudsColorTextStatic      = ofColor::fromHex(0xFFFFFF);
static ofColor CloudsColorTextHover       = ofColor::fromHex(0x439CED);
static ofColor CloudsColorTextActive      = ofColor::fromHex(0xA3CEF7);
static ofColor CloudsColorTextSelected    = ofColor::fromHex(0x034C89);

//TAB FILL
static ofColor CloudsColorTabFillStatic       = ofColor::fromHex(0x101010, 255 * .6);
static ofColor CloudsColorTabFillHover        = ofColor::fromHex(0x439CED, 255 * .5);
static ofColor CloudsColorTabFillActive       = ofColor::fromHex(0x034C89, 255 * .7);
static ofColor CloudsColorTabFillSelectd      = ofColor::fromHex(0x034C89, 255 * .7);
static ofColor CloudsColorTabStrokeSelectd    = ofColor::fromHex(0xFFFFFF, 255 * .7);

//--------------------------------------------------------------------
enum CloudsVisualLevel { FAST, PRETTY };

//--------------------------------------------------------------------
string GetCloudsDataPath(bool ignored = false);
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
//--------------------------------------------------------------------
//bool CheckForUpdates();
//--------------------------------------------------------------------
string FindCloudsThumbDrive();
//--------------------------------------------------------------------
void ParseVHXIds(const string& path, map<string, string>& idMap);
//--------------------------------------------------------------------
void TrimVHXId(string& str);

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
