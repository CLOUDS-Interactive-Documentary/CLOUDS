
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ClusterData.h"

class CloudsClip {
  public:
	CloudsClip();
	
	static string relinkFilePath(string filePath);
    
	string name;
    string person;
    string clip;
	
	string fcpFileId;
    string sourceVideoFilePath;
	ofColor color; //clip marker color
    string startingQuestion;
	
    //svg data
    ClusterData cluster;
	map<string, ofColor> clusterColors;
    
    
    float currentScore;
    int startFrame;
    int endFrame;
	
    vector<string> keywords;
	
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
    string getFFMpegLine(string _exportFolder);
	
	bool hasCombinedVideo;
	string combinedVideoPath;
	string combinedCalibrationXMLPath;
	
	//adjustment paramters set by the exporter
	string getAdjustmentXML();
	void loadAdjustmentFromXML(bool forceReload = false);
	void saveAdjustmentToXML();
	void addAdjustmentToXML(ofxXmlSettings adjustment);
	
	//adjustment parameters
	float minDepth;
	float maxDepth;
	ofVec3f adjustTranslate;
	ofVec3f adjustRotate;
	ofVec3f adjustScale;

	//contour parameters
	ofColor contourTargetColor;
	float contourTargetThreshold;
	float contourMinBlobSize;
	
	bool adjustmentLoaded;
};
