//
//  CloudsFCPParser.cpp
//  example-empty
//
//  Created by James George on 2/4/13.
//
//

#include "CloudsFCPParser.h"
#include "CloudsGlobal.h"

bool distanceSort(pair<string,float> a, pair<string,float> b ){
    return a.second > b.second;
}

CloudsFCPParser::CloudsFCPParser(){
	printErrors = false;
    sortedByOccurrence = false;
	backupTimeInterval = 60*2;
	lastBackupTime = -backupTimeInterval;
}

void CloudsFCPParser::loadFromFiles(){
    setup(getDataPath() + "fcpxml");
    parseLinks(getDataPath() + "links/clouds_link_db.xml");
    parseClusterMap(getDataPath() + "gephi/CLOUDS_test_5_26_13.SVG");
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
	hasCombinedVideoIndeces.clear();
	
    linkedConnections.clear();
    suppressedConnections.clear();
    sourceSupression.clear();
    keyThemes.clear();
    tagToKeyTheme.clear();
    
    dir.allowExt("xml");
    dir.listDir(xmlDirectory);
    if( dir.numFiles() > 0 ){
        for(int i = 0; i < dir.numFiles(); i++){
            addXMLFile( dir.getPath(i) );
        }
    }
	
    refreshAllKeywords();
}

void CloudsFCPParser::parseClusterMap(string mapFile){
    float maxCx=0;
    float maxCy=0;
    float minCx=-1;
    float minCy=-1;
    float maxR=0;
    float minR=80;
    
    ofxXmlSettings mapsXML;
    
    if(mapsXML.loadFile(mapFile)){
        mapsXML.pushTag("svg");
        
        int numClips = mapsXML.getNumTags("g");
        
        mapsXML.pushTag("g",1);
        int numCircles = mapsXML.getNumTags("circle");
        map<string, vector<CloudsLink> >::iterator it;
        for(int j=0; j<numCircles;j++){
            string circleName;
            circleName = mapsXML.getAttribute("circle", "class",circleName, j);
            
            if(clipIDToIndex.find(circleName) == clipIDToIndex.end()){
                ofLogError() << "Clip " << circleName << " not found in cluster map";
                continue;
            }
            
            CloudsClip& clip = allClips[ clipIDToIndex[circleName] ];
            clip.cluster.Id = clip.getID();
            
            clip.cluster.hexColor = mapsXML.getAttribute("circle","fill",clip.cluster.hexColor,j);
            clip.cluster.hexColor.erase(clip.cluster.hexColor.begin()); //remove #
            unsigned int colorHex;
            std::stringstream ss;
            ss << std::hex << clip.cluster.hexColor;
            ss >> colorHex;
            
            clip.cluster.Color.setHex(colorHex);
            
            clusterMapColors.insert(clip.cluster.hexColor);
            //			if(clip.cluster.Color == ofColor(255)){
            //                cout<<clip.cluster.Id<<endl;
            //            }
            string radius;
            radius = mapsXML.getAttribute("circle", "r", radius,j);
            clip.cluster.Radius = ofToFloat(radius);
            maxR = MAX(maxR,clip.cluster.Radius);
            minR = MIN(minR,clip.cluster.Radius);
            
            string cx,cy;
            cx = mapsXML.getAttribute("circle", "cx", cx,j);
            cy = mapsXML.getAttribute("circle", "cy", cy,j);
            clip.cluster.Centre = ofVec2f(ofToFloat(cx),ofToFloat(cy));
            
            maxCx = MAX(maxCx,clip.cluster.Centre.x);
            minCx = MIN(minCx,clip.cluster.Centre.x);
            maxCy = MAX(maxCy,clip.cluster.Centre.y);
            minCy = MIN(minCy,clip.cluster.Centre.y);
        }
        
        for(int i = 0; i < allClips.size(); i++){
            if(allClips[i].cluster.Id != ""){
                allClips[i].cluster.Centre.x = ofMap(allClips[i].cluster.Centre.x, minCx, maxCx, 0, 1);
                allClips[i].cluster.Centre.y = ofMap(allClips[i].cluster.Centre.y, minCy, maxCy, 0, 1);
                allClips[i].cluster.Radius = ofMap(allClips[i].cluster.Radius, minR, maxR, 0, 1);
            }
            //            else{
            //                cout<<"ERROR CLIP NOT FOUND IN MAP:"<<allClips[i].getLinkName()<<endl;
            //            }
            
        }
        //		cout<<minR<<","<<maxR<<endl;
        //		cout<<maxCx<<","<<minCx<<"::"<<maxCy<<","<<minCy;
        mapsXML.popTag();//g
        
        mapsXML.popTag(); //svg
    }
}

