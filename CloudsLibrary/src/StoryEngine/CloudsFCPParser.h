/**
 * CLOUDS FCP Parser
 * Class for parsing a set of FCP XML and presenting keywords
 *
 * Copyright 2013 James George
 */

#pragma once
#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "CloudsClip.h"
#include "CloudsLink.h"
#include <set>
#include "ofRange.h"
#include "CloudsProjectExample.h"

class CloudsFCPParser {
  public:

	CloudsFCPParser();

	void loadFromFiles();
    void setup(string directory);
    void refreshXML();
	void setCombinedVideoDirectory(string directory);
	bool printErrors;

#pragma mark Clips
	bool hasClipWithLinkName(string linkname);
	bool hasClipWithID(string ID);
	CloudsClip& getClipWithLinkName( string linkname );
	CloudsClip& getClipWithLinkName( string linkname, bool& clipFound );
	CloudsClip& getClipWithID( string ID );
	CloudsClip& getClipWithID( string ID, bool& clipFound );

#pragma mark Links
	void parseVOClips();
    void parseSpeakersVolume();
    void parseLinks(string linkFile);
	void parseClusterNetwork(string fileName);
	void parseProjectExamples(string filename);
	vector<CloudsProjectExample>& getProjectExamples();
	CloudsProjectExample& getProjectExampleWithTitle(string title);
	
	void getOverlappingClipIDs();
    void autolinkSequentialClips();
    
    map<string,string> cloudsClipToFileID;
    map<string,vector<CloudsClip> > fileIDtoCloudsClips;
    map<string,vector<string> > overlappingClipsMap;
    
	vector<CloudsLink>& getLinksForClip(CloudsClip& clip);
    vector<CloudsLink>& getLinksForClip(string clipName);
	vector<CloudsLink>& getSuppressionsForClip(CloudsClip& clip);
    vector<CloudsLink>& getSuppressionsForClip(string clipName);

	void addLink(string sourceName, string targetName);
	void addLink(CloudsClip& source, CloudsClip& target);
    void addLink(CloudsLink& link);
	void setSpeakerVolume(string speaker, float vol);
    float getSpeakerVolume(string speakerFullName);
    
    void removeLink(string linkName, int linkIndex);
	void removeLink(string linkName, string targetName);
	void saveLinks(string linkFile);

	void suppressConnection(string sourceName, string targetName);
	void suppressConnection(CloudsClip& source, CloudsClip& target);
	void suppressConnection(CloudsLink& link);
    void unsuppressConnection(string linkName, int linkIndex);
	void unsuppressConnection(string linkName, string targetName);
	void unsuppressConnection(CloudsLink& link);
    void refreshAllKeywords();
	void printSpeakerList();
	void printDichotomyRatios();
    
    void addIntervention(string clipName, string interventionName);
	bool clipHasIntervention(string clipName);
	
	//QUERIES
	//true if A has any out going links at all
	bool clipHasLink(CloudsClip& clip);
	bool clipHasLink(string clipName);
	//true if A links to B
    bool clipLinksTo(string clipNameA, string clipNameB);
	//true if A links to B or B links to A and the index of the link
	bool clipLinksTo(string clipNameA, string clipNameB, int& index);
	//true if A links to B or B links to A
	bool clipsShareLink(string clipNameA, string clipNameB);
	//true if A and B have clips that link to one another
	bool keywordsShareLink(string keyA, string keyB);
	
	//are there any clips that this suppresses?
	bool clipHasSuppressions(CloudsClip& clip);
	bool clipHasSuppressions(string clipName);
	
    //are there any revoked or additional toolkits?
    bool clipHasRevokedKeywords(CloudsClip& clip);
    bool clipHasAdditionalKeywords(CloudsClip& clip);
    
	//old way
    //are there any starting Questions?
    bool clipHasStartingQuestions(CloudsClip& clip);
    bool clipHasStartingQuestions(string clipName);
    
	//are we suppressing this connection?
	bool linkIsSuppressed(string clipNameA, string clipNameB);
	bool linkIsSuppressed(string clipNameA, string clipNameB, int& index);
	
