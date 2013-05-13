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
	minDepth = 400;
	maxDepth = 1200;
}

string CloudsClip::getLinkName(){
	return person + " - " + name;
}

string CloudsClip::getMetaInfo(){
	return clip + ": [" + ofToString(startFrame) + ", " + ofToString(endFrame) + "] fcp id: " + fcpFileId;
}

string CloudsClip::getCombinedPNGExportFolder(){
	string nameNoSpaces = name;
	ofStringReplace(nameNoSpaces, " ", "_");
	return person + "_" + nameNoSpaces + "/";	
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

string CloudsClip::getSceneFolder(){
	return ofFilePath::getEnclosingDirectory(ofFilePath::getEnclosingDirectory(relinkFilePath(sourceVideoFilePath)));
}

//--------------------------------------------------------------------
string CloudsClip::relinkFilePath(string filePath){
	
	if( !ofFile(filePath).exists() ){
		//		cout << "Switched clip from " << clipFilePath;
		ofStringReplace(filePath, "Nebula_backup", "Seance");
		ofStringReplace(filePath, "Nebula", "Seance");
		//		cout << " to " << clipFilePath << endl;
	}
	return filePath;
}
