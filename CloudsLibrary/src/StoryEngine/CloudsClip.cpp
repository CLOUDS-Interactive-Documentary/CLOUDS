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
	adjustmentLoaded = false;
	minDepth = 400;
	maxDepth = 1200;

    
}

string CloudsClip::getLinkName(){
	return person + " - " + name;
}

string CloudsClip::getStartingQuestion(){
    if(startingQuestion.empty()){
        return "-";
    }
    else{
    return startingQuestion;
    }
    
}
void CloudsClip::setStartingQuestion(string question){
    startingQuestion = question;
}
bool CloudsClip::hasStartingQuestion(){
    return !startingQuestion.empty();
}
string CloudsClip::getMetaInfo(){
	return clip + ": [" + ofToString(startFrame) + ", " + ofToString(endFrame) + "] fcp id: " + fcpFileId;
}

string CloudsClip::getID(){
	string nameNoSpaces = name;
	ofStringReplace(nameNoSpaces, " ", "_");
	return person + "_" + nameNoSpaces;
}

string CloudsClip::getCombinedPNGExportFolder(){
    //	string nameNoSpaces = name;
    //	ofStringReplace(nameNoSpaces, " ", "_");
	return getID() + "/";
}

string CloudsClip::getCombinedMovieFile(){
	return getID() + ".mov";
}

string CloudsClip::getCombinedCalibrationXML(){
	return getID() + ".xml";
}

string CloudsClip::getFFMpegLine(string _exportFolder){
    
    float frameRate = 23.98;
    float duration = ((float)(endFrame-startFrame))/frameRate;
    
    string dstSound = _exportFolder + "/" +  getID()+".wav";
    
    stringstream pipeline1;
    pipeline1 << "ffmpeg -i \"" << relinkFilePath(sourceVideoFilePath) << "\"";
    pipeline1 << " -ss " << ofToString((float)startFrame/(float)frameRate);
    pipeline1 << " -t " << ofToString(duration);
    pipeline1 << " -ac 2 -ar 44100 -vn \"" << dstSound <<"\"";
    
    stringstream pipeline2;
    pipeline2 << "ffmpeg -start_number " << ofToString(startFrame);
    pipeline2 << " -f image2 -r " << ofToString(frameRate);
    pipeline2 << " -i \"" << _exportFolder << "/" << getCombinedPNGExportFolder() << getID() << "_%05d.png\"";
    pipeline2 << " -i \"" << dstSound << "\" -acodec copy ";
    pipeline2 << " -codec:v libx264 -pix_fmt yuv420p -b 8000k -r 23.976 \"" << _exportFolder << "/" << getCombinedMovieFile() << "\"";
    
    
    stringstream pipeline3;
    pipeline3 << "cp \"" << _exportFolder << "/" << getCombinedPNGExportFolder() << "_calibration.xml\" ";
    pipeline3 << "\""<<_exportFolder << "/" << getCombinedCalibrationXML() << "\"";
    
    return "\n" + pipeline1.str() + "\n" + pipeline2.str() + "\n" + pipeline3.str() + "\n";
}

string CloudsClip::getAdjustmentXML(){
	return ofFilePath::getEnclosingDirectory(ofFilePath::getEnclosingDirectory( ofFilePath::removeExt(relinkFilePath(sourceVideoFilePath)) )) + "adjustment.xml";
}

