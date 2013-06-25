//
//  CloudsFCPParser.cpp
//  example-empty
//
//  Created by James George on 2/4/13.
//
//

#include "CloudsFCPParser.h"

CloudsFCPParser::CloudsFCPParser(){
    keywordsDirty = false;
    sortedByOccurrence = false;
}

void CloudsFCPParser::loadFromFiles(){
	if(ofDirectory("../../../CloudsData/").exists()){
		setup("../../../CloudsData/fcpxml/");
		parseLinks("../../../CloudsData/links/clouds_link_db.xml");
        parseClusterMap("../../../CloudsData/gephi/CLOUDS_test_5_26_13.SVG");
	}
	else{
		setup("xml");
		parseLinks("clouds_link_db.xml");
        parseClusterMap("CLOUDS_test_5_26_13.SVG");
	}
}

void CloudsFCPParser::setup(string directory){
    xmlDirectory = directory;
    refreshXML();
	
    map<string, int>::iterator it;
    for(it = allKeywords.begin(); it != allKeywords.end(); it++){
        //        cout << it->first << ": " << it->second << endl;
    }
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
	
    int totalLinks =0;
    ofxXmlSettings linksXML;
    if(linksXML.loadFile(linkFile)){
        int numClips = linksXML.getNumTags("clip");
        for(int i = 0; i < numClips; i++){
            linksXML.pushTag("clip", i);
            string clipName = linksXML.getValue("name", "");
            int numLinks = linksXML.getNumTags("link");
            int startQuestion = linksXML.getNumTags("startingQuestion");
			if(numLinks > 0){
                //				cout << "clip " << clipName << " had no links!" << endl;
				for(int l = 0; l < numLinks; l++){
					CloudsLink newLink;
					linksXML.pushTag("link", l);
					
					newLink.sourceName = clipName;
					newLink.targetName = linksXML.getValue("target", "");
					newLink.startFrame = linksXML.getValue("startFrame", -1);
					newLink.endFrame   = linksXML.getValue("endFrame", -1);
					
					linksXML.popTag(); //link
					
					linkedConnections[newLink.sourceName].push_back( newLink );
				}
			}
			
			int numSuppressed = linksXML.getNumTags("suppress");
			if(numSuppressed > 0){
				for(int l = 0; l < numSuppressed; l++){
					CloudsLink newLink;
					linksXML.pushTag("suppress", l);
					
					newLink.sourceName = clipName;
					newLink.targetName = linksXML.getValue("target", "");
					newLink.startFrame = linksXML.getValue("startFrame", -1);
					newLink.endFrame   = linksXML.getValue("endFrame", -1);
					
					linksXML.popTag(); //link
                    totalLinks++;
//                    cout<<"suppressed"<<newLink.targetName<<" for "<<clipName<<endl;
					suppressedConnections[newLink.sourceName].push_back( newLink );
				}
			}
			
            if(startQuestion > 0){
                string question = linksXML.getValue("startingQuestion", "");
                bool hasQuestionClip;
                CloudsClip& c =  getClipWithLinkName(clipName,hasQuestionClip);
                if(hasQuestionClip){
                    c.setStartingQuestion(question);
                    questionIds.push_back( c.getID() );                    
                    cout << c.getID() << " has question: " << c.getStartingQuestion() << endl;
                }
                else{
                    ofLogError("CloudsFCPParser::parseLinks") << clipName << " not found! has question " << question;
                }
            }
			
            linksXML.popTag(); //clip
        }
    }
    cout<<"total Suppressions:"<<totalLinks<<endl;
}


void CloudsFCPParser::saveLinks(string linkFile){
    ofxXmlSettings linksXML;
    map<string, vector<CloudsLink> >::iterator it;
    int numClips = 0;
	
	for(int i = 0; i < allClips.size(); i++){
		string clipName = allClips[i].getLinkName();
		
		bool hasLink = clipHasLink( allClips[i]);
		
		bool hasSuppressed = clipHasSuppressions( allClips[i] );
        
        bool hasStartingQuestion = clipHasStartingQuestions(allClips[i]);
		
        if(hasLink || hasSuppressed || hasStartingQuestion){
			
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
			

			linksXML.popTag();
		}
	}
	
    
    linksXML.saveFile(linkFile);
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

void CloudsFCPParser::suppressConnection(CloudsLink& link){
	if(!linkIsSuppressed(link.sourceName, link.targetName)){
		cout << "Suppressed connection " << link.sourceName << " >> " << link.targetName << endl;
		suppressedConnections[link.sourceName].push_back(link);
	}
}

void CloudsFCPParser::unsuppressConnection(string linkName, int linkIndex){
	if(suppressedConnections.find(linkName) != suppressedConnections.end()){
		cout << "Unsuppressed connection " << linkName << " >> " << linkIndex << endl;
		suppressedConnections[linkName].erase( suppressedConnections[linkName].begin() + linkIndex );
	}
}

void CloudsFCPParser::unsuppressConnection(string linkName, string targetName){
	if(clipHasSuppressions(linkName)){
		int linkIndex;
		if(linkIsSuppressed(linkName, targetName,linkIndex)){
			cout << "Unsuppressed connection " << linkName << " >> " << targetName << endl;
			suppressedConnections[linkName].erase( suppressedConnections[linkName].begin() + linkIndex );
		}
	}
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
				cm.keywords = ofSplitString(keywordString, ",",true,true);
				for(int k = 0; k < cm.keywords.size(); k++){
					if(cm.keywords[k].find("?") == string::npos &&
					   cm.keywords[k].find("link:") == string::npos)
					{
						allKeywords[cm.keywords[k]]++;
					}
				}
                //            cout << "       added marker: \"" << cm.name << "\" with [" << cm.keywords.size() << "] keywords" << endl;
                clipIDToIndex[cm.getID()] = allClips.size();
				clipLinkNameToIndex[cm.getLinkName()] = allClips.size();
				allClips.push_back(cm);
			}
        }
        fcpXML.popTag(); //marker
    }
    keywordsDirty = true;
}


