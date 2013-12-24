//
//  CloudsFCPParser.cpp
//  example-empty
//
//  Created by James George on 2/4/13.
//
//

#include "CloudsFCPParser.h"
#include "CloudsGlobal.h"
#include "CloudsSpeaker.h"
#include "CloudsDichotomy.h"

bool distanceSort(pair<string,float> a, pair<string,float> b ){
    return a.second < b.second;
}

bool distanceSortLargeToSmall(pair<string,float> a, pair<string,float> b ){
    return a.second > b.second;
}

CloudsFCPParser::CloudsFCPParser(){
	printErrors = false;
    sortedByOccurrence = false;
	backupTimeInterval = 60*2;
	lastBackupTime = -backupTimeInterval;
}

void CloudsFCPParser::loadFromFiles(){
    setup(GetCloudsDataPath() + "fcpxml");
	parseVOClips();
    parseLinks(GetCloudsDataPath() + "links/clouds_link_db.xml");
//    parseClusterMap(GetCloudsDataPath() + "gephi/2013_7_25_Clouds_conversation.SVG");
	//parseClusterMap(GetCloudsDataPath() + "gephi/CLOUDSClusterMap.svg");
	parseClusterNetwork(GetCloudsDataPath() + "pajek/CloudsNetwork.net");
	parseProjectExamples(GetCloudsDataPath() + "secondaryDisplay/web/xml/projects.xml");
	
}

void CloudsFCPParser::setup(string directory){
    xmlDirectory = directory;
    refreshXML();
}

void CloudsFCPParser::refreshXML(){
    ofDirectory dir;
    allClips.clear();
    
    fileIdToPath.clear();
    fileIdToName.clear();
    clipIDToIndex.clear();
	clipLinkNameToIndex.clear();
    keywordVector.clear();
	hasMediaAssetIndeces.clear();
	
    linkedConnections.clear();
    suppressedConnections.clear();
    sourceSupression.clear();
    
    dir.allowExt("xml");
    dir.listDir(xmlDirectory);
    if( dir.numFiles() > 0 ){
        for(int i = 0; i < dir.numFiles(); i++){
            addXMLFile( dir.getPath(i) );
        }
    }
	
	//printSpeakerList();
    getOverlappingClipIDs();
    autolinkSequentialClips();
	CloudsSpeaker::populateSpeakers();
    refreshAllKeywords();
}

void CloudsFCPParser::saveClusterMap(map<string, ofVec2f> centroidMap ){
    float maxCx=6124.1;
    float maxCy=6008.18;
    float minCx=-7113.02;
    float minCy=-5992.12;
    map<string, ofVec2f> ::iterator it;
    
    for(it = centroidMap.begin(); it != centroidMap.end(); it++){
        
        cout<<"<text transform=\"matrix(1 0 0 1 "<<it->second.x<<" "<<it->second.y<<")\" font-family=\"'MyriadPro-Regular'\" font-size=\"12\">"<<it->first<<"</text>"<<endl;
    }
}

void CloudsFCPParser::parseVOClips(){
	
	ofDirectory dir(GetCloudsDataPath() + "VO");
	dir.allowExt("aif");
	dir.allowExt("wav");
	dir.allowExt("mp3");
	dir.allowExt("aiff");
	
	dir.listDir();
	
	for(int i = 0; i < dir.numFiles(); i++){
		
		CloudsClip clip;
		
		clip.voiceOverAudio = true;
		clip.hasMediaAsset = true; //should be something like 'backing asset'
		clip.voiceOverAudioPath = dir.getPath(i);
		clip.sourceVideoFilePath = dir.getPath(i);
		clip.startFrame = 0;
		clip.endFrame = 35*24.; //DOES THIS WORK LOL?
		
		string name = ofFilePath::getBaseName( dir.getName(i) );
		//remove weird final cut track name
		ofStringReplace(name, "_1-2", "");
		vector<string> components = ofSplitString(name,"_");
        //validate
		if(components.size() != 2){
			ofLogError("CloudsFCPParser::parseVOClips") << "VO Clip " << dir.getPath(i) << " incorrectly formatted";
			continue;
		}
		
		clip.person = components[0];
		clip.name = components[1];
		
		cout << "added VO only clip " << clip.getLinkName() << endl;
		
		clipIDToIndex[clip.getID()] = allClips.size();
		clipLinkNameToIndex[clip.getLinkName()] = allClips.size();
		allClips.push_back(clip);
	}
		
}

void CloudsFCPParser::parseLinks(string linkFile){
    
    linkedConnections.clear();
	suppressedConnections.clear();
//	questionIds.clear();
	
    int totalLinks = 0;
    ofxXmlSettings linksXML;
    if(!linksXML.loadFile(linkFile)){
		if(printErrors) ofSystemAlertDialog("UNABLE TO LOAD LINKS! do not proceed");
		return;
	}
	
	int numClips = linksXML.getNumTags("clip");
	for(int i = 0; i < numClips; i++){
		linksXML.pushTag("clip", i);
		
		string clipName = linksXML.getValue("name", "");
		int numLinks = linksXML.getNumTags("link");
		int numSuppressed = linksXML.getNumTags("suppress");
		int numRevoked = linksXML.getNumTags("revokedKeywords");
		int numAdditional = linksXML.getNumTags("additionalKeywords");
		int startQuestion = linksXML.getNumTags("startingQuestion");
		
		if(numLinks > 0){
			for(int l = 0; l < numLinks; l++){
				CloudsLink newLink;
				linksXML.pushTag("link", l);
				
				newLink.sourceName = clipName;
				newLink.targetName = linksXML.getValue("target", "");
				newLink.startFrame = linksXML.getValue("startFrame", -1);
				newLink.endFrame   = linksXML.getValue("endFrame", -1);
				
				linksXML.popTag(); //link
				
				if(!hasClipWithLinkName(newLink.sourceName)){
					string errorText = "Final Cut XML is missing \"" + newLink.sourceName + "\" which linked to \"" + newLink.targetName + "\".";
					ofLogError() << errorText;
					if(printErrors) ofSystemAlertDialog(errorText);
					continue;
				}
				if(!hasClipWithLinkName(newLink.targetName)){
					string errorText = "Final Cut XML is missing \"" + newLink.targetName + "\" which was linked from \"" + newLink.sourceName + "\".";
					ofLogError() << errorText;
					if(printErrors) ofSystemAlertDialog(errorText);
					continue;
				}
				linkedConnections[newLink.sourceName].push_back( newLink );
			}
		}
		
		if(numSuppressed > 0){
			for(int l = 0; l < numSuppressed; l++){
				CloudsLink newLink;
				linksXML.pushTag("suppress", l);
				
				newLink.sourceName = clipName;
				newLink.targetName = linksXML.getValue("target", "");
				newLink.startFrame = linksXML.getValue("startFrame", -1);
				newLink.endFrame   = linksXML.getValue("endFrame", -1);
				
				linksXML.popTag();//suppress
                
				if(!hasClipWithLinkName(newLink.sourceName)){
					string errorText = "Final Cut XML is missing \"" + newLink.sourceName + "\" which was suppressed from \"" + newLink.targetName + "\".";
					ofLogError() << errorText;
					if(printErrors) ofSystemAlertDialog(errorText);
					continue;
				}
				if(!hasClipWithLinkName(newLink.targetName)){
					string errorText = "Final Cut XML is missing \"" + newLink.targetName + "\" which was suppressed from \"" + newLink.sourceName + "\".";
					ofLogError() << errorText;
					if(printErrors) ofSystemAlertDialog(errorText);
					continue;
				}
				
				suppressedConnections[newLink.sourceName].push_back( newLink );
			}
		}
		
//		if(startQuestion > 0){
//			string question = linksXML.getValue("startingQuestion", "");
//			bool hasQuestionClip;
//			CloudsClip& c = getClipWithLinkName(clipName,hasQuestionClip);
//			if(hasQuestionClip){
//				c.setStartingQuestion(question);
//				questionIds.push_back( c.getID() );
//                //				cout << c.getID() << " has question: " << c.getStartingQuestion() << endl;
//			}
//			else{
//				ofLogError("CloudsFCPParser::parseLinks") << clipName << " not found! has question " << question;
//			}
//		}
        
		//add revoked and additional keywords
        if(numRevoked > 0){
            string revokedKeywordsString = linksXML.getValue("revokedKeywords", "");
            vector<string> revokedKeywords = ofSplitString(revokedKeywordsString, ",");
            CloudsClip& c = getClipWithLinkName(clipName);
            
            for(int i = 0; i < revokedKeywords.size(); i++){
                c.revokeKeyword(revokedKeywords[i]);
            }
        }
        
        if(numAdditional > 0){
            string additionalKeywordsString = linksXML.getValue("additionalKeywords", "");
            vector<string> additionalKeywords = ofSplitString(additionalKeywordsString, ",");
            CloudsClip& c = getClipWithLinkName(clipName);
            
            for(int i = 0; i < additionalKeywords.size(); i++){
                c.addKeyword(additionalKeywords[i]);
            }
        }
        
		linksXML.popTag(); //clip
	}
    
    
    for(int i = 0; i < allClips.size();i++){
        reciprocateSuppressions(allClips[i]);
    }
    
	refreshAllKeywords();
}

