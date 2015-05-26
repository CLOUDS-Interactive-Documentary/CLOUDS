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
ofBuffer& GetFontBuffer();
//--------------------------------------
ofBuffer& GetBoldFontBuffer();
//--------------------------------------
ofBuffer& GetMediumFontBuffer();
//--------------------------------------
ofBuffer& GetThinFontBuffer();
//--------------------------------------
string utf8_substr(string originalString, int maxLength);