void CloudsFCPParser::parseLinks(string linkFile){
    
    linkedConnections.clear();
	suppressedConnections.clear();
	questionIds.clear();
	
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
		
		if(startQuestion > 0){
			string question = linksXML.getValue("startingQuestion", "");
			bool hasQuestionClip;
			CloudsClip& c = getClipWithLinkName(clipName,hasQuestionClip);
			if(hasQuestionClip){
				c.setStartingQuestion(question);
				questionIds.push_back( c.getID() );
                //				cout << c.getID() << " has question: " << c.getStartingQuestion() << endl;
			}
			else{
				ofLogError("CloudsFCPParser::parseLinks") << clipName << " not found! has question " << question;
			}
		}
        
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

void CloudsFCPParser::populateKeywordCentroids(){
    for(int k =0; k < getAllKeywords().size(); k++){
        vector<CloudsClip> clips = getClipsWithKeyword(getAllKeywords()[k]);
        
        float numClips =0;
        ofVec2f centroid;
        
        for( int i=0; i<clips.size(); i++){
            if(clips[i].cluster.Centre != ofVec2f(-1, -1)){
                centroid += clips[i].cluster.Centre;
                numClips++;
            }
        }
        
        centroid /= numClips;
        keywordCentroids.push_back(make_pair(getAllKeywords()[k], centroid));
        keywordCentroidIndex[getAllKeywords()[k]]= k;
    }
    
    
}

void CloudsFCPParser::calculateCohesionMedianForKeywords(){
    for( int i=0; i< getAllKeywords().size(); i++){
        string currentKeyword = getAllKeywords()[i];
        ofVec2f keywordCentroid = getKeywordCentroid(currentKeyword);
        vector<CloudsClip> clips = getClipsWithKeyword(currentKeyword);
        
        vector<float> distancesPerClip;
        float maxDistance = 0;
        float minDistance = INT_MAX;
        float numClips  =0;
        float totalDistance = 0;
        
        for (int k=0; k<clips.size(); k++) {
            if(clips[k].cluster.Centre != ofVec2f(-1,-1)){
                float distance = keywordCentroid.distance(clips[k].cluster.Centre);
                totalDistance += distance;
                distancesPerClip.push_back(distance);
                maxDistance = MAX(maxDistance,distance);
                minDistance = MIN(minDistance,distance);
                numClips++;
            }
            
            float avgDistance = totalDistance / numClips;
            
            if(distancesPerClip.begin() != distancesPerClip.end()){
                std::sort(distancesPerClip.begin(), distancesPerClip.end());
                float medianValue = distancesPerClip[distancesPerClip.size()/2];
                keywordCohesionMap[currentKeyword] = medianValue / maxDistance;
            }
        }
        
    }
}
float CloudsFCPParser::getCohesionIndexForKeyword(string keyword){
    
    if(keywordCohesionMap.find(keyword) != keywordCohesionMap.end()){
        return keywordCohesionMap[keyword];
    }
    ofLogError()<<"Couldnt find cohesion index for keyword: "<<endl;
    return 0;
    
}
float CloudsFCPParser::getDistanceFromAdjacentKeywords(string keyword1, string keyword2){
    
    return getKeywordCentroid(keyword1).distance(getKeywordCentroid(keyword2));
}

ofVec2f CloudsFCPParser::getKeywordCentroid(string keyword){
    
    ofVec2f centroid;
//    int index = keywordCentroidIndex[keyword];
	int index = getCentroidMapIndex(keyword);
    if(index != -1){
        return keywordCentroids[index].second;
    }
    ofLogError()<<"No centroid found for keyword: "<< keyword<<endl;
    
    return ofVec2f(-1, -1);
}

int CloudsFCPParser::getCentroidMapIndex(string keyword){
    if(keywordCentroidIndex.find(keyword) != keywordCentroidIndex.end()){
        return keywordCentroidIndex[keyword];
    }
    ofLogError()<<"Couldnt find index for keyword: "<<keyword<<endl;
	return -1;
}

vector<string> CloudsFCPParser::getAdjacentKeywords( string currentKeyword , int numOfDesiredKeywords){
    string keyword = "";
    ofVec2f centroid;
    vector<pair<string, float> > distancePair;
    
    centroid = getKeywordCentroid(currentKeyword);
    
    for (int j=0; j < keywordCentroids.size(); j++) {
        
        float distance = centroid.distance(keywordCentroids[j].second);
        distancePair.push_back(make_pair(keywordCentroids[j].first, distance));
    }
    
    sort(distancePair.begin(), distancePair.end(),distanceSort);
    
    vector<string> adjacentKeywords;
    numOfDesiredKeywords =MIN(distancePair.size(),numOfDesiredKeywords);
    
    for (int k=0 ; k<numOfDesiredKeywords; k++) {
        adjacentKeywords.push_back(distancePair[k].first);
    }
    
    return adjacentKeywords;
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
        bool hasStartingQuestion = clipHasStartingQuestions(allClips[i]);
		bool hasRevokedKeywords = clipHasRevokedKeywords(allClips[i]);
        bool hasAdditionalKeywords = clipHasAdditionalKeywords(allClips[i]);
        
        if(hasLink || hasSuppressed || hasStartingQuestion || hasRevokedKeywords || hasAdditionalKeywords){
			
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
            
            if(hasStartingQuestion){
                string startQuestion = allClips[i].getStartingQuestion();
                linksXML.addValue("startingQuestion", startQuestion);
            }
            
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

bool CloudsFCPParser::clipHasStartingQuestions(CloudsClip& clip){
    return clip.hasStartingQuestion();
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
				cm.color.r = fcpXML.getValue("color:red", 0);
				cm.color.g = fcpXML.getValue("color:green", 0);
				cm.color.b = fcpXML.getValue("color:blue", 0);
				string keywordString = ofToLower( fcpXML.getValue("comment", "") );
				ofStringReplace(keywordString, "\n", ",");
                vector<string> fcpKeywords = ofSplitString(keywordString, ",",true,true);
				cm.setOriginalKeywords(fcpKeywords);
                
                //            cout << "       added marker: \"" << cm.name << "\" with [" << cm.keywords.size() << "] keywords" << endl;
                clipIDToIndex[cm.getID()] = allClips.size();
				clipLinkNameToIndex[cm.getLinkName()] = allClips.size();
				allClips.push_back(cm);
			}
        }
        fcpXML.popTag(); //marker
    }
}