#pragma mark KEYWORD + CLUSTER DATA
void CloudsFCPParser::parseClusterNetwork(string fileName){
	ofBuffer pajekFile = ofBufferFromFile(fileName);
	bool findingNodes = false;
	while(!pajekFile.isLastLine()){
		string line = pajekFile.getNextLine();
		
		if(line == "" || line.at(0) == '%'){
			continue;
		}
		
		if (line.find("*Vertices") != string::npos ) {
			findingNodes = true;
			continue;

		}
		if (line.find("*Edgeslist") != string::npos ) {
			break;
		}
		
		if(findingNodes){
			vector<string> components = ofSplitString(line, " ");
			string linkName = ofSplitString(line, "\"")[1];
			bool foundClip;
			CloudsClip& clip = getClipWithLinkName(linkName,foundClip);
			if(!foundClip){
				ofLogError("CLIP " + linkName + " NOT FOUND IN CLUSTER NETWORK");
				continue;
			}
			int numcomp = components.size();
			clip.networkPosition = ofVec3f(ofToFloat(components[numcomp-3]),
										   ofToFloat(components[numcomp-2]),
										   ofToFloat(components[numcomp-1]));
			
		}
	}
	
//	populateKeywordCentroids();
//	calculateCohesionMedianForKeywords();
//	calculateKeywordAdjascency();
	calculateKeywordFamilies();
}

void CloudsFCPParser::parseProjectExamples(string filename){
	
	clipIdToProjectExample.clear();
	projectExamples.clear();
	
	ofxXmlSettings projectExamplesXML;
	if(!projectExamplesXML.loadFile(filename)){
		ofLogError("CloudsFCPParser::parseProjectExamples") << "Project examples failed to parse at path" << filename;
		return;
	}
		
	projectExamplesXML.pushTag("clouds");
	int numProjectExamples = projectExamplesXML.getNumTags("project");
	for(int i = 0; i < numProjectExamples; i++){
		
		string projectTitle = projectExamplesXML.getAttribute("project", "title", "", i);
		if(projectTitle == ""){
			ofLogError("CloudsFCPParser::parseProjectExamples") << "Project " << i << " does not have a title";
			continue;
		}
		
		projectExamplesXML.pushTag("project",i);

		CloudsProjectExample example;
		example.title = projectTitle;
		example.creatorName = projectExamplesXML.getValue("creator_name", "");
		example.description = projectExamplesXML.getValue("description", "");
		if(projectExamplesXML.tagExists("videos")){
			projectExamplesXML.pushTag("videos");
			int numVideos = projectExamplesXML.getNumTags("file");
			if(numVideos == 0){
				ofLogError("CloudsFCPParser::parseProjectExamples") << "Project " << projectTitle << " doesn't have ny <file> tags in <videos>";
			}
			for(int f = 0; f < numVideos; f++){
				example.exampleVideos.push_back(projectExamplesXML.getValue("file","",f));
			}
			projectExamplesXML.popTag(); //videos
		}
		else{
			ofLogError("CloudsFCPParser::parseProjectExamples") << "Project " << projectTitle << " doesn't have <videos> tag";
		}
		
		projectExamplesXML.popTag();//project
		
		clipIdToProjectExample[example.title] = projectExamples.size();
		projectExamples.push_back(example);
	}
	projectExamplesXML.popTag();//clouds
	
	//populate project examples on all clips
	for(int i = 0; i < allClips.size(); i++){
		if(allClips[i].hasProjectExample){
			allClips[i].projectExample = getProjectExampleWithTitle(allClips[i].projectExampleTitle);
		}
	}
}

vector<CloudsProjectExample>& CloudsFCPParser::getProjectExamples(){
	return projectExamples;
}

CloudsProjectExample& CloudsFCPParser::getProjectExampleWithTitle(string title){
	if(clipIdToProjectExample.find(title) == clipIdToProjectExample.end()){
		ofLogError("CloudsFCPParser::getProjectExampleWithTitle") << "Couldn't find project example with title " << title;
		return dummyProjectExample;
	}
	return projectExamples[ clipIdToProjectExample[title] ];
}

void CloudsFCPParser::populateKeywordCentroids(){

	keywordCentroids.clear();
	keywordCentroidIndex.clear();
	vector<string>& keywords = getContentKeywords();
    for(int k = 0; k < keywords.size(); k++){
        vector<CloudsClip> clips = getClipsWithKeyword(keywords[k]);
        
        float numClips = 0;
        ofVec3f centroid(0,0,0);
        
        for( int i = 0; i < clips.size(); i++){
            if( clips[i].networkPosition != ofVec3f(-1, -1, -1) ){
                centroid += clips[i].networkPosition;
                numClips++;
            }
        }
        
        centroid /= numClips;
		
        keywordCentroids.push_back( make_pair(keywords[k], centroid) );
        keywordCentroidIndex[keywords[k]] = k;
    }
}

void CloudsFCPParser::calculateKeywordAdjascency(){
	keywordAdjacency.clear();
	vector<string>& keywords = getContentKeywords();
	for(int i = 0; i < keywords.size(); i++){
		keywordAdjacency[keywords[i]] = getAdjacentKeywords(keywords[i], 10);
	}
}