	float percentOfClipsLinked();
	float getAllClipDuration();
	
#pragma mark Keywords
    void sortKeywordsByOccurrence(bool byOccurrence);
    vector<string>& getAllKeywords();
    vector<CloudsClip>& getAllClips();
	CloudsClip& getRandomClip(bool hasMediaAsset = false,
							  bool hasQuestion = false,
							  bool hasStartQuestion = false);
	int getNumberOfClipsWithKeyword(string filterWord);
	vector<CloudsClip> getClipsWithKeyword(string filterWord);
	vector<CloudsClip> getClipsWithKeyword(string filterWord, vector<CloudsClip>& searchClips);
    vector<CloudsClip> getClipsWithKeyword(const vector<string>& filter);
	vector<CloudsClip> getClipsWithKeyword(const vector<string>& filter, vector<CloudsClip>& searchClips);
    vector<CloudsClip> getClipsWithQuestionsForTopic(string topic);
	
    set<string> getRelatedKeywords(string filterWord);
	int getNumberOfSharedKeywords(CloudsClip& a, CloudsClip& b);
	
	vector<string> getSharedKeywords(CloudsClip& a, CloudsClip& b);
	int getNumberOfSharedClips(string keywordA, string keywordB);
	
	vector<CloudsClip> getSharedClips(string keywordA, string keywordB);
	vector<CloudsClip> getMetaDataConnections(CloudsClip& source);
	int getNumMetaDataConnections(CloudsClip& source);
	
    int occurrencesOfKeyword(string keyword);
    bool operator()(const string& a, const string& b);
    vector<string>& getContentKeywords();
	vector<string>& getKeywordFamily(string keyword);
      ofVec2f getKeywordCentroid(string keyword);
	
    void saveInterventions(string interventionsFile);
	void saveSpeakersVolume(string speakerVolFile);
    
#pragma mark key themes
	string closestKeyThemeToTag(string searchTag);
	
	set<string> clusterMapColors;
    vector<string> getAdjacentKeywords( string currentKeyword, int numOfDesiredKeywords);
    float getCohesionIndexForKeyword(string keyword);
    float getDistanceFromAdjacentKeywords(string keyword1, string keyword2);
    
//    void saveClusterMap(map<string, ofVec2f> centroidMap);
  protected:
    
    void reciprocateSuppressions(CloudsClip& clip );
    
    string xmlDirectory;
    string combinedVideoDirectory;
    
	bool sortedByOccurrence;
	
    void addXMLFile(string xmlFile);
    void parseClipItem(ofxXmlSettings& finalCutXml, string xmlName);
   
    map<string, string> fileIdToPath;
    map<string, string> fileIdToName;

    set<string> markerLinkNames;
	set<string> speakerFcpIds;
    
    vector<CloudsClip> allClips;
    map<string, int> clipIDToIndex;
    map<string, int> clipLinkNameToIndex;
    map<string, vector<int> > questionTopicstoClipIndex;
    
    map<string, int> allKeywords;
    map<string, int> contentKeywords;
    
    vector<string> keywordVector;
    vector<string> contentKeywordVector;
	
	vector<int> questionIndeces;
	vector<int> startQuestionIndeces;
	vector<int> hasMediaAssetIndeces;
	vector<int> hasMediaAssetAndQuestionIndeces;
	vector<int> hasCombinedAndIsStartingClipIndeces;
	
    map<string, vector<CloudsLink> > linkedConnections;
	map<string, vector<CloudsLink> > suppressedConnections;
	map<string, vector<string> > sourceSupression;
    map<string, string> clipInterventions;
    map<string, float> speakerVolumes;
    
	//PROJECT EXAMPLES
	vector<CloudsProjectExample> projectExamples;
	map<string,int> clipIdToProjectExample;
	
	//KEYWORDS + CLUSTER NETWORK
    vector<pair<string, ofVec3f> > keywordCentroids;
	map<string, vector<string> > keywordAdjacency;
	map<string, vector<string> > keywordFamilies;
    map<string, int> keywordCentroidIndex;
    map<string, float> keywordCohesionMap;
    void populateKeywordCentroids();
    void calculateCohesionMedianForKeywords();
    int getCentroidMapIndex(string keyword);
	void calculateKeywordAdjascency();
	void calculateKeywordFamilies();

    
	CloudsProjectExample dummyProjectExample;
    CloudsClip dummyClip; // for failed reference returns
	float lastBackupTime;
	float backupTimeInterval;
};