void CloudsClip::loadAdjustmentFromXML(bool forceReload){
    
	if(adjustmentLoaded && !forceReload){
		return;
	}
	
	ofxXmlSettings adjustmentSettings;
	if(!adjustmentSettings.loadFile(getAdjustmentXML())){
		ofLogError() << "Couldn't load adjustment XML" << getAdjustmentXML() << endl;
	}
	
	adjustTranslate.x = adjustmentSettings.getValue("adjustment:translate:x", 0.);
	adjustTranslate.y = adjustmentSettings.getValue("adjustment:translate:y", 0.);
	adjustTranslate.z = adjustmentSettings.getValue("adjustment:translate:z", 0.);
	
	adjustRotate.x = adjustmentSettings.getValue("adjustment:rotate:x", 0.);
	adjustRotate.y = adjustmentSettings.getValue("adjustment:rotate:y", 0.);
	adjustRotate.z = adjustmentSettings.getValue("adjustment:rotate:z", 0.);
	
	adjustScale.x = adjustmentSettings.getValue("adjustment:scale:x", 1.);
	adjustScale.y = adjustmentSettings.getValue("adjustment:scale:y", 1.);
	
	minDepth = adjustmentSettings.getValue("adjustment:depth:min", 300);
	maxDepth = adjustmentSettings.getValue("adjustment:depth:max", 1200);
	
	contourTargetColor = ofColor(adjustmentSettings.getValue("adjustment:extraction:colorr", 255),
								 adjustmentSettings.getValue("adjustment:extraction:colorg", 255),
								 adjustmentSettings.getValue("adjustment:extraction:colorb", 255));
	contourTargetThreshold = adjustmentSettings.getValue("adjustment:extraction:threshold", 100);
	
	contourMinBlobSize = adjustmentSettings.getValue("adjustment:extraction:blobsize", 100);
	
	cout << "FOR CLIP " << getID() << " LOADED " << contourTargetColor << " target thresh " << contourTargetThreshold << " blob size " << contourMinBlobSize << endl;
	
	adjustmentLoaded = true;
}

void CloudsClip::saveAdjustmentToXML(){
	
	ofxXmlSettings alignmentSettings;
	alignmentSettings.addTag("adjustment");
	alignmentSettings.pushTag("adjustment");
	
	alignmentSettings.addTag("translate");
	alignmentSettings.pushTag("translate");
	alignmentSettings.addValue("x", adjustTranslate.x);
	alignmentSettings.addValue("y", adjustTranslate.y);
	alignmentSettings.addValue("z", adjustTranslate.z);
	alignmentSettings.popTag();
	
	alignmentSettings.addTag("rotate");
	alignmentSettings.pushTag("rotate");
	alignmentSettings.addValue("x", adjustRotate.x);
	alignmentSettings.addValue("y", adjustRotate.y);
	alignmentSettings.addValue("z", adjustRotate.z);
	alignmentSettings.popTag();
	
	alignmentSettings.addTag("scale");
	alignmentSettings.pushTag("scale");
	alignmentSettings.addValue("x", adjustScale.x);
	alignmentSettings.addValue("y", adjustScale.y);
	alignmentSettings.popTag();
	
	alignmentSettings.addTag("depth");
	alignmentSettings.pushTag("depth");
	alignmentSettings.addValue("min", minDepth);
	alignmentSettings.addValue("max", maxDepth);
	alignmentSettings.popTag();

	alignmentSettings.addTag("extraction");
	alignmentSettings.pushTag("extraction");
	alignmentSettings.addValue("colorr", contourTargetColor.r);
	alignmentSettings.addValue("colorg", contourTargetColor.g);
	alignmentSettings.addValue("colorb", contourTargetColor.b);
	alignmentSettings.addValue("threshold", contourTargetThreshold);
	alignmentSettings.addValue("blobsize", contourMinBlobSize);
	
	cout << "FOR CLIP " << getID() << " SAVED " << contourTargetColor << " target thresh " << contourTargetThreshold << " blob size " << contourMinBlobSize << endl;
	
	alignmentSettings.popTag();

	alignmentSettings.popTag();
	
	alignmentSettings.saveFile(getAdjustmentXML());
}

void CloudsClip::addAdjustmentToXML(ofxXmlSettings adjustment){
	
}

string CloudsClip::getSceneFolder(){
	return ofFilePath::getEnclosingDirectory(ofFilePath::getEnclosingDirectory(relinkFilePath(sourceVideoFilePath)));
}


//--------------------------------------------------------------------
string CloudsClip::relinkFilePath(string filePath){
	
	if( !ofFile(filePath).exists() ){
		//		cout << "Switched clip from " << clipFilePath;
        if(ofFile::doesFileExist("/Volumes/Seance/")){
            ofStringReplace(filePath, "Nebula_backup", "Seance");
            ofStringReplace(filePath, "Nebula", "Seance");
        }
        else if(ofFile::doesFileExist("/Volumes/Nebula_helper/")){
            ofStringReplace(filePath, "Nebula_backup", "Nebula_helper");
            ofStringReplace(filePath, "Nebula", "Nebula_helper");
        }
        
        //		cout << " to " << clipFilePath << endl;
	}
	return filePath;
}