//returns keywords that are close to the given keyword on the cluster map
vector<string> CloudsFCPParser::getAdjacentKeywords( string currentKeyword , int numOfDesiredKeywords){
    string keyword = "";
    ofVec2f centroid;
    vector<pair<string, float> > distancePair;
    
    centroid = getKeywordCentroid(currentKeyword);
    
    for (int j=0; j < keywordCentroids.size(); j++) {
        if(keywordCentroids[j].first != currentKeyword){
			float distance = centroid.distance(keywordCentroids[j].second);
			distancePair.push_back(make_pair(keywordCentroids[j].first, distance));
		}
    }
    
    sort(distancePair.begin(), distancePair.end(), distanceSort);
    
    vector<string> adjacentKeywords;
    numOfDesiredKeywords = MIN(distancePair.size(),numOfDesiredKeywords);

//	cout << "keyword " << currentKeyword << endl;
    for (int k = 0; k < numOfDesiredKeywords; k++) {
//		cout << "	" << distancePair[k].first << " " <<distancePair[k].second << endl;
        adjacentKeywords.push_back(distancePair[k].first);
    }
    
    return adjacentKeywords;
}

void CloudsFCPParser::calculateKeywordFamilies(){
	keywordFamilies.clear();
	vector<string>& keywords = getContentKeywords();
	ofBuffer keywordFamilyBuffer;
	
	for(int i = 0; i < keywords.size(); i++){
		
		string keywordA = keywords[i];
		int clipsWithKeywordA = getNumberOfClipsWithKeyword(keywordA);
		set<string> related = getRelatedKeywords(keywordA);
		set<string>::iterator it;
		vector< pair<string,float> > overlapScore;
		
		for(it = related.begin(); it != related.end(); it++){
			string keywordB = *it;
			int clipsWithKeywordB = getNumberOfClipsWithKeyword(keywordB);
			int clipsInCommon = getNumberOfSharedClips(keywordA, keywordB);
			float percent = 1.0 * clipsInCommon / (clipsWithKeywordA + clipsWithKeywordB - clipsInCommon);
			overlapScore.push_back(make_pair(keywordB, percent));
		}
		
		sort(overlapScore.begin(), overlapScore.end(), distanceSortLargeToSmall);
//		cout << "keyword " << keywordA << endl;
		
//		keywordFamilyBuffer.append("keyword " + keywordA + "\n");
		for(int i = 0; i < MIN(overlapScore.size(), 10); i++){
			//cout << "	" << overlapScore[i].first << " " << overlapScore[i].second << endl;
			keywordFamilies[keywordA].push_back(overlapScore[i].first);
//			keywordFamilyBuffer.append("	" + overlapScore[i].first + " " + ofToString(overlapScore[i].second) + "\n" );
		}
	}
//	ofBufferToFile(GetCloudsDataPath() + "stats/keyword_families.txt", keywordFamilyBuffer);
}

vector<string>& CloudsFCPParser::getKeywordFamily(string keyword){
	return keywordFamilies[keyword];
}

float CloudsFCPParser::getDistanceFromAdjacentKeywords(string keyword1, string keyword2){    
    return getKeywordCentroid(keyword1).distance(getKeywordCentroid(keyword2));
}

float CloudsFCPParser::getCohesionIndexForKeyword(string keyword){
    if(keywordCohesionMap.find(keyword) != keywordCohesionMap.end()){
        return keywordCohesionMap[keyword];
    }
//    ofLogError("CloudsFCPParser::getCohesionIndexForKeyword")<<"Couldnt find cohesion index for keyword: " << keyword << endl;
    return 0;
}

void CloudsFCPParser::calculateCohesionMedianForKeywords(){
	
	vector< pair<string, float> > topicCohesionPairs;
	vector<string>& keywords = getContentKeywords();
    for( int i=0; i< keywords.size(); i++){
        string currentKeyword = keywords[i];
        ofVec2f keywordCentroid = getKeywordCentroid(currentKeyword);
        vector<CloudsClip> clips = getClipsWithKeyword(currentKeyword);
        
        vector<float> distancesPerClip;
        float maxDistance = 0;
        float minDistance = INT_MAX;
        float numClips  =0;
        float totalDistance = 0;
        
        for (int k=0; k<clips.size(); k++) {
            if(clips[k].networkPosition != ofVec3f(-1,-1,-1)){
                float distance = keywordCentroid.distance(clips[k].networkPosition);
                totalDistance += distance;
                distancesPerClip.push_back(distance);
                maxDistance = MAX(maxDistance,distance);
                minDistance = MIN(minDistance,distance);
                numClips++;
            }
        }
		
		float avgDistance = totalDistance / numClips;

		if(distancesPerClip.size() != 0 && maxDistance != 0){
			std::sort(distancesPerClip.begin(), distancesPerClip.end());
			float medianValue = distancesPerClip[distancesPerClip.size()/2];
			keywordCohesionMap[currentKeyword] = medianValue / maxDistance;
			topicCohesionPairs.push_back(make_pair(currentKeyword, keywordCohesionMap[currentKeyword]));
//			cout << "**** Keyword " << currentKeyword << " has an average distance of " << avgDistance << " from " << numClips << " has cohesion score " << medianValue << "/" << maxDistance << "		" << medianValue / maxDistance << endl;
		}
		
    }
	
//	sort(topicCohesionPairs.begin(), topicCohesionPairs.end(), distanceSort);
//	cout << "COHESION:" << endl;
//	for(int i = 0; i < topicCohesionPairs.size(); i++){
//		cout << "	" << topicCohesionPairs[i].first << " " << topicCohesionPairs[i].second << endl;
//	}

}

ofVec2f CloudsFCPParser::getKeywordCentroid(string keyword){
    
    int index = getCentroidMapIndex(keyword);
    if(index != -1){
        return keywordCentroids[index].second;
    }
    //ofLogError("CloudsFCPParser::getKeywordCentroid")<<"No centroid found for keyword: "<< keyword<<endl;
    
    return ofVec2f(-1, -1);
}

int CloudsFCPParser::getCentroidMapIndex(string keyword){
    if(keywordCentroidIndex.find(keyword) != keywordCentroidIndex.end()){
        return keywordCentroidIndex[keyword];
    }
    ofLogError("CloudsFCPParser::getCentroidMapIndex")<<" Couldnt find  index for keyword: "<<keyword<<endl;
	return -1;
}

