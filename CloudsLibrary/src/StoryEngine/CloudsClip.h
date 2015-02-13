
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "CloudsProjectExample.h"

class CloudsClip {
  public:

	CloudsClip();
	
	string name;
    string person;
    string clip;

	string getSpeakerLastName();
	string getSpeakerFirstName();
    string getSpeakerFullName();
	string getSpeakerGender();
	string getLanguage();
    float getSpeakerVolume();
	
	string fcpFileId;
    string sourceVideoFilePath;
	
	ofVec3f networkPosition;
	ofVec2f networkPosition2d; //populated by gephi graph

    float speakerVolume;
	bool voiceOverAudio;
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
    void setProjectExample(string projectExample);
    void setSpeakerVolume(float vol);
	
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
	bool isPartOfSeries();
	
    map<string,string>& getAllQuestionTopicPairs();
    string getQuestionForTopic(string topic);
	vector<string> getTopicsWithQuestions();
    vector<string> getQuestions();
	
	//project example stuff
	bool hasProjectExample;
	string projectExampleTitle;
	CloudsProjectExample projectExample;

    //overlapping clips
    void addOverlappingClip(CloudsClip* clip);
	void addOverlappingClipID(string clipID);
//	void removeOverlappingClipName(string clipName);
    vector<string>& getOverlappingClipIDs();
    bool hasOverlappingClips();
    bool overlapsWithClip(CloudsClip* clip);
    bool overlapsWithClipID(string clipID);
	
	bool is30FPS();
	float getDuration();
    string getLinkName();
	string getMetaInfo();
	string getSceneFolder();
	
	string getID();
	string getCombinedPNGExportFolder();
	string getCombinedMovieFile();
	string getCombinedCalibrationXML();
    string getSubtitlesPath();
    string getFFMpegLine(string alternativeVideoPath, string exportFolder);
    
	bool hasSubtitleFile();
	bool hasMediaAsset;
	string combinedVideoPath;
	string voiceOverAudioPath;
	string combinedCalibrationXMLPath;
    string subtitlePath;
	
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
    
    //Skin Parameters
    ofFloatColor skinTargetColor;
	
    float skinLowerThreshold;
    float skinUpperThreshold;
    float skinHueWeight;
    float skinSatWeight;
    float skinBrightWeight;
	
	bool adjustmentLoaded;
    
  protected:
    vector<string> originalKeywords;    //came in from FCP
    vector<string> additionalKeywords;  //added manually
    vector<string> revokedKeywords;     //manually removed
    vector<string> specialKeywords;        //special Keywords start with #
    vector<string> topicWithQuestions;
    map<string,string> questionTopicMap; //question-topic pairs
    vector<string> overlappingClipIDs;
    bool keywordsDirty;
    void collateKeywords();
    vector<string> keywords; //collated

};
