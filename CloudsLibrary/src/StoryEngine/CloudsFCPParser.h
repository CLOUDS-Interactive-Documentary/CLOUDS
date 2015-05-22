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
    void setup(const string& directory);
    void refreshXML();
	void loadMediaAssets();//default combine directory
	//void setCombinedVideoDirectory(const string& directory);
	bool printErrors;

#pragma mark Clips
	bool hasClipWithLinkName(const string& linkname);
	bool hasClipWithID(const string& ID);
	CloudsClip* getClipWithLinkName( const string& linkname );
	CloudsClip* getClipWithLinkName( const string& linkname, bool& clipFound );
	CloudsClip* getClipWithID( const string& ID );
	CloudsClip* getClipWithID( const string& ID, bool& clipFound );

#pragma mark Links
	void parseVOClips();
    void parseSpeakersVolume();
    void parseLinks(const string& linkFile);
	void parseClusterNetwork(const string& fileName);
	void parseProjectExamples(const string& filename);
    void parseTopicAssociations(const string& filename);
    
#ifdef VHX_MEDIA
    //void trimVHXId(string& str);
    //void parseVHXIds(const string& filename, map<string, string>& idMap);
    void mapVHXMedia();
#endif
    
	vector<CloudsProjectExample>& getProjectExamples();
	CloudsProjectExample& getProjectExampleWithTitle(const string& title);
	
	void getOverlappingClipIDs();
    void autolinkSequentialClips();
    
    map<string,string> cloudsClipToFileID;
    map<string,vector<CloudsClip*> > fileIDtoCloudsClips;
    map<string,vector<string> > overlappingClipsMap;
    
	vector<CloudsLink>& getLinksForClip(CloudsClip* clip);
    vector<CloudsLink>& getLinksForClip(const string& clipName);
	vector<CloudsLink>& getSuppressionsForClip(CloudsClip* clip);
    vector<CloudsLink>& getSuppressionsForClip(const string& clipName);

	void addLink(const string& sourceName, const string& targetName);
	void addLink(CloudsClip* source, CloudsClip* target);
    void addLink(CloudsLink& link);
	void setSpeakerVolume(const string& speaker, float vol);
    float getSpeakerVolume(const string& speakerFullName);
    
    void removeLink(const string& linkName, int linkIndex);
	void removeLink(const string& linkName, const string& targetName);
	void saveLinks();

	void suppressConnection(const string& sourceName, const string& targetName);
	void suppressConnection(CloudsClip* source, CloudsClip* target);
	void suppressConnection(CloudsLink& link);
    void unsuppressConnection(const string& linkName, int linkIndex);
	void unsuppressConnection(const string& linkName, const string& targetName);
	void unsuppressConnection(CloudsLink& link);
    void refreshAllKeywords();
	void printSpeakerList();
	void printDichotomyRatios();
    
    void addIntervention(const string& clipName, const string& interventionName);
	bool clipHasIntervention(const string& clipName);
	
	//QUERIES
	//true if A has any out going links at all
	bool clipHasLink(CloudsClip* clip);
	bool clipHasLink(const string& clipName);
	//true if A links to B
    bool clipLinksTo(const string& clipNameA, const string& clipNameB);
	//true if A links to B or B links to A and the index of the link
	bool clipLinksTo(const string& clipNameA, const string& clipNameB, int& index);
	//true if A links to B or B links to A
	bool clipsShareLink(const string& clipNameA, const string& clipNameB);
	//true if A and B have clips that link to one another
	bool keywordsShareLink(const string& keyA, const string& keyB);
	
	//are there any clips that this suppresses?
	bool clipHasSuppressions(CloudsClip* clip);
	bool clipHasSuppressions(string clipName);
	
    //are there any revoked or additional toolkits?
    bool clipHasRevokedKeywords(CloudsClip* clip);
    bool clipHasAdditionalKeywords(CloudsClip* clip);
    
	//old way
    //are there any starting Questions?
    bool clipHasStartingQuestions(CloudsClip* clip);
    bool clipHasStartingQuestions(const string& clipName);
    
	//are we suppressing this connection?
	bool linkIsSuppressed(const string& clipNameA, const string& clipNameB);
	bool linkIsSuppressed(const string& clipNameA, const string& clipNameB, int& index);
	
	float percentOfClipsLinked();
	float getAllClipDuration();
	
