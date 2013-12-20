//
//  CloudsClip.cpp
//  CLOUDS
//
//  Created by James George on 4/19/13.
//
//

#include "CloudsClip.h"
#include "CloudsGlobal.h"
#include "CloudsSpeaker.h"

//#define FRAME_PADDING 24



CloudsClip::CloudsClip(){
	currentScore = 0;
	startFrame = 0;
    endFrame = 0;
	hasCombinedVideo = false;
	adjustmentLoaded = false;
	minDepth = 400;
	maxDepth = 1200;
    keywordsDirty = true;
	networkPosition = ofVec3f(-1,-1,-1);
	hasProjectExample = false;
}

string CloudsClip::getLinkName(){
	return person + " - " + name;
}

string CloudsClip::getSpeakerFirstName(){
	return CloudsSpeaker::speakers[person].firstName;
}

string CloudsClip::getSpeakerLastName(){
	return CloudsSpeaker::speakers[person].lastName;
}

string CloudsClip::getSpeakerGender(){
	return CloudsSpeaker::speakers[person].gender;
}

float CloudsClip::getDuration(){
	return (endFrame - startFrame) / (is30FPS() ? 29.97 : 23.976); //TODO: HigaSan was recorded @ 30.0, need to compensate
}

//string CloudsClip::getStartingQuestion(){
//    if(startingQuestion.empty()){
//        return "-";
//    }
//    else{
//		return startingQuestion;
//    }
//}

void CloudsClip::addOverlappingClipName( string clipName){
    overlappingClips.push_back(clipName);
}

void CloudsClip::removeOverlappingClipName(string clipName) {
    if(ofContains(overlappingClips, clipName)){
        overlappingClips.erase(overlappingClips.begin()+ofFind(overlappingClips, clipName));
        
        cout<<"removing clip "<<clipName <<" from overlapping vector of clip "<< getLinkName()<<endl;
     }
}

bool CloudsClip::hasOverlappingClips(){
    return !overlappingClips.empty();
}

