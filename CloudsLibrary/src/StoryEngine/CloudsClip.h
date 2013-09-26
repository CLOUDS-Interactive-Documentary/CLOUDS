
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
//#include "ClusterData.h"

class CloudsClip {
  public:
	CloudsClip();
	
	string name;
    string person;
    string clip;

	string getSpeakerLastName();
	string getSpeakerFirstName();
	string getSpeakerGender();
	
	string fcpFileId;
    string sourceVideoFilePath;
	ofColor color;
	
    string startingQuestion;
	
	ofVec3f networkPosition;
	
	//JG took out in replacement for
    //svg data
//    ClusterData cluster;
//	map<string, ofColor> clusterColors;
    
    float currentScore;
    int startFrame;
    int endFrame;

//    vector<string> keywords;
    vector<string>& getOriginalKeywords();
    //used to save out links
    vector<string>& getAdditionalKeywords();
    vector<string>& getRevokedKeywords();
    vector<string>& getAllTopicsWithQuestion();
    
    //use everywhere for real keywoords
	vector<string>& getKeywords();
    
    //get special # keywords
    vector<string>& getSpecialKeywords();

    
    //question topic pairs
	bool hasQuestion();
    map<string,string>& getAllQuestionTopicPairs();
    void addQuestionTopicPair(string topic, string question);
    string getQuestionForTopic(string topic);
    vector<string> getQuestionsVector();
    //called from the FCPParser
    void setOriginalKeywords(vector<string>& keywords);
    //called from the CloudsLinker UI
    void setDesiredKeywords(vector<string>& desiredKeywords);
    
    vector<string> getOverlappingClips();
    bool hasOverlappingClips();
    
    //called during ParseLinks
    void addKeyword(string keyword);
    void revokeKeyword(string keyword);
    bool hasAdditionalKeywords();
    bool hasRevokedKeywords();
    bool hasSpecialKeyword(string keyword);
    bool hasKeyword(string keyword);
    
	float getDuration();
    string getLinkName();
	string getMetaInfo();
    string getStartingQuestion();
	string getSceneFolder();
	void setStartingQuestion(string question);
    bool hasStartingQuestion();
	
	string getID();
	string getCombinedPNGExportFolder();
	string getCombinedMovieFile();
	string getCombinedCalibrationXML();
    string getFFMpegLine(string alternativeVideoPath, string exportFolder);
    
    void addOverlappingClipName(string clipName);
	

	bool hasCombinedVideo;
	string combinedVideoPath;
	string combinedCalibrationXMLPath;
	
	string getRelinkedVideoFilePath();
	
	//adjustment paramters set by the exporter
	string getAdjustmentXML();
	void loadAdjustmentFromXML(bool forceReload = false);
	void saveAdjustmentToXML();
	
	//adjustment parameters
	float minDepth;
	float maxDepth;
	ofVec3f adjustTranslate;
	ofVec3f adjustRotate;
	ofVec3f adjustScale;
	ofVec2f faceCoord;
	
	//contour parameters
	ofColor contourTargetColor;
	float contourTargetThreshold;
	float contourMinBlobSize;
	
	bool adjustmentLoaded;
    
  protected:
    vector<string> originalKeywords;    //came in from FCP
    vector<string> additionalKeywords;  //added manually
    vector<string> revokedKeywords;     //manually removed
    vector<string> specialKeywords;        //special Keywords start with #
    vector<string> topicWithQuestions;
    map<string,string> questionTopicMap; //question-topic pairs
    vector<string> overlappingClips;
    bool keywordsDirty;
    void collateKeywords();
    vector<string> keywords; //collated

};
