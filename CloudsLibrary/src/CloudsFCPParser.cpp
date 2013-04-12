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
    dir.allowExt("xml");
    dir.listDir(xmlDirectory);
    if( dir.numFiles() > 0 ){
        for(int i = 0; i < dir.numFiles(); i++){
            addXMLFile( dir.getPath(i) );
        }
    }
}

void CloudsFCPParser::parseLinks(string linkFile){

    sourceLinks.clear();
    
    ofxXmlSettings linksXML;
    if(linksXML.loadFile(linkFile)){
        int numClips = linksXML.getNumTags("clip");
        for(int i = 0; i < numClips; i++){
            linksXML.pushTag("clip", i);
            string clipName = linksXML.getValue("name", "");
            int numLinks = linksXML.getNumTags("link");
            for(int l = 0; l < numLinks; l++){
                CloudsLink newLink;
                linksXML.pushTag("link", l);
                
                newLink.sourceName = clipName;
                newLink.targetName = linksXML.getValue("target", "");
                newLink.startFrame = linksXML.getValue("startFrame", -1);
                newLink.endFrame   = linksXML.getValue("endFrame", -1);
                
                linksXML.popTag(); //link
                
                sourceLinks[newLink.sourceName].push_back( newLink );
            }
            
            linksXML.popTag(); //clip
        }
    }
}

void CloudsFCPParser::saveLinks(string linkFile){
    ofxXmlSettings linksXML;
    map<string, vector<CloudsLink> >::iterator it;
    int numClips = 0;
    for(it = sourceLinks.begin(); it != sourceLinks.end(); it++){
        linksXML.addTag("clip");
        linksXML.pushTag("clip", numClips++);
        
        linksXML.addValue("name", it->first);
        //linksXML.addValue("meta", sourceLinks[it->first].getMetaInfo() );
        vector<CloudsLink>& clipLinks = it->second;
        for(int l = 0; l < clipLinks.size(); l++){
            
            linksXML.addTag("link");
            linksXML.pushTag("link", l);

            linksXML.addValue("target", clipLinks[l].targetName );
            linksXML.addValue("startFrame", clipLinks[l].startFrame);
            linksXML.addValue("endFrame", clipLinks[l].endFrame);
            
            linksXML.popTag(); //link!
        }
        
        linksXML.popTag(); //clip
    }
    
    linksXML.saveFile(linkFile);
}

void CloudsFCPParser::removeLink(string linkName, int linkIndex){
    if(sourceLinks.find(linkName) != sourceLinks.end() && linkIndex < sourceLinks[linkName].size()){
        sourceLinks[linkName].erase(sourceLinks[linkName].begin() + linkIndex);
    }
    else{
        cout << "failed to remove link " << linkIndex << " from " << linkName << endl;
    }
}

void CloudsFCPParser::removeLink(string linkName, string targetName){
	int linkIndex;
	if(clipLinksTo(linkName, targetName, linkIndex)){
		sourceLinks[linkName].erase(sourceLinks[linkName].begin() + linkIndex);
    }
    else{
        cout << "failed to remove link " << linkIndex << " from " << linkName << endl;
    }
}

