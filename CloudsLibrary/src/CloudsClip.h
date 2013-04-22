
#pragma once

#include "ofMain.h"

class CloudsClip {
  public:
	CloudsClip();
	
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
    vector<string> keywords;
	
    string getLinkName();
	string getMetaInfo();

	//these are filenames
	string getCombinedMovieFile();
	string getCombinedCalibrationXML();
	
	//these are complete file paths, absolute directories
	//this is set by the FCP database parser when the directory is set
	string combinedVideoFilePath;
	string combinedVideoCalibrationXml;
	
};