void CloudsFCPParser::setCombinedVideoDirectory(string directory){
	hasCombinedVideoIndeces.clear();
	hasCombinedVideoAndQuestionIndeces.clear();
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
//			cout << "Clip " << allClips[i].getLinkName() << " combined video found!" << endl;
		}
	}
	
	ofLogNotice("CloudsFCPParser::setCombinedVideoDirectory") << "there are " << hasCombinedVideoAndQuestionIndeces.size() << " items with questions & combined " << endl;
}

CloudsClip& CloudsFCPParser::getRandomClip(bool mustHaveCombinedVideoFile, bool mustHaveQuestion){
	if(mustHaveCombinedVideoFile && mustHaveQuestion){
		if(hasCombinedVideoAndQuestionIndeces.size() == 0){
			ofLogError() << "CloudsFCPParser::getRandomClip has no questions clips with combined videos";
			return dummyClip;
		}
		return allClips[ hasCombinedVideoAndQuestionIndeces[ofRandom(hasCombinedVideoAndQuestionIndeces.size())] ];
	}
	else if(mustHaveCombinedVideoFile){
		if(hasCombinedVideoIndeces.size() == 0){
			ofLogError() << "CloudsFCPParser::getRandomClip has no combined videos ";
			return dummyClip;
		}
		return allClips[ hasCombinedVideoIndeces[ofRandom(hasCombinedVideoIndeces.size())] ];
	}
	else if(mustHaveQuestion){
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
	
	getAllKeywords();
	
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


//void CloudsFCPParser::suppressConnection(CloudsClip& a, CloudsClip& b){
//
//}
//
//void CloudsFCPParser::unsuppressConnection(CloudsClip& a, CloudsClip& b){
//
//}
//
//bool CloudsFCPParser::isConnectionSuppressed(CloudsClip& a, CloudsClip& b){
//
//}

vector<string>& CloudsFCPParser::getAllKeywords(){
    if(keywordsDirty){
        refreshKeywordVector();
    }
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

void CloudsFCPParser::addLink(CloudsLink& link){
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
		if( ofContains(allClips[c].keywords, filterWord) ){
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

vector<CloudsClip> CloudsFCPParser::getClipsWithKeyword(const vector<string>& filter){
    vector<CloudsClip> filteredMarkers;
	set<string> includedClips;
    for(int c = 0; c < allClips.size(); c++){
        for(int i = 0; i < filter.size(); i++){
            if(ofContains(allClips[c].keywords, filter[i]) &&
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
		vector<string>& keys = relatedClips[i].keywords;
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
		if(ofContains(allClips[i].keywords, keywordA) &&
		   ofContains(allClips[i].keywords, keywordB))
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
		if(ofContains(allClips[i].keywords, keywordA) &&
		   ofContains(allClips[i].keywords, keywordB))
		{
			clipsInCommon++;
		}
	}
	return clipsInCommon;
}

int CloudsFCPParser::getNumberOfSharedKeywords(CloudsClip& a, CloudsClip& b){
	int sharedKeywordCount = 0;
	for(int i = 0; i < a.keywords.size(); i++){
		if(ofContains(b.keywords, a.keywords[i])){
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
    vector<CloudsClip> connections = getClipsWithKeyword(source.keywords);
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
    vector<CloudsClip> connections = getClipsWithKeyword(source.keywords);
    for(int j = 0; j < connections.size(); j++){
        CloudsClip& clipB = connections[j];
        string nameB = connections[j].getLinkName();
        if(nameA != nameB &&
           source.person != clipB.person &&
           !linkIsSuppressed(nameA, nameB) &&
           !clipLinksTo(nameA, nameB) &&
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
	for(int i = 0; i < a.keywords.size(); i++){
		if(ofContains(b.keywords, a.keywords[i])){
			sharedKeywords.push_back(a.keywords[i]);
		}
	}
	return sharedKeywords;
}

void CloudsFCPParser::refreshKeywordVector(){
    keywordVector.clear();
    map<string, int>::iterator it;
    for(it = allKeywords.begin(); it != allKeywords.end(); it++){
        keywordVector.push_back(it->first);
    }
    keywordsDirty = false;
}

bool CloudsFCPParser::operator()(const string& a, const string& b){
    if(sortedByOccurrence){
        return allKeywords[a] > allKeywords[b];
    }
    else{
        return a < b;
    }
}

vector<string> CloudsFCPParser::getClustersForPerson(string personName){
    
}