void CloudsFCPParser::saveLinks(string linkFile){
	
    int numClips = 0;
	if( (ofGetElapsedTimef() - lastBackupTime) >= backupTimeInterval){
		char backup[1024];
		sprintf( backup, "%s_backup_Y.%02d_MO.%02d_D.%02d_H.%02d_MI.%02d.xml", ofFilePath::removeExt(linkFile).c_str(), ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes() );
		lastBackupTime = ofGetElapsedTimef();
		if(!ofFile(linkFile).copyTo(backup)){
			if(printErrors) ofSystemAlertDialog("UNABLE TO CREATE LINK BACK UP");
			return;
		}

		cout << "BACKUPING UP FILE FROM " << linkFile << " to " << backup << endl;
	}
	
    ofxXmlSettings linksXML;
    map<string, vector<CloudsLink> >::iterator it;
	
	for(int i = 0; i < allClips.size(); i++){
		string clipName = allClips[i].getLinkName();
		
		bool hasLink = clipHasLink( allClips[i]);
		bool hasSuppressed = clipHasSuppressions( allClips[i] );
//        bool hasStartingQuestion = clipHasStartingQuestions(allClips[i]);
		bool hasRevokedKeywords = clipHasRevokedKeywords(allClips[i]);
        bool hasAdditionalKeywords = clipHasAdditionalKeywords(allClips[i]);
        
        if(hasLink || hasSuppressed || /*hasStartingQuestion ||*/ hasRevokedKeywords || hasAdditionalKeywords){
			
			linksXML.addTag("clip");
			linksXML.pushTag("clip", numClips++);
			
			linksXML.addValue("name", clipName);
            
			if(hasLink){
				vector<CloudsLink>& clipLinks = linkedConnections[clipName];
                
				for(int l = 0; l < clipLinks.size(); l++){
					linksXML.addTag("link");
					linksXML.pushTag("link", l);
					linksXML.addValue("target", clipLinks[l].targetName );
					linksXML.addValue("startFrame", clipLinks[l].startFrame);
					linksXML.addValue("endFrame", clipLinks[l].endFrame);
					linksXML.popTag(); //link!
				}
			}
			
			if(hasSuppressed){
				vector<CloudsLink>& suppressed = suppressedConnections[clipName];
				for(int l = 0; l < suppressed.size(); l++){
					linksXML.addTag("suppress");
					linksXML.pushTag("suppress", l);
					linksXML.addValue("target", suppressed[l].targetName );
					linksXML.addValue("startFrame", suppressed[l].startFrame);
					linksXML.addValue("endFrame", suppressed[l].endFrame);
					linksXML.popTag(); //suppress!
				}
			}
            
//            if(hasStartingQuestion){
//                string startQuestion = allClips[i].getStartingQuestion();
//                linksXML.addValue("startingQuestion", startQuestion);
//            }
            
            if(hasRevokedKeywords){
                string revokedKeywords=ofJoinString(allClips[i].getRevokedKeywords(), ",") ;
                linksXML.addValue("revokedKeywords", revokedKeywords);
                //                cout<<"revoking keywords in save: "<<revokedKeywords<<endl;
                
            }
            
            if(hasAdditionalKeywords){
                string additionalKeywords = ofJoinString(allClips[i].getAdditionalKeywords(),"," );
                linksXML.addValue("additionalKeywords", additionalKeywords);
            }
			
			linksXML.popTag();
		}
	}
    
    if(! linksXML.saveFile(linkFile) ){
		if(printErrors) ofSystemAlertDialog("UNABLE TO SAVE LINKS. DO NOT PROCEED");
	}
}

void CloudsFCPParser::removeLink(string linkName, int linkIndex){
    if(linkedConnections.find(linkName) != linkedConnections.end() && linkIndex < linkedConnections[linkName].size()){
		cout << "removed link " << linkIndex << " from " << linkName << endl;
        linkedConnections[linkName].erase(linkedConnections[linkName].begin() + linkIndex);
    }
    else{
        cout << "failed to remove link " << linkIndex << " from " << linkName << endl;
    }
}

void CloudsFCPParser::removeLink(string linkName, string targetName){
	int linkIndex;
	if(clipLinksTo(linkName, targetName, linkIndex)){
		cout << "removed link " << targetName << " from " << linkName << endl;
		linkedConnections[linkName].erase(linkedConnections[linkName].begin() + linkIndex);
    }
    else{
        cout << "failed to remove link " << targetName << " from " << linkName << endl;
    }
}

void CloudsFCPParser::suppressConnection(string sourceName, string targetName){
	CloudsLink l;
	l.sourceName = sourceName;
	l.targetName = targetName;
	l.startFrame = -1;
	l.endFrame = -1;
	suppressConnection(l);
}

void CloudsFCPParser::suppressConnection(CloudsClip& source, CloudsClip& target){
	CloudsLink l;
	l.sourceName = source.getLinkName();
	l.targetName = target.getLinkName();
	l.startFrame = -1;
	l.endFrame = -1;
	suppressConnection(l);
}

void CloudsFCPParser::suppressConnection(CloudsLink& link){
	
	// remove any existing links
    if(clipLinksTo(link.sourceName,link.targetName)){
        removeLink(link.sourceName,link.targetName);
        cout<<"Link being removed from "<< link.sourceName<<" and "<<link.targetName<<
		" in suppressConnection function"<< endl;
    }
    
	//suppress one way
	if(!linkIsSuppressed(link.sourceName, link.targetName)){
		cout << "Suppressed connection " << link.sourceName << " >> " << link.targetName << endl;
		suppressedConnections[link.sourceName].push_back(link);
	}
	
	//reciprocate the suppression
    if(!linkIsSuppressed(link.targetName, link.sourceName)){
		
		// remove any existing link the other way
		if(clipLinksTo(link.targetName,link.sourceName)){
			removeLink(link.targetName,link.sourceName);
			cout<<"Link being removed from "<< link.targetName<<" and "<<link.sourceName<<
			" in suppressConnection function"<< endl;
		}
		
        CloudsLink swap = link;
        swap.targetName = link.sourceName;
        swap.sourceName = link.targetName;
        suppressedConnections[link.targetName].push_back(swap);
    }
}

void CloudsFCPParser::unsuppressConnection(string linkName, string targetName){
	
	if(clipHasSuppressions(linkName)){
		int linkIndex;
		if(linkIsSuppressed(linkName, targetName, linkIndex)){
			cout << "Unsuppressed connection " << linkName << " >> " << targetName << endl;
            unsuppressConnection(linkName, linkIndex);
		}
        else {
            ofLogError() << "Could not unsuppress connection" << linkName << " to " << targetName;
        }
	}
}

void CloudsFCPParser::unsuppressConnection(string linkName, int linkIndex){
	
	if(suppressedConnections.find(linkName) != suppressedConnections.end()){
		
        cout << "Unsuppressed connection " << linkName << " >> " << linkIndex << endl;
        CloudsLink& suppressedLink = suppressedConnections[linkName][linkIndex];
        int reciprocalIndex;
        if(linkIsSuppressed(suppressedLink.targetName, suppressedLink.sourceName, reciprocalIndex)){
            suppressedConnections[suppressedLink.targetName].erase( suppressedConnections[suppressedLink.targetName].begin() + reciprocalIndex );
        }
        else{
            ofLogError() << "No reciprocal suppression between clip " << suppressedLink.sourceName << " and " << suppressedLink.targetName;
            if(printErrors) ofSystemAlertDialog( "No reciprocal suppression between clip " + suppressedLink.sourceName + " and " + suppressedLink.targetName);
        }
        
		suppressedConnections[linkName].erase( suppressedConnections[linkName].begin() + linkIndex );
	}
}

void CloudsFCPParser::unsuppressConnection(CloudsLink& link){
	unsuppressConnection(link.sourceName, link.targetName);
}

bool CloudsFCPParser::clipHasLink(CloudsClip& clip){
	return clipHasLink(clip.getLinkName());
}
bool CloudsFCPParser::clipHasLink(string clipName){
	return linkedConnections.find(clipName) != linkedConnections.end() && linkedConnections[clipName].size() > 0;
}

bool CloudsFCPParser::clipsShareLink(string clipNameA, string clipNameB){
	return clipLinksTo(clipNameA, clipNameB) || clipLinksTo(clipNameB, clipNameA);
}

bool CloudsFCPParser::clipLinksTo(string clipNameA, string clipNameB) {
	int deadIndex;
	return clipLinksTo(clipNameA, clipNameB, deadIndex);
}

