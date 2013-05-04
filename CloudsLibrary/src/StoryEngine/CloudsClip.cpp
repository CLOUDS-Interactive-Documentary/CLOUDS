//
//  CloudsClip.cpp
//  CLOUDS
//
//  Created by James George on 4/19/13.
//
//

#include "CloudsClip.h"

CloudsClip::CloudsClip(){
	currentScore = 0;
	startFrame = 0;
    endFrame = 0;
	hasCombinedVideo = false;
}

string CloudsClip::getLinkName(){
	return person + " - " + name;
}

string CloudsClip::getMetaInfo(){
	return clip + ": [" + ofToString(startFrame) + ", " + ofToString(endFrame) + "] fcp id: " + fcpFileId;
}

string CloudsClip::getCombinedMovieFile(){
	string nameNoSpaces = name;
	ofStringReplace(nameNoSpaces, " ", "_");
	return person + "_" + nameNoSpaces + ".mov";
}

string CloudsClip::getCombinedCalibrationXML(){
	string nameNoSpaces = name;
	ofStringReplace(nameNoSpaces, " ", "_");
	return person + "_" + nameNoSpaces + ".xml";
}