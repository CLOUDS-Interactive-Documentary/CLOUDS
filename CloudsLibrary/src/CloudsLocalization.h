//
//  CloudsLocalization.h
//
//
//

#pragma once

#include "ofMain.h"


//--------------------------------------
bool InitLocalization();
//--------------------------------------
string GetTranslationForString(string toTranslate);
//--------------------------------------
string GetFontPath();
//--------------------------------------
string GetMediumFontPath();
//--------------------------------------
string GetThinFontPath();
//--------------------------------------
string utf8_substr(string originalString, int maxLength);