void CloudsFCPParser::refreshAllKeywords(){
	
	allKeywords.clear();
	keywordVector.clear();
	
    for(int i = 0; i < allClips.size(); i++){
        
        vector<string>& newKeywords = allClips[i].getKeywords();
        for(int k = 0; k < newKeywords.size(); k++){
            //            if(newKeywords[k].find("?") == string::npos &&
            //               newKeywords[k].find("link:") == string::npos)
            //            {
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
        
        
    }
    
    map<string, int>::iterator it;
    for(it = allKeywords.begin(); it != allKeywords.end(); it++){
        keywordVector.push_back(it->first);
    }
    
}

void CloudsFCPParser::setCombinedVideoDirectory(string directory){
	hasCombinedVideoIndeces.clear();
	hasCombinedVideoAndQuestionIndeces.clear();
    hasCombinedAndIsStartingClipIndeces.clear();
    
	combinedVideoDirectory = directory;
    //	cout << "Setting combined directory to " << directory << " looking for all clips " << allClips.size() << endl;
	for(int i = 0; i < allClips.size(); i++){
		allClips[i].hasCombinedVideo = false;
		allClips[i].combinedVideoPath = directory + "/" + allClips[i].getCombinedMovieFile();
		allClips[i].combinedCalibrationXMLPath = directory + "/" + allClips[i].getCombinedCalibrationXML();
		allClips[i].hasCombinedVideo = ofFile(allClips[i].combinedVideoPath).exists() && ofFile(allClips[i].combinedCalibrationXMLPath).exists();
        //        cout << " combined video path is " << allClips[i].combinedVideoPath << " " << allClips[i].combinedCalibrationXMLPath << endl;
        
		if(allClips[i].hasCombinedVideo){
			hasCombinedVideoIndeces.push_back(i);
			if(allClips[i].hasStartingQuestion()){
				hasCombinedVideoAndQuestionIndeces.push_back(i);
			}
            if(allClips[i].hasSpecialKeyword("#start")){
                hasCombinedAndIsStartingClipIndeces.push_back(i);
            }
            //			cout << "Clip " << allClips[i].getLinkName() << " combined video found!" << endl;
		}
	}
	
	ofLogNotice("CloudsFCPParser::setCombinedVideoDirectory") << "there are " << hasCombinedVideoAndQuestionIndeces.size() << " items with questions & combined " << endl;
}

CloudsClip& CloudsFCPParser::getRandomClip(bool mustHaveCombinedVideoFile, bool startingClip){
	if(mustHaveCombinedVideoFile && startingClip){
		if(hasCombinedAndIsStartingClipIndeces.size() == 0){
			ofLogError() << "CloudsFCPParser::getRandomClip has no start clips clips with combined videos";
			return dummyClip;
		}
		return allClips[ hasCombinedAndIsStartingClipIndeces[ofRandom(hasCombinedAndIsStartingClipIndeces.size())] ];
	}
	else if(mustHaveCombinedVideoFile){
		if(hasCombinedVideoIndeces.size() == 0){
			ofLogError() << "CloudsFCPParser::getRandomClip has no combined videos ";
			return dummyClip;
		}
		return allClips[ hasCombinedVideoIndeces[ofRandom(hasCombinedVideoIndeces.size())] ];
	}
	else if(startingClip){
		if(questionIds.size() == 0){
			ofLogError("CloudsFCPParser::getRandomClip") << " has no questions";
			return dummyClip;
		}
        CloudsClip& clip = getClipWithID( questionIds[ ofRandom(questionIds.size()) ] ) ;
        cout << "has a question" << clip.getID() << endl;
		return ( clip );
	}
	else {
		return allClips[ ofRandom(allClips.size())];
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

void CloudsFCPParser::populateKeyThemes(){
	keyThemes.clear();
	
	keyThemes.insert("computation");
	keyThemes.insert("simulation");
	keyThemes.insert("form");
	keyThemes.insert("physics");
	keyThemes.insert("biology");
	keyThemes.insert("vision");
	keyThemes.insert("people");
	keyThemes.insert("social networks");
	keyThemes.insert("toolkit");
    
	populateKeyThemes(keyThemes);
}

void CloudsFCPParser::populateKeyThemes(set<string>& themes){
	
	keyThemes = themes;
	
	//search through all tags to find the shortest path to key
	tagToKeyTheme.clear();
	
	//refreshKeywordVector();
	refreshAllKeywords();
	
	for(int i = 0; i < keywordVector.size(); i++){
        
		string closestKeyword = closestKeyThemeToTag(keywordVector[i]);
		tagToKeyTheme[ keywordVector[i] ] = closestKeyword;
		cout << "Closest key theme to '" << keywordVector[i] << "' is '" << closestKeyword << "'" << endl;
	}
}

string CloudsFCPParser::closestKeyThemeToTag(string searchTag){
	
	vector<string> keys;
	set<string> usedKeys;
	keys.push_back(searchTag);
	
	while(!keys.empty()){
		//get one key
		string key = keys[0];
		keys.erase(keys.begin());
		
		//if it's in the key themes you found it
		if(keyThemes.find(key) != keyThemes.end()){
			return key;
		}
		//otherwise add it to the used bin
		usedKeys.insert(key);
		
		//and all the related keys that we haven't traversed yet
		set<string> tokens = getRelatedKeywords(key);
		set<string>::iterator it;
		for( it = tokens.begin(); it != tokens.end(); it++ ){
			if(usedKeys.find(*it) == usedKeys.end() && !ofContains(keys, *it)){
				keys.push_back(*it);
			}
		}
		
		//and search the oldest in the queue on the next iteration...
	}
	
	//didn't find a match!
	return "";
}


string CloudsFCPParser::getKeyThemeForTag(string tag){
	if(tagToKeyTheme.find(tag) == tagToKeyTheme.end()){
		ofLogError() << "couldn't find key theme for tag " << tag;
		return "";
	}
	return tagToKeyTheme[ tag ];
}

vector<string>& CloudsFCPParser::getAllKeywords(){
    return keywordVector;
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
	vector<string> filter;
	filter.push_back(filterWord);
	return getClipsWithKeyword(filter);
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


vector<CloudsClip> CloudsFCPParser::getClipsWithKeyword(const vector<string>& filter){
    vector<CloudsClip> filteredMarkers;
	set<string> includedClips;
    for(int c = 0; c < allClips.size(); c++){
        for(int i = 0; i < filter.size(); i++){
            
            //if filter[i] is #special, use special
            vector<string>& searchVector = (filter[i][0] == '#') ?
            allClips[c].getSpecialKeywords() : allClips[c].getKeywords();
            
            if(ofContains(searchVector, filter[i]) &&
			   includedClips.find(allClips[c].getLinkName()) == includedClips.end()){
				includedClips.insert(allClips[c].getLinkName());
                filteredMarkers.push_back(allClips[c]);
                break;
            }
        }
    }
    return filteredMarkers;
}

set<string> CloudsFCPParser::getRelatedKeywords(string filterWord){
	set<string> relatedKeywords;
    
	vector<CloudsClip> relatedClips = getClipsWithKeyword(filterWord);
	for(int i = 0; i < relatedClips.size(); i++){
		vector<string>& keys = relatedClips[i].getKeywords();
		for(int k = 0; k < keys.size(); k++){
			relatedKeywords.insert(keys[k]);
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

//void CloudsFCPParser::refreshKeywordVector(){
//}

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
            
            cout<<"Added reciprocal suppresion for source: "<<sourceClip.getLinkName()<<" and target: "<<targetClip.getLinkName()<<endl;
        }
    }
}

vector<string> CloudsFCPParser::getClustersForPerson(string personName){
    
}