bool CloudsFCPParser::clipLinksTo(string clipNameA, string clipNameB, int& index){
	if(clipHasLink(clipNameA)) {
		vector<CloudsLink>& links = linkedConnections[ clipNameA ];
		for(int i = 0; i < links.size(); i++){
			if(links[ i ].targetName == clipNameB){
				index = i;
				return true;
			}
		}
	}
	return false;
}

bool CloudsFCPParser::clipHasSuppressions(CloudsClip& clip){
	return clipHasSuppressions(clip.getLinkName());
}
bool CloudsFCPParser::clipHasSuppressions(string clipName){
	return suppressedConnections.find(clipName) != suppressedConnections.end() && suppressedConnections[clipName].size() > 0;
}


bool CloudsFCPParser::clipHasRevokedKeywords(CloudsClip& clip){
    return clip.hasRevokedKeywords();
}

bool CloudsFCPParser::clipHasAdditionalKeywords(CloudsClip& clip){
    return clip.hasAdditionalKeywords();
}

bool CloudsFCPParser::clipHasStartingQuestions(string clipName){
    CloudsClip& c = getClipWithLinkName(clipName);
    return clipHasStartingQuestions(c);
}
bool CloudsFCPParser::clipHasStartingQuestions(CloudsClip& clip){
    return clip.hasStartingQuestion();
}

bool CloudsFCPParser::linkIsSuppressed(string clipNameA, string clipNameB){
	int deadIndex;
	return linkIsSuppressed(clipNameA, clipNameB, deadIndex);
}

bool CloudsFCPParser::linkIsSuppressed(string clipNameA, string clipNameB, int& index){
	if(clipHasSuppressions(clipNameA)){
		vector<CloudsLink>& suppressions = suppressedConnections[clipNameA];
		for(int i = 0; i < suppressions.size(); i++){
			if(suppressions[i].targetName == clipNameB){
				index = i;
				return true;
			}
		}
	}
	return false;
}

bool CloudsFCPParser::keywordsShareLink(string keyA, string keyB){
	vector<string> keywordFilter;
	keywordFilter.push_back(keyA);
	keywordFilter.push_back(keyB);
	
	vector<CloudsClip> clips = getClipsWithKeyword(keywordFilter);
	for(int i = 0; i < clips.size(); i++){
		for(int j = i+1; j < clips.size(); j++){
			if(clipsShareLink(clips[i].getLinkName(), clips[j].getLinkName())){
				return true;
			}
		}
	}
	return false;
}



void CloudsFCPParser::addXMLFile(string xmlFile){
    ofxXmlSettings fcpXML;
    if(fcpXML.loadFile(xmlFile)){
        fcpXML.pushTag("xmeml");
        int numSequences = fcpXML.getNumTags("sequence");
        for(int i = 0; i < numSequences; i++){
            string name = fcpXML.getAttribute("sequence", "id", "", i);
            //            cout << "name is " << name << endl;
            fcpXML.pushTag("sequence", i);
            fcpXML.pushTag("media");
            fcpXML.pushTag("video");
            
            int numTracks = fcpXML.getNumTags("track");
            //          cout << "   found " << numTracks << " tracks" << endl;
            for(int t = 0; t < numTracks; t++){
                fcpXML.pushTag("track", t);
                int numClipItems = fcpXML.getNumTags("clipitem");
                //                cout << "   found " << numClipItems << " clips" << endl;
                for(int c = 0; c < numClipItems; c++){
                    fcpXML.pushTag("clipitem", c);
                    parseClipItem(fcpXML, name);
                    fcpXML.popTag(); //clipitem
                }
                fcpXML.popTag(); // track
            }
            
            fcpXML.popTag(); //video
            fcpXML.popTag(); //media
            fcpXML.popTag(); //sequence
        }
        fcpXML.popTag(); //xmeml
        
    }
    else{
        ofLogError("CloudsFCPParser::addXMLFile -- file parse failed " + xmlFile);
    }
	
	
}

void CloudsFCPParser::parseClipItem(ofxXmlSettings& fcpXML, string currentName){
    string fileID = fcpXML.getAttribute("file", "id", "");
    string clipFileName = fcpXML.getValue("file:name", "");
    string clipFilePath = fcpXML.getValue("file:pathurl", "");
    if(clipFileName != ""){
        fileIdToName[fileID] = clipFileName;
        fileIdToPath[fileID] = clipFilePath;
    }
    else{
        clipFileName = fileIdToName[fileID];
        clipFilePath = fileIdToPath[fileID];
    }
    ofStringReplace(clipFilePath, "file://localhost", "");
    int numMarkers = fcpXML.getNumTags("marker");
    for(int m = 0; m < numMarkers; m++){
        fcpXML.pushTag("marker", m);
        
        CloudsClip cm;
        cm.startFrame = fcpXML.getValue("in", 0);
        cm.endFrame = fcpXML.getValue("out", 0);
        string comment = fcpXML.getValue("comment", "");
        //validate
        if(comment != "" && cm.endFrame - cm.startFrame > 1 && cm.endFrame > 0){
            cm.name = fcpXML.getValue("name", "");
            cm.person = currentName;
			cm.fcpFileId = fileID;
			cm.clip = clipFileName;
			cm.sourceVideoFilePath = clipFilePath;
			
			if( markerLinkNames.find(cm.getLinkName()) != markerLinkNames.end() ){
				ofLogError() << "DUPLICATE CLIP " << cm.getLinkName() << " " << cm.getMetaInfo();
				//ofLogError() << "	EXISTING CLIP INFO " << getClipWithLinkName(cm.getLinkName()).getMetaInfo();
			}
			else{
				markerLinkNames.insert( cm.getLinkName() );
//				cm.color.r = fcpXML.getValue("color:red", 0);
//				cm.color.g = fcpXML.getValue("color:green", 0);
//				cm.color.b = fcpXML.getValue("color:blue", 0);
				string keywordString = ofToLower( fcpXML.getValue("comment", "") );
				ofStringReplace(keywordString, "\n", ",");
                vector<string> fcpKeywords = ofSplitString(keywordString, ",",true,true);
				cm.setOriginalKeywords(fcpKeywords);
                
                //            cout << "       added marker: \"" << cm.name << "\" with [" << cm.keywords.size() << "] keywords" << endl;
                clipIDToIndex[cm.getID()] = allClips.size();
				clipLinkNameToIndex[cm.getLinkName()] = allClips.size();
				allClips.push_back(cm);
                
                //used for checking overlapping clips
                cloudsClipToFileID[cm.getLinkName()] =fileID;
                fileIDtoCloudsClips[fileID].push_back(cm);
//                cout<<" Adding "<<cm.getLinkName()<<" to  video file : "<<fileID<<endl;
			}
        }

        fcpXML.popTag(); //marker
    }
}

