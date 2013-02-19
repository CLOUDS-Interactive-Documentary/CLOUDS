/**
 * CLOUDS FCP Parser
 * Class for parsing a set of FCP XML and presenting keywords
 *
 * Copyright 2013 James George
 */

#pragma once
#include "ofMain.h"
#include "ofxXmlSettings.h"
#include <set>

typedef struct {
    string name;
    string person;
    string clip;
    ofColor color;
    int startFrame;
    int endFrame;
    vector<string> keywords;
} ClipMarker;

class CloudsFCPParser {
  public:
    CloudsFCPParser();
    void setup(string directory);
    
    void refreshXML();
    vector<string>& getAllKeywords();
    vector<ClipMarker>& getAllClips();
    
    int occurrencesOfKeyword(string keyword);
    
  protected:
    string xmlDirectory;
    void addXMLFile(string xmlFile);
    void parseClipItem(ofxXmlSettings& finalCutXml, string xmlName);
    vector<ClipMarker> markers;
    map<string, int> allKeywords;
    vector<string> keywordVector;
    
    bool keywordsDirty;
    void refreshKeywordVector();

};