#pragma mark Keywords
    void sortKeywordsByOccurrence(bool byOccurrence);
    vector<string>& getAllKeywords();
    vector<CloudsClip*>& getAllClips();
	CloudsClip* getRandomClip(bool hasMediaAsset = false,
							  bool hasQuestion = false,
							  bool hasStartQuestion = false);
	int getNumberOfClipsWithKeyword(const string& filterWord);
	vector<CloudsClip*> getClipsWithKeyword(const string& filterWord, bool useMasterTopics = false);
	vector<CloudsClip*> getClipsWithKeyword(const string& filterWord, vector<CloudsClip*>& searchClips, bool useMasterTopics = false);
    vector<CloudsClip*> getClipsWithKeyword(const vector<string>& filter);
	vector<CloudsClip*> getClipsWithKeyword(const vector<string>& filter, vector<CloudsClip*>& searchClips);
    vector<CloudsClip*> getClipsWithQuestionsForTopic(const string&  topic);
	vector<CloudsClip*> getClipsForPerson(string person);
    
    set<string> getRelatedKeywords(const string&  filterWord);
	int getNumberOfSharedKeywords(CloudsClip* a, CloudsClip* b);
	
	vector<string> getSharedKeywords(CloudsClip* a, CloudsClip* b);
	int getNumberOfSharedClips(const string& keywordA, const string& keywordB);
	
	vector<CloudsClip*> getSharedClips(const string&  keywordA, const string&  keywordB);
	vector<CloudsClip*> getMetaDataConnections(CloudsClip* source);
	int getNumMetaDataConnections(CloudsClip* source);
	
    bool operator()(const string& a, const string& b);
    
    int occurrencesOfKeyword(const string& keyword);
    vector<string>& getContentKeywords();
	vector<string>& getKeywordFamily(const string&  keyword);
	//ofVec3f getKeywordCentroid(const string& keyword);
    
    //master topics are fit to be shown to viwers and contain many keywords as subtopics
	set<string>& getMasterTopics();
    bool isMasterKeyword(const string& keyword);
    bool hasMasterTopicAssociation(const string& keyword);
    string getMasterKeyword(const string& keyword);
	ofVec3f getMasterTopicPosition(const string& keyword);
    
    void saveInterventions(const string&  interventionsFile);
	void saveSpeakersVolume(const string&  speakerVolFile);
    
#pragma mark key themes
	string closestKeyThemeToTag(const string&  searchTag);
	
	set<string> clusterMapColors;
    //vector<string> getAdjacentKeywords(const string&  currentKeyword, int numOfDesiredKeywords);
    //float getCohesionIndexForKeyword(const string&  keyword);
    //float getDistanceFromAdjacentKeywords(const string&  keyword1, const string&  keyword2);
    
#pragma mark Sound
    void parseSounds(map<string, CloudsMedia *>& sounds);
    
  protected:
    void reciprocateSuppressions(CloudsClip* clip );
    
    string xmlDirectory;
    string combinedVideoDirectory;
    
	bool sortedByOccurrence;
	
    void addXMLFile(const string&  xmlFile);
    void parseClipItem(ofxXmlSettings& finalCutXml, const string&  xmlName);
   
    map<string, string> fileIdToPath;
    map<string, string> fileIdToName;

    set<string> markerLinkNames;
	set<string> speakerFcpIds;
    
    vector<CloudsClip*> allClips;
    map<string, int> clipIDToIndex;
    map<string, int> clipLinkNameToIndex;
    map<string, vector<int> > questionTopicstoClipIndex;
    
    map<string, int> allKeywords;
    map<string, int> contentKeywords;

    map<string,string> masterTopicAssociations; //go from subtopic -> master topic
    map<string,vector<string> > masterTopicList; //go from master topic -> list of subtopics
    map<string, int> masterTopicClipCount;
    set<string> masterTopicSet; //all user facing topics
    map<string, ofVec3f> masterTopicCentroids;

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
	//map<string, vector<string> > keywordAdjacency;
	map<string, vector<string> > keywordFamilies;
    //map< string, int> keywordCentroidIndex;
    //map<string, float> keywordCohesionMap;
    void populateKeywordCentroids();
    //void calculateCohesionMedianForKeywords();
    //int getCentroidMapIndex(const string&  keyword);
	//void calculateKeywordAdjascency();
	void calculateKeywordFamilies();
    void disperseUnpositionedClips();
    
	CloudsProjectExample dummyProjectExample;
    CloudsClip dummyClip; // for failed reference returns
	float lastBackupTime;
	float backupTimeInterval;
};