void CloudsFCPParser::getOverlappingClipIDs(){

    vector<CloudsClip>& allClips = getAllClips();

    for(int i=0; i<allClips.size(); i++){
        //this map is populated in parseClipItem() .
        //In terms of FCP XML fileID tag is mapped to link name for clip which comes from the marker tag.
        string fileId = cloudsClipToFileID[allClips[i].getLinkName()];
        
        // fileIDTOCloudsClip is also populated in  parseClipItem() using this map to associate
        //all clips associated with a file id. Here file id is referenced again from the FCP XML

        vector<CloudsClip> clipsFromSameFile = fileIDtoCloudsClips[fileId];
    
        for(int j =0; j<clipsFromSameFile.size(); j++){

            //        cout<<  "Checking for overlaps between "<< allClips[i].getLinkName() << " and "<< clipsFromSameFile[j].getLinkName()<< endl;
            
            if(allClips[i].getLinkName() != clipsFromSameFile[j].getLinkName()){
                
                if( ofRange(allClips[i].startFrame,allClips[i].endFrame).intersects(ofRange(clipsFromSameFile[j].startFrame,clipsFromSameFile[j].endFrame))){
                    
                    overlappingClipsMap[allClips[i].getLinkName()].push_back(clipsFromSameFile[j].getLinkName());
                    
                    //adding the overlapping clip name to a vector in the CloudsClip
                    allClips[i].addOverlappingClipName(clipsFromSameFile[j].getLinkName());

//                    ofLogNotice()<< "OVERLAPPING CLIPS: "<<allClips[i].getLinkName()  << " overlaps with clip "<< clipsFromSameFile[j].getLinkName()<<endl;
                }
                
            }
        }
    }
    
}


void CloudsFCPParser::autolinkSequentialClips(){

    vector<CloudsClip>& allClips = getAllClips();
    
    for(int i=0; i<allClips.size(); i++){
        
        string clipName= allClips[i].getLinkName();
        string nums = "0123456789";
        
        //does the clip name end with a number?
        if( clipName.find_last_of(nums) == clipName.length() -1){
            
            for(int j=0; j<getAllClips().size();j++){

                string compareName =getAllClips()[j].getLinkName();
                
                //do the clips have the same name except for the last char i.e the sequence number? && are they not the same clip?
                if( ! clipName.compare(0, clipName.length() -1, compareName, 0, compareName.length() -1)  &&
                    clipName != compareName){
                    
                    int clipID = clipName.at(clipName.length() -1) - '0';
                    int compareID = compareName.at(compareName.length() -1 ) - '0';
                    
                    if(compareID == ++clipID){
                        
                        //adding link from clip N -1 -> N
                        addLink(allClips[i], getAllClips()[j]);
                        cout<<"Auto linking "<<allClips[i].getLinkName() << " and "<<getAllClips()[j].getLinkName() << " as they are in sequence" <<endl;

                        //removing clip N fromt clip N-1's overlapping list so that they story engine doesnt reject it.
                        getAllClips()[i].removeOverlappingClipName(allClips[j].getLinkName());
                    }
                }
            }
        }
    }
}

void CloudsFCPParser::printSpeakerList(){
	speakerFcpIds.clear();
	for(int i = 0; i < allClips.size(); i++){
		speakerFcpIds.insert(allClips[i].person);
	}
	set<string>::iterator it;
	cout << "ALL SPEAKER IDs" << endl;
	for( it = speakerFcpIds.begin(); it != speakerFcpIds.end(); it++){
		cout << "	" << *it << endl;
	}
}

void CloudsFCPParser::printDichotomyRatios(){
	vector<CloudsDichotomy> dichotomies = CloudsDichotomy::getDichotomies();
	ofBuffer dichotomyScores;
	
	for(int i = 0; i < getContentKeywords().size(); i++){
		vector<CloudsClip> clipsForKeyword = getClipsWithKeyword(getContentKeywords()[i]);
		dichotomyScores.append("Keyword: " + getContentKeywords()[i] + " (" + ofToString(clipsForKeyword.size()) + ") clips\n");
		for(int d = 0; d < dichotomies.size(); d++){
			vector<CloudsClip> leftClips  = getClipsWithKeyword(dichotomies[d].left,  clipsForKeyword);
			vector<CloudsClip> rightClips = getClipsWithKeyword(dichotomies[d].right, clipsForKeyword);
			char report[1024];
			sprintf(report, "	%s %d %.02f%% :: %s %d %.02f%%\n",
					dichotomies[d].left.c_str(),  int(leftClips.size()),  (1.0*leftClips.size()  / clipsForKeyword.size() ),
					dichotomies[d].right.c_str(), int(rightClips.size()), (1.0*rightClips.size() / clipsForKeyword.size()));
			dichotomyScores.append(report);
		}
	}
	
	ofBufferToFile(GetCloudsDataPath() + "DichotomyRatios.txt", dichotomyScores);
}

void CloudsFCPParser::refreshAllKeywords(){

	allKeywords.clear();
	keywordVector.clear();
	contentKeywordVector.clear();
	questionTopicstoClipIndex.clear();
	contentKeywords.clear();

	questionIndeces.clear();
	startQuestionIndeces.clear();

    for(int i = 0; i < allClips.size(); i++){
        
        vector<string>& newKeywords = allClips[i].getKeywords();
        for(int k = 0; k < newKeywords.size(); k++){
            //            if(newKeywords[k].find("?") == string::npos &&
            //               newKeywords[k].find("link:") == string::npos)
            //            {
            contentKeywords[newKeywords[k]]++;
            allKeywords[newKeywords[k]]++;
            //            }
        }
        
        vector<string>& specialKeywords = allClips[i].getSpecialKeywords();
        for(int j=0 ; j<specialKeywords.size();j++){
            allKeywords[specialKeywords[j]]++;
        }
        
        //add index of clips for topic questions
        vector<string>& topicsWithQuestions = allClips[i].getAllTopicsWithQuestion();
        for(int l =0; l< topicsWithQuestions.size(); l++){
            questionTopicstoClipIndex[topicsWithQuestions[l]].push_back(i);
        }
	
		if(allClips[i].hasQuestion()){
			questionIndeces.push_back(i);
			if(allClips[i].hasSpecialKeyword("#start")){
				startQuestionIndeces.push_back(i);
			}
		}
    }
    
    map<string, int>::iterator it;
    for(it = contentKeywords.begin(); it != contentKeywords.end(); it++){
        contentKeywordVector.push_back(it->first);
    }

    for(it = allKeywords.begin(); it != allKeywords.end(); it++){
        keywordVector.push_back(it->first);
    }
}

void CloudsFCPParser::setCombinedVideoDirectory(string directory){
	hasMediaAssetIndeces.clear();
	hasMediaAssetAndQuestionIndeces.clear();
    hasCombinedAndIsStartingClipIndeces.clear();
    
	combinedVideoDirectory = directory;
    //	cout << "Setting combined directory to " << directory << " looking for all clips " << allClips.size() << endl;
	for(int i = 0; i < allClips.size(); i++){
		allClips[i].hasMediaAsset = false;
		allClips[i].combinedVideoPath = directory + "/" + allClips[i].getCombinedMovieFile();
		allClips[i].combinedCalibrationXMLPath = directory + "/" + allClips[i].getCombinedCalibrationXML();
		allClips[i].hasMediaAsset = ofFile(allClips[i].combinedVideoPath).exists() && ofFile(allClips[i].combinedCalibrationXMLPath).exists();
        //        cout << " combined video path is " << allClips[i].combinedVideoPath << " " << allClips[i].combinedCalibrationXMLPath << endl;
        
		if(allClips[i].hasMediaAsset){
			hasMediaAssetIndeces.push_back(i);
			if(allClips[i].hasQuestion()){
				hasMediaAssetAndQuestionIndeces.push_back(i);
				if(allClips[i].hasSpecialKeyword("#start")){
					hasCombinedAndIsStartingClipIndeces.push_back(i);
				}				
			}
            //			cout << "Clip " << allClips[i].getLinkName() << " combined video found!" << endl;
		}
	}
	
	ofLogNotice("CloudsFCPParser::setCombinedVideoDirectory") << "there are " << hasMediaAssetAndQuestionIndeces.size() << " items with questions & combined " << endl;
}

