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


class ClipMarker {
	
  public:
	
    string name;
    string person;
    string clip;
    string filePath;
	string fcpFileId;
    ofColor color;
    int startFrame;
    int endFrame;
    vector<string> keywords;

    string getLinkName(){
        return person + " - " + name;
    }
    
    string getMetaInfo(){
        return clip + ": [" + ofToString(startFrame) + ", " + ofToString(endFrame) + "] fcp id: " + fcpFileId;
    }
};

typedef struct {
    string sourceName;
    string targetName;
    int startFrame;
    int endFrame;
} CloudsLink;

class CloudsFCPParser {
  public:
    CloudsFCPParser();
    void setup(string directory);
    void refreshXML();
#pragma mark Clips
	ClipMarker getClipWithLinkName( string linkname );
	
#pragma mark Links
	//MANAGE
    void parseLinks(string linkFile);
	vector<CloudsLink>& getLinksForClip(ClipMarker& clip);
    vector<CloudsLink>& getLinksForClip(string clipName);
    void addLink(CloudsLink& link);
    void saveLinks(string linkFile);
    void removeLink(string linkName, int linkIndex);
	
	//QUERIES
	//true if A links to B
    bool clipLinksTo(string clipNameA, string clipNameB);
	//true if A links to B or B links to A
	bool clipsShareLink(string clipNameA, string clipNameB);
	//true if A and B have clips that link to one another
	bool keywordsShareLink(string keyA, string keyB);
	
#pragma mark Keywords
    void sortKeywordsByOccurrence(bool byOccurrence);
    vector<string>& getAllKeywords();
    vector<ClipMarker>& getAllClips();
	vector<ClipMarker> getClipsWithKeyword(string filterWord);
    vector<ClipMarker> getClipsWithKeyword(const vector<string>& filter);
    set<string> getRelatedKeywords(string filterWord);
	vector<string> getSharedKeywords(ClipMarker& a, ClipMarker& b);
	int getNumberOfSharedClips(string keywordA, string keywordB);
	
	vector<ClipMarker> getSharedClips(string keywordA, string keywordB);
	
    int occurrencesOfKeyword(string keyword);
    bool operator()(const string& a, const string& b);
    
  protected:
    string xmlDirectory;
    void addXMLFile(string xmlFile);
    void parseClipItem(ofxXmlSettings& finalCutXml, string xmlName);

    map<string, string> fileIdToPath;
    map<string, string> fileIdToName;
    vector<ClipMarker> markers;
	set<string> markerLinkNames;

    map<string, int> allKeywords;
    vector<string> keywordVector;

    map<string, vector<CloudsLink> > sourceLinks;

    bool keywordsDirty;
    void refreshKeywordVector();
    bool sortedByOccurrence;
    
};