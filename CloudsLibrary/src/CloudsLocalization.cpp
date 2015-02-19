//
//  CloudsGlobal.h
//  CloudsStoryEngine
//
//  Created by James George on 7/11/13.
//
//

#pragma once

#include "ofMain.h"
#include "CloudsLocalization.h"
#include "CloudsGlobal.h"
#include "ofxLocalization.h"


//--------------------------------------
static ofxLocalization localization;
static bool localizationLoaded = false;
bool InitLocalization(){
	localizationLoaded = true;
	return localization.load(GetCloudsDataPath() + "language/languagefile.csv");
}

//--------------------------------------
string GetTranslationForString(string toTranslate){
	if(!localizationLoaded){
		InitLocalization();
	}

	return localization.translateKeyToLanguage(toTranslate, GetLanguage());
}

//--------------------------------------
string GetFontPath(){
	if(GetLanguage() == "JAPANESE"){
		return GetCloudsDataPath() + "font/mplus-1c-regular.ttf";
	}
	else{
		return 	GetCloudsDataPath() + "font/Blender-BOOK.ttf";
	}
}
