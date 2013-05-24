
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class CloudsClip {
  public:
	CloudsClip();
	
	static string relinkFilePath(string filePath);
    
	string name;
    string person;
    string clip;
	
	string fcpFileId;
    string sourceVideoFilePath;
	

    ofColor color;
	
	float currentScore;
	
    int startFrame;
    int endFrame;
	
    vector<string> keywords;
	
    string getLinkName();
	string getMetaInfo();

	string getSceneFolder();
	
	//these are filenames
	string getID();
	string getCombinedPNGExportFolder();
	string getCombinedMovieFile();
	string getCombinedCalibrationXML();
    string getFFMpegLine(string _exportFolder);
	
	bool hasCombinedVideo;
	string combinedVideoPath;
	string combinedCalibrationXMLPath;
	
	string getAdjustmentXML();
	void loadAdjustmentFromXML();
	void saveAdjustmentToXML();
	void addAdjustmentToXML(ofxXmlSettings adjustment);
	
	//adjustment parameters
	float minDepth;
	float maxDepth;
	ofVec3f adjustTranslate;
	ofVec3f adjustRotate;
	ofVec3f adjustScale;
	
};