bool CloudsFCPParser::clipHasLink(string clipName){
	return sourceLinks.find(clipName) != sourceLinks.end();
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
		vector<CloudsLink>& links = sourceLinks[ clipNameA ];
		for(int i = 0; i < links.size(); i++){
			if(links[ i ].targetName == clipNameB){
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
	
	vector<ClipMarker> clips = getClipsWithKeyword(keywordFilter);
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
        
        ClipMarker cm;
        cm.startFrame = fcpXML.getValue("in", 0);
        cm.endFrame = fcpXML.getValue("out", 0);
        string comment = fcpXML.getValue("comment", "");
        //validate
        if(comment != "" && cm.endFrame - cm.startFrame > 1 && cm.endFrame > 0){
            cm.name = fcpXML.getValue("name", "");
            cm.person = currentName;
			cm.fcpFileId = fileID;
			cm.clip = clipFileName;
			cm.filePath = clipFilePath;
			
			if( markerLinkNames.find(cm.getLinkName()) != markerLinkNames.end() ){
				ofLogError() << "DUPLICATE CLIP " << cm.getLinkName() << " " << cm.getMetaInfo();
				ofLogError() << "	EXISTING CLIP INFO " << getClipWithLinkName(cm.getLinkName()).getMetaInfo();
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
				markers.push_back(cm);
			}
        }
        fcpXML.popTag(); //marker
    }
    keywordsDirty = true;
}

ClipMarker CloudsFCPParser::getClipWithLinkName( string linkname ){
	for(int i = 0; i < markers.size(); i++){
		if(markers[i].getLinkName() == linkname){
			return markers[i];
		}
	}
	return ClipMarker();
}

void CloudsFCPParser::sortKeywordsByOccurrence(bool byOccurrence){
    if(sortedByOccurrence != byOccurrence){
        sortedByOccurrence = byOccurrence;
        ofSort(keywordVector, *this);
    }
}

float CloudsFCPParser::percentOfClipsLinked(){
	int clipsLinked = 0;
	for(int i = 0; i < markers.size(); i++){
		if(clipHasLink( markers[i].getLinkName() )){
			clipsLinked++;
		}
	}
	
	return (1.0*clipsLinked)/markers.size();
}

vector<string>& CloudsFCPParser::getAllKeywords(){
    if(keywordsDirty){
        refreshKeywordVector();
    }
    return keywordVector;
}

int CloudsFCPParser::occurrencesOfKeyword(string keyword){
    return allKeywords[keyword];
}

vector<ClipMarker>& CloudsFCPParser::getAllClips(){
    return markers;
}

vector<CloudsLink>& CloudsFCPParser::getLinksForClip(ClipMarker& clip){
    return sourceLinks[clip.getLinkName()];
}

vector<CloudsLink>& CloudsFCPParser::getLinksForClip(string clipName){
    return sourceLinks[clipName];
}

void CloudsFCPParser::addLink(CloudsLink& link){
	if( !clipLinksTo(link.sourceName, link.targetName) ){
		sourceLinks[link.sourceName].push_back( link );
	}
}

int CloudsFCPParser::getNumberOfClipsWithKeyword(string filterWord){
	int keywordsFound = 0;
    for(int c = 0; c < markers.size(); c++){
		if( ofContains(markers[c].keywords, filterWord) ){
			keywordsFound++;
		}
	}
	return keywordsFound;
}

vector<ClipMarker> CloudsFCPParser::getClipsWithKeyword(string filterWord){
	vector<string> filter;
	filter.push_back(filterWord);
	return getClipsWithKeyword(filter);
}

vector<ClipMarker> CloudsFCPParser::getClipsWithKeyword(const vector<string>& filter){
    vector<ClipMarker> filteredMarkers;
	set<string> includedClips;
    for(int c = 0; c < markers.size(); c++){
        for(int i = 0; i < filter.size(); i++){
            if(ofContains(markers[c].keywords, filter[i]) &&
			   includedClips.find(markers[c].getLinkName()) == includedClips.end()){
				includedClips.insert(markers[c].getLinkName());
                filteredMarkers.push_back(markers[c]);
                break;
            }
        }
    }
    return filteredMarkers;
}

set<string> CloudsFCPParser::getRelatedKeywords(string filterWord){
	set<string> relatedKeywords;
	vector<ClipMarker> relatedClips = getClipsWithKeyword(filterWord);
	for(int i = 0; i < relatedClips.size(); i++){
		vector<string>& keys = relatedClips[i].keywords;
		for(int k = 0; k < keys.size(); k++){
			relatedKeywords.insert(keys[k]);
		}
	}
	return relatedKeywords;
}

vector<ClipMarker> CloudsFCPParser::getSharedClips(string keywordA, string keywordB){
	vector<ClipMarker> sharedClips;
//	cout << "Computing shared clips for " << keywordA << " " << keywordB << endl;
	for(int i = 0; i < markers.size(); i++){
		if(ofContains(markers[i].keywords, keywordA) &&
		   ofContains(markers[i].keywords, keywordB))
		{
//			cout << "	Adding Clip " << markers[i].getLinkName() << " marker " << i << endl;
			sharedClips.push_back(markers[i]);
		}
	}
	return sharedClips;
}

int CloudsFCPParser::getNumberOfSharedClips(string keywordA, string keywordB){
	int clipsInCommon = 0;
	for(int i = 0; i < markers.size(); i++){
		if(ofContains(markers[i].keywords, keywordA) &&
		   ofContains(markers[i].keywords, keywordB))
		{
			clipsInCommon++;
		}
	}
	return clipsInCommon;
}

//
//Return a vector of keywords shared by both clips
vector<string> CloudsFCPParser::getSharedKeywords(ClipMarker& a, ClipMarker& b){
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