CloudsClip& CloudsFCPParser::getRandomClip(bool hasMediaAsset,
										   bool hasQuestion,
										   bool hasStartQuestion)
{
	if(hasMediaAsset && hasStartQuestion){
		if(hasCombinedAndIsStartingClipIndeces.size() == 0){
			ofLogError("CloudsFCPParser::getRandomClip") << "has no start  clips with combined videos";
			return dummyClip;
		}
//		cout << " has " << hasCombinedAndIsStartingClipIndeces.size() << endl;
		return allClips[ hasCombinedAndIsStartingClipIndeces[ ofRandom(hasCombinedAndIsStartingClipIndeces.size())] ];
	}
	else if(hasMediaAsset && hasQuestion){
		if(hasMediaAssetAndQuestionIndeces.size() == 0){
			ofLogError("CloudsFCPParser::getRandomClip") << "has no questions clips with combined videos";
			return dummyClip;
		}
		cout << " has " << hasMediaAssetAndQuestionIndeces.size() << endl;
		return allClips[ hasMediaAssetAndQuestionIndeces[ ofRandom(hasMediaAssetAndQuestionIndeces.size())] ];
	}
	else if(hasMediaAsset){
		if(hasMediaAssetIndeces.size() == 0){
			ofLogError("CloudsFCPParser::getRandomClip") << "has no combined videos ";
			return dummyClip;
		}
		return allClips[ hasMediaAssetIndeces[ ofRandom(hasMediaAssetIndeces.size())] ];
	}
	else if(hasStartQuestion){
		if(startQuestionIndeces.size() == 0){
			ofLogError("CloudsFCPParser::getRandomClip") << " has no start questions";
			return dummyClip;
		}
        CloudsClip& clip = allClips[ startQuestionIndeces[ ofRandom(startQuestionIndeces.size()) ]];
//        cout << "has a question" << clip.getID() << endl;
		return clip;
	}
	else if(hasQuestion){
		if(questionIndeces.size() == 0){
			ofLogError("CloudsFCPParser::getRandomClip") << " has no questions";
			return dummyClip;
		}
        CloudsClip& clip = allClips[ questionIndeces[ ofRandom(questionIndeces.size()) ]];
		//        cout << "has a question" << clip.getID() << endl;
		return clip;		
	}
	else {
		return allClips[ ofRandom(allClips.size()) ];
	}
}

bool CloudsFCPParser::hasClipWithLinkName(string linkname){
	return clipLinkNameToIndex.find(linkname) != clipLinkNameToIndex.end();
}

bool CloudsFCPParser::hasClipWithID(string ID){
	return clipIDToIndex.find(ID) != clipIDToIndex.end();
}

CloudsClip& CloudsFCPParser::getClipWithLinkName( string linkname ){
    bool dumb;
    return getClipWithLinkName(linkname, dumb);
}

CloudsClip& CloudsFCPParser::getClipWithLinkName( string linkname, bool& found ){
    found = clipLinkNameToIndex.find(linkname) != clipLinkNameToIndex.end();
	if(found){
		return allClips[ clipLinkNameToIndex[linkname] ];
	}
    ofLogError() << "No clip found with link name " << linkname << ". Returning dummy video!";
	return dummyClip;
}

CloudsClip& CloudsFCPParser::getClipWithID( string ID ){
    bool dumb;
    return getClipWithID(ID, dumb);
}

CloudsClip& CloudsFCPParser::getClipWithID( string ID, bool& foundClip ){
    foundClip = clipIDToIndex.find(ID) != clipIDToIndex.end();
	if(foundClip){
		return allClips[ clipIDToIndex[ID] ];
	}
    ofLogError() << "No clip found with ID " << ID << ". Returning dummy video!";
	return dummyClip;
}

void CloudsFCPParser::sortKeywordsByOccurrence(bool byOccurrence){
    if(sortedByOccurrence != byOccurrence){
        sortedByOccurrence = byOccurrence;
        ofSort(keywordVector, *this);
    }
}

float CloudsFCPParser::percentOfClipsLinked(){
	int clipsLinked = 0;
	for(int i = 0; i < allClips.size(); i++){
		if(clipHasLink( allClips[i].getLinkName() )){
			clipsLinked++;
		}
	}
	
	return (1.0*clipsLinked)/allClips.size();
}


vector<string>& CloudsFCPParser::getAllKeywords(){
    return keywordVector;
}
vector<string>& CloudsFCPParser::getContentKeywords(){
    return contentKeywordVector;
}

int CloudsFCPParser::occurrencesOfKeyword(string keyword){
    return allKeywords[keyword];
}

vector<CloudsClip>& CloudsFCPParser::getAllClips(){
    return allClips;
}

vector<CloudsLink>& CloudsFCPParser::getLinksForClip(CloudsClip& clip){
    return linkedConnections[clip.getLinkName()];
}

vector<CloudsLink>& CloudsFCPParser::getLinksForClip(string clipName){
    return linkedConnections[clipName];
}

void CloudsFCPParser::addLink(string sourceName, string targetName){
	CloudsLink l;
	l.sourceName = sourceName;
	l.targetName = targetName;
	l.startFrame = -1;
	l.endFrame = -1;
	addLink(l);
}

void CloudsFCPParser::addLink(CloudsClip& source, CloudsClip& target){
	CloudsLink l;
	l.sourceName = source.getLinkName();
	l.targetName = target.getLinkName();
	l.startFrame = -1;
	l.endFrame = -1;
	addLink(l);
}

void CloudsFCPParser::addLink(CloudsLink& link){
    
	if(linkIsSuppressed(link.sourceName, link.targetName)){
		unsuppressConnection(link);
	}
    
	if( !clipLinksTo(link.sourceName, link.targetName) ){
		linkedConnections[link.sourceName].push_back( link );
	}
}

vector<CloudsLink>& CloudsFCPParser::getSuppressionsForClip(CloudsClip& clip){
    return suppressedConnections[clip.getLinkName()];
}

vector<CloudsLink>& CloudsFCPParser::getSuppressionsForClip(string clipName){
    return suppressedConnections[clipName];
}

int CloudsFCPParser::getNumberOfClipsWithKeyword(string filterWord){
	int keywordsFound = 0;
    for(int c = 0; c < allClips.size(); c++){
		if( ofContains(allClips[c].getKeywords(), filterWord) ){
			keywordsFound++;
		}
	}
	return keywordsFound;
}

vector<CloudsClip> CloudsFCPParser::getClipsWithKeyword(string filterWord){
	return getClipsWithKeyword(filterWord, getAllClips());
}

vector<CloudsClip> CloudsFCPParser::getClipsWithKeyword(string filterWord, vector<CloudsClip>& searchClips){
	vector<string> filter;
	filter.push_back(filterWord);
	return getClipsWithKeyword(filter, searchClips);
}

vector<CloudsClip> CloudsFCPParser::getClipsWithKeyword(const vector<string>& filter){
	return getClipsWithKeyword(filter, getAllClips());
}

