
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

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
	
    float currentScore;
    int startFrame;
    int endFrame;
	
    //called from the FCPParser
    void setOriginalKeywords(vector<string>& keywords);
    //called from the CloudsLinker UI
    void setDesiredKeywords(vector<string>& desiredKeywords);
    //called during ParseLinks
    void addKeyword(string keyword);
    void revokeKeyword(string keyword);
    bool hasAdditionalKeywords();
    bool hasRevokedKeywords();
    bool hasSpecialKeyword(string keyword);
    bool hasKeyword(string keyword);
    
    vector<string>& getOriginalKeywords();
    //used to save out links
    vector<string>& getAdditionalKeywords();
    vector<string>& getRevokedKeywords();
    vector<string>& getAllTopicsWithQuestion();

    //use everywhere for real keywoords
	vector<string>& getKeywords();
    //get special # keywords
    vector<string>& getSpecialKeywords();
    
    //questions with topics
    bool hasStartingQuestion();
    void addQuestionTopicPair(string topic, string question);
	bool hasQuestion();
    map<string,string>& getAllQuestionTopicPairs();
    string getQuestionForTopic(string topic);
	vector<string> getTopicsWithQuestions();
    vector<string> getQuestions();
	
    //overlapping clips
    vector<string> getOverlappingClips();
    bool hasOverlappingClips();
    

	float getDuration();
    string getLinkName();
	string getMetaInfo();
//    string getStartingQuestion();
	string getSceneFolder();
	
	string getID();
	string getCombinedPNGExportFolder();
	string getCombinedMovieFile();
	string getCombinedCalibrationXML();
    string getFFMpegLine(string alternativeVideoPath, string exportFolder);
    
    void addOverlappingClipName(string clipName);
	void removeOverlappingClipName(string clipName);

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
