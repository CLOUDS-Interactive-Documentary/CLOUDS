
#pragma once

#include "ofMain.h"

class CloudsClip {
  public:
	CloudsClip();
	
	static string relinkFilePath(string filePath);
    
	string name;
    string person;
    string clip;
	
	string fcpFileId;
    string sourceVideoFilePath;
	
	bool hasCombinedVideo;
    ofColor color;
	
	float currentScore;
	
    int startFrame;
    int endFrame;
	
	//adjustment parameters
	float minDepth;
	float maxDepth;
	
    vector<string> keywords;
	
    string getLinkName();
	string getMetaInfo();

	string getSceneFolder();
	
	//these are filenames
	string getCombinedPNGExportFolder();
	
	string getCombinedMovieFile();
	string getCombinedCalibrationXML();
	
	//these are complete file paths, absolute directories
	//this is set by the FCP database parser when the directory is set
	string combinedVideoFilePath;
	string combinedVideoCalibrationXml;
	
	
};
