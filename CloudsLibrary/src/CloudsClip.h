
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
	string combinedVideoFilePath;
	string combinedVideoCalibrationXml;
    ofColor color;
	
	float currentScore;
	
    int startFrame;
    int endFrame;
    vector<string> keywords;
	
    string getLinkName();
	string getMetaInfo();
};