vector<string> CloudsClip::getOverlappingClips(){
    return overlappingClips;
}
//void CloudsClip::setStartingQuestion(string question){
//    startingQuestion = question;
//}
bool CloudsClip::hasStartingQuestion(){
	return hasQuestion() && hasSpecialKeyword("#start");
}
bool CloudsClip::hasAdditionalKeywords(){
    return !additionalKeywords.empty();
}
bool CloudsClip::hasRevokedKeywords(){
    return !revokedKeywords.empty();
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

bool CloudsClip::is30FPS(){
    return person == "Higa";
}

string CloudsClip::getFFMpegLine(string alternativeVideoPath, string exportFolder){

    float frameRate = is30FPS() ? 29.97 : 23.98;
	int framePadding = is30FPS() ? 30 : 24;
    float duration = ((float)( (endFrame-startFrame) + (framePadding*2 - 1) )) / frameRate;
    
    string dstSound = exportFolder + "/" +  getID() + ".wav";
    string srcSound;
	if(alternativeVideoPath != ""){
		srcSound = alternativeVideoPath + ofFilePath::getFileName(sourceVideoFilePath);
	}
	else{
		srcSound = relinkFilePath(sourceVideoFilePath);
	}
	
    stringstream pipeline1;
    pipeline1 << "ffmpeg -i \"" << srcSound << "\"";
    pipeline1 << " -ss " << ofToString( (float)(startFrame-framePadding+1) / (float)frameRate );
    pipeline1 << " -t "  << ofToString( duration );
    pipeline1 << " -ac 2 -ar 44100 -vn \"" << dstSound <<"\"";
    
    stringstream pipeline2;
    pipeline2 << "ffmpeg -start_number " << ofToString(startFrame-framePadding+1);
    pipeline2 << " -f image2 -r " << ofToString(frameRate);
    pipeline2 << " -i \"" << exportFolder << "/" << getCombinedPNGExportFolder() << getID() << "_%05d.png\"";
    pipeline2 << " -i \"" << dstSound << "\" -acodec copy ";
	if(is30FPS()){
		pipeline2 << " -codec:v libx264 -pix_fmt yuv420p -b 8000k -r 29.97 \"" << exportFolder << "/" << getCombinedMovieFile() << "\"";
	}
	else{
		pipeline2 << " -codec:v libx264 -pix_fmt yuv420p -b 8000k -r 23.976 \"" << exportFolder << "/" << getCombinedMovieFile() << "\"";
	}
    
    stringstream pipeline3;
    pipeline3 << "cp \"" << exportFolder << "/" << getCombinedPNGExportFolder() << "_calibration.xml\" ";
    pipeline3 << "\""<<exportFolder << "/" << getCombinedCalibrationXML() << "\"";
    
    return "\n" + pipeline1.str() + "\n" + pipeline2.str() + "\n" + pipeline3.str() + "\n";
}

string CloudsClip::getRelinkedVideoFilePath(){
	return relinkFilePath(sourceVideoFilePath);
}

string CloudsClip::getAdjustmentXML(){
	return ofFilePath::getEnclosingDirectory(ofFilePath::getEnclosingDirectory( ofFilePath::removeExt(relinkFilePath(sourceVideoFilePath)) )) + "adjustment.xml";
}

vector<string>& CloudsClip::getOriginalKeywords(){
    return originalKeywords;
}

vector<string>& CloudsClip::getAdditionalKeywords(){
    return additionalKeywords;
}

vector<string>& CloudsClip::getRevokedKeywords(){
    return revokedKeywords;
}

vector<string>& CloudsClip::getKeywords(){
    if(keywordsDirty){
        collateKeywords();
    }
    return keywords;
}

vector<string>& CloudsClip::getSpecialKeywords(){
    if(keywordsDirty){
        collateKeywords();
    }
    return specialKeywords;
}

bool CloudsClip::hasQuestion(){
    if(keywordsDirty){
        collateKeywords();
    }
	return questionTopicMap.size() > 0;
}

map<string,string>& CloudsClip::getAllQuestionTopicPairs(){
    if(keywordsDirty){
        collateKeywords();
    }
    return questionTopicMap;
}

vector<string>& CloudsClip::getAllTopicsWithQuestion(){
    if(keywordsDirty){
        collateKeywords();
    }
    return topicWithQuestions;
}

void CloudsClip::collateKeywords(){
    
    keywords = originalKeywords;
    specialKeywords.clear();
    
    //go through and remove revoked keywords
    for (int k = 0; k<revokedKeywords.size(); k++) {
        if(ofContains(keywords, revokedKeywords[k])){
            keywords.erase(keywords.begin()+ofFind(keywords, revokedKeywords[k]));
//            cout<<"Removing keywords for clip "<<name<< " : "<< revokedKeywords[k]<<endl;
        }
    }
    
    //go through and add additional
    for (int l = 0; l < additionalKeywords.size(); l++) {
        
        if (!ofContains(keywords, additionalKeywords[l]) ){
            keywords.push_back(additionalKeywords[l]);
//            cout<<"Adding addition keywords for clip "<<name<< " : "<< additionalKeywords[l]<<endl;
        }
    }
    
    //remove special keywords from keywords -> specialKeywords
    for (int l = keywords.size() - 1 ; l>=0; l--) {
        
        if(keywords[l].compare(0, 1, "#") == 0 && !ofContains(specialKeywords, keywords[l])){
//            cout<<"Special keywords for clip "<<name<< " : "<<keywords[l]<<". Erasing from keywords list"<<endl;
            specialKeywords.push_back(keywords[l]);
            keywords.erase(keywords.begin()+l);

        }
        else if(keywords[l][0] == '?' && !ofContains(specialKeywords, keywords[l])){
            
//           cout<<keywords[l] << " is a question in the new format. removing from keywords list and adding to questions"<< endl;
            
            //format of question topic pair is ?topic:question
            specialKeywords.push_back(keywords[l]);
            vector<string>questionTopicPair = ofSplitString(keywords[l], ":");
            ofStringReplace(questionTopicPair[0], "?", "");
            addQuestionTopicPair(questionTopicPair[0], questionTopicPair[1]);
            keywords.erase(keywords.begin() + l);
        }
    }
    
	hasProjectExample = false;
	for(int i = 0; i < specialKeywords.size(); i++){
		if(specialKeywords[i].find("example") != string::npos){
			vector<string> exampleProject = ofSplitString(specialKeywords[i], "?");
			if(exampleProject.size() != 2){
				ofLogError("CloudsClip::collateKeywords") << "Clip " << getLinkName() << " doesn't have a specific example tagged";
			}
			else {
				hasProjectExample = true;
				
			}
			break;
		}
	}
    keywordsDirty = false;
}

void CloudsClip::setOriginalKeywords(vector<string>& keywords){
    originalKeywords = keywords;
	keywordsDirty = true;
}

void CloudsClip::setDesiredKeywords(vector<string>& desiredKeywords){
    additionalKeywords.clear();
    revokedKeywords.clear();
    
    //find all the keywords not in the original list (additonal)
    for(int i= 0; i<desiredKeywords.size();i++){
        
        //Check to see if its a special keyword
        if(! ofContains(originalKeywords, desiredKeywords[i])&& !ofContains(additionalKeywords, desiredKeywords[i]) ){
//            cout<<"adding addtional keyword : "<< desiredKeywords[i]<<" to clip "<<name<<endl;
            addKeyword(desiredKeywords[i]);
        }
    }
    
    //find all the keywords missing from the original list (rvoked)
    for(int i=0; i < originalKeywords.size() ; i++){
        if(! ofContains(desiredKeywords, originalKeywords[i])&&! ofContains(revokedKeywords, originalKeywords[i])){
//            cout<<"revoking keyword : "<< originalKeywords[i]<<" from clip "<<name<<endl;
            revokeKeyword(originalKeywords[i]);
        }
    }
    keywordsDirty =true;
}

void CloudsClip::addKeyword(string keyword){
    cout << "adding keyword " << keyword<<endl;
    if(!ofContains(additionalKeywords, keyword) &&
       !ofContains(originalKeywords, keyword))
    {
        additionalKeywords.push_back(keyword);
        keywordsDirty = true;
    }
}

bool CloudsClip::hasKeyword(string keyword){
	return ofContains(getKeywords(), keyword);
}

void CloudsClip::setProjectExample(string projectExample){
	for(int i = 0; i < specialKeywords.size(); i++){
		if(ofToLower(specialKeywords[i]).find("example") != string::npos){
			revokeKeyword(specialKeywords[i]);
			break;
		}
	}
	
	addKeyword("#example?"+projectExample);
	keywordsDirty = true;
}

bool CloudsClip::hasSpecialKeyword(string keyword){
	if (keyword.at(0) != '#') {
		keyword = "#"+keyword;
	}
	return ofContains(specialKeywords,keyword);
}

void CloudsClip::revokeKeyword(string keyword){
	
//	cout << "revoking keyword " << keyword << endl;
//	cout << "	in original? " << (ofContains(originalKeywords, keyword) ? "YES" : "NO") << endl;
//	cout << "	in additional? " << (ofContains(additionalKeywords, keyword) ? "YES" : "NO") << endl;
//	cout << "	in revoked? " << (ofContains(revokedKeywords, keyword) ? "YES" : "NO") << endl;
//	
    if(!ofContains(revokedKeywords, keyword) &&
        ofContains(originalKeywords, keyword))
    {
        revokedKeywords.push_back(keyword);
        keywordsDirty = true;
    }
	else if(ofContains(additionalKeywords, keyword)){
		additionalKeywords.erase(additionalKeywords.begin() +
								 ofFind(additionalKeywords, keyword));
		keywordsDirty = true;
	}
}

void CloudsClip::addQuestionTopicPair(string topic, string question){
    
//    cout<<"adding question : " << question << " for topic "<< topic << " in clip " << getLinkName()<<endl;

    questionTopicMap[topic] = question;
    topicWithQuestions.push_back(topic);
}

string CloudsClip::getQuestionForTopic(string topic){
    if(questionTopicMap.find(topic) != questionTopicMap.end()){
        return questionTopicMap[topic];
    }
    ofLogError("CloudsClip::getQuestionForTopic")<<"No question found for topic "<<topic<<" in clip: "<<getLinkName()<<endl;
    return "";
}

vector<string> CloudsClip::getQuestions(){
    vector<string> questions;
    map<string,string>::iterator it;
    for( it = questionTopicMap.begin(); it != questionTopicMap.end(); it++){
        questions.push_back(it->second);
    }
    return questions;
}

vector<string> CloudsClip::getTopicsWithQuestions(){
	vector<string> topics;
	map<string,string>::iterator it;
    for( it = questionTopicMap.begin(); it != questionTopicMap.end(); it++){
        topics.push_back(it->first);
    }
	return topics;
}

void CloudsClip::loadAdjustmentFromXML(bool forceReload){
    
    //	if(adjustmentLoaded && !forceReload){
    //		return;
    //	}
	
	ofxXmlSettings adjustmentSettings;
	if(!adjustmentSettings.loadFile(getAdjustmentXML())){
//		ofLogError() << "Couldn't load adjustment XML" << getAdjustmentXML() << endl;
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
	
	skinTargetColor = ofFloatColor(adjustmentSettings.getValue("adjustment:skin:targetR", 1.0),
								   adjustmentSettings.getValue("adjustment:skin:targetG", 0.0),
								   adjustmentSettings.getValue("adjustment:skin:targetB", 0.0));
	
//	cout << "loaded skin target color " << skinTargetColor << endl;
	
	skinLowerThreshold = adjustmentSettings.getValue("adjustment:skin:lowerThreshold", 0.);
    skinUpperThreshold = adjustmentSettings.getValue("adjustment:skin:upperThreshold", 1.);
    skinHueWeight = adjustmentSettings.getValue("adjustment:skin:hueWeight", 0.5);
    skinSatWeight = adjustmentSettings.getValue("adjustment:skin:satWeight", 0.5);
    skinBrightWeight = adjustmentSettings.getValue("adjustment:skin:brightWeight", 0.5);

//	contourTargetThreshold = adjustmentSettings.getValue("adjustment:extraction:threshold", 100);
//	contourMinBlobSize = adjustmentSettings.getValue("adjustment:extraction:blobsize", 100);
	
	faceCoord = ofVec2f(adjustmentSettings.getValue("adjustment:extraction:faceu", 320.),
						adjustmentSettings.getValue("adjustment:extraction:facev", 110.));
	
//    cout << "loaded face coord color " << faceCoord << endl;
	
	//cout << "FOR CLIP " << getID() << " LOADED " << contourTargetColor << " target thresh " << contourTargetThreshold << " blob size " << contourMinBlobSize << endl;
	
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
    /*
	alignmentSettings.addValue("colorr", contourTargetColor.r);
	alignmentSettings.addValue("colorg", contourTargetColor.g);
	alignmentSettings.addValue("colorb", contourTargetColor.b);
     
	alignmentSettings.addValue("threshold", contourTargetThreshold);
	alignmentSettings.addValue("blobsize", contourMinBlobSize);
    */
	alignmentSettings.addValue("faceu", faceCoord.x);
	alignmentSettings.addValue("facev", faceCoord.y);

	
	//cout << "FOR CLIP " << getID() << " SAVED " << contourTargetColor << " target thresh " << contourTargetThreshold << " blob size " << contourMinBlobSize << endl;
	
	alignmentSettings.popTag(); //extraction

    ///SM ADDED
    alignmentSettings.addTag("skin");
    alignmentSettings.pushTag("skin");
    alignmentSettings.addValue("targetR",skinTargetColor.r);
    alignmentSettings.addValue("targetG",skinTargetColor.g);
    alignmentSettings.addValue("targetB",skinTargetColor.b);
    alignmentSettings.addValue("hueWeight", skinHueWeight);
    alignmentSettings.addValue("satWeight",skinSatWeight);
    alignmentSettings.addValue("brightWeight", skinBrightWeight);
    alignmentSettings.addValue("lowerThreshold", skinLowerThreshold);
    alignmentSettings.addValue("upperThreshold",skinUpperThreshold);
    alignmentSettings.popTag();//skin
    
	alignmentSettings.popTag(); //adjustment
	
	alignmentSettings.saveFile(getAdjustmentXML());
}

string CloudsClip::getSceneFolder(){
	return ofFilePath::getEnclosingDirectory(ofFilePath::getEnclosingDirectory(relinkFilePath(sourceVideoFilePath)));
}