vector<CloudsClip> CloudsFCPParser::getClipsWithKeyword(const vector<string>& filter, vector<CloudsClip>& searchClips){
    vector<CloudsClip> filteredMarkers;
	set<string> includedClips;
    for(int c = 0; c < searchClips.size(); c++){
        for(int i = 0; i < filter.size(); i++){
            
            //if filter[i] is #special, use special
            vector<string>& searchVector = (filter[i][0] == '#') ?
				searchClips[c].getSpecialKeywords() : searchClips[c].getKeywords();
            
            if(ofContains(searchVector, filter[i]) &&
			   includedClips.find(searchClips[c].getLinkName()) == includedClips.end())
			{
				includedClips.insert(searchClips[c].getLinkName());
                filteredMarkers.push_back(searchClips[c]);
                break;
            }
        }
    }
    return filteredMarkers;
}

vector<CloudsClip> CloudsFCPParser::getClipsWithQuestionsForTopic(string topic){
    vector<CloudsClip> clips;
    
    if(questionTopicstoClipIndex.find(topic) != questionTopicstoClipIndex.end())
    {
        vector<int> clipIndex =questionTopicstoClipIndex[topic];
        
        for(int i=0; i<clipIndex.size(); i++){
            clips.push_back(allClips[i]);
        }
        
    }
    
    return clips;
}



set<string> CloudsFCPParser::getRelatedKeywords(string filterWord){
	set<string> relatedKeywords;
    
	vector<CloudsClip> relatedClips = getClipsWithKeyword(filterWord);
	for(int i = 0; i < relatedClips.size(); i++){
		vector<string>& keys = relatedClips[i].getKeywords();
		for(int k = 0; k < keys.size(); k++){
			if(keys[k] != filterWord){
				relatedKeywords.insert(keys[k]);
			}
		}
	}
	return relatedKeywords;
}

vector<CloudsClip> CloudsFCPParser::getSharedClips(string keywordA, string keywordB){
	vector<CloudsClip> sharedClips;
    //	cout << "Computing shared clips for " << keywordA << " " << keywordB << endl;
	for(int i = 0; i < allClips.size(); i++){
		if(ofContains(allClips[i].getKeywords(), keywordA) &&
		   ofContains(allClips[i].getKeywords(), keywordB))
		{
            //			cout << "	Adding Clip " << markers[i].getLinkName() << " marker " << i << endl;
			sharedClips.push_back(allClips[i]);
		}
	}
	return sharedClips;
}

int CloudsFCPParser::getNumberOfSharedClips(string keywordA, string keywordB){
	int clipsInCommon = 0;
	for(int i = 0; i < allClips.size(); i++){
		if(ofContains(allClips[i].getKeywords(), keywordA) &&
		   ofContains(allClips[i].getKeywords(), keywordB))
		{
			clipsInCommon++;
		}
	}
	return clipsInCommon;
}

int CloudsFCPParser::getNumberOfSharedKeywords(CloudsClip& a, CloudsClip& b){
	int sharedKeywordCount = 0;
	for(int i = 0; i < a.getKeywords().size(); i++){
		if(ofContains(b.getKeywords(), a.getKeywords()[i])){
			sharedKeywordCount++;
		}
	}
	return sharedKeywordCount;
}

float CloudsFCPParser::getAllClipDuration(){
	float duration = 0;
	for(int i = 0; i < allClips.size(); i++){
		duration += allClips[i].getDuration();
	}
	return duration;
}

int CloudsFCPParser::getNumMetaDataConnections(CloudsClip& source){
	int clipcount = 0;
    string nameA = source.getLinkName();
    vector<CloudsClip> connections = getClipsWithKeyword(source.getKeywords());
    for(int j = 0; j < connections.size(); j++){
        CloudsClip& clipB = connections[j];
        string nameB = connections[j].getLinkName();
        if(nameA != nameB &&
           source.person != clipB.person &&
           !linkIsSuppressed(nameA, nameB) &&
           !clipLinksTo(nameA, nameB) &&
           getNumberOfSharedKeywords(source, clipB) > 1 )
        {
			clipcount++;
        }
    }
	return clipcount;
}

vector<CloudsClip> CloudsFCPParser::getMetaDataConnections(CloudsClip& source){
	vector<CloudsClip> clips;
    string nameA = source.getLinkName();
    vector<CloudsClip> connections = getClipsWithKeyword(source.getKeywords());
    for(int j = 0; j < connections.size(); j++){
        CloudsClip& clipB = connections[j];
        string nameB = connections[j].getLinkName();
        if(nameA != nameB &&
           source.person != clipB.person &&
           !linkIsSuppressed(nameA, nameB) &&
           //           !clipLinksTo(nameA, nameB) &&
           getNumberOfSharedKeywords(source, clipB) > 1 )
        {
            clips.push_back(clipB);
        }
    }
    
	return clips;
}

//Return a vector of keywords shared by both clips
vector<string> CloudsFCPParser::getSharedKeywords(CloudsClip& a, CloudsClip& b){
	vector<string> sharedKeywords;
	for(int i = 0; i < a.getKeywords().size(); i++){
		if(ofContains(b.getKeywords(), a.getKeywords()[i])){
			sharedKeywords.push_back(a.getKeywords()[i]);
		}
	}
	return sharedKeywords;
}

bool CloudsFCPParser::operator()(const string& a, const string& b){
    if(sortedByOccurrence){
        return allKeywords[a] > allKeywords[b];
    }
    else{
        return a < b;
    }
}

void CloudsFCPParser::reciprocateSuppressions(CloudsClip& sourceClip){
    
    //get all suppressions
    vector<CloudsLink>& sourceSuppressions = getSuppressionsForClip(sourceClip.getLinkName());
    //for each...
    for(int i =0; i<sourceSuppressions.size();i++){
        //get the target
        CloudsClip& targetClip = getClipWithLinkName(sourceSuppressions[i].targetName);
        
        //if it's not suppressed
        if( ! linkIsSuppressed( targetClip.getLinkName(), sourceClip.getLinkName()) ){
            suppressConnection(targetClip, sourceClip);
            
            cout<<"Added reciprocal suppression for source: "<<sourceClip.getLinkName()<<" and target: "<<targetClip.getLinkName()<<endl;
        }
    }
}


void CloudsFCPParser:: addIntervention(string clipName, string interventionName){
    clipInterventions[clipName] = interventionName;
}

bool CloudsFCPParser::clipHasIntervention(string clipName){
	return clipInterventions.find(clipName) != clipInterventions.end();
}

void CloudsFCPParser::saveInterventions(string interventionsFile){
    int numClips = 0;
    ofxXmlSettings linksXML;
    map<string, vector<CloudsLink> >::iterator it;
	
	for(int i = 0; i < allClips.size(); i++){
		string clipName = allClips[i].getLinkName();
		
		bool hasIntervention = clipHasIntervention(allClips[i].getLinkName());

        
        if(hasIntervention){
			
			linksXML.addTag("clip");
			linksXML.pushTag("clip", numClips++);
			
			linksXML.addValue("name", clipName);
            

				vector<CloudsLink>& clipLinks = linkedConnections[clipName];
                
				for(int l = 0; l < clipLinks.size(); l++){
					linksXML.addTag("intervention");
					linksXML.pushTag("intervention", l);
					linksXML.addValue("name", clipInterventions[clipName] );;
					linksXML.popTag(); //link!
				}
			
			
			linksXML.popTag();
		}
	}
    
    if(! linksXML.saveFile(interventionsFile) ){
		if(printErrors) ofSystemAlertDialog("UNABLE TO SAVE LINKS. DO NOT PROCEED");
	}
    
}


