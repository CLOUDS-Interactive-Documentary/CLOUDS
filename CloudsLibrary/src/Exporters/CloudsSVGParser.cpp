//
//  CloudsSVGParser.cpp
//  CloudsSVGer
//
//  Created by James George on 4/1/14.
//
//

#include "CloudsSVGParser.h"
#include "ofxXmlSettings.h"
#include "CloudsGlobal.h"

CloudsSVGParser::CloudsSVGParser(){
}

void CloudsSVGParser::loadFile(string path){
	
	ofxXmlSettings graph;
	graph.loadFile(path);
	
	graph.pushTag("svg");
	graph.pushTag("g",1);
	
	int numNodes = graph.getNumTags("circle");
	for(int i = 0; i < numNodes; i++){
		string clipLinkName = graph.getAttribute("circle", "class", "", i);
		if(parser->hasClipWithLinkName(clipLinkName)){
			CloudsClip& clip = parser->getClipWithLinkName(clipLinkName);
			clip.networkPosition2d = ofVec2f(graph.getAttribute("circle","cx", 0, i),
											 graph.getAttribute("circle","cy", 0, i));
//			cout << "Clip position " << clip.getLinkName() << " is " << clip.networkPosition2d << endl;
		}
		else{
			ofLogError("Couldn't find clip " + clipLinkName);
		}
	}
	
	//populate associations
	map<string,string> associations;
	ofBuffer topicAssociations = ofBufferFromFile(GetCloudsDataPath() + "logs/TopicAssociations.txt");
	while(!topicAssociations.isLastLine()){
		
		string line = topicAssociations.getNextLine();
		if(line.find(":") == string::npos){
			cout << "Skipping line " << line << endl;
			continue;
		}
		
		vector<string> association = ofSplitString(line, ":", true, true);
		if(association.size() != 2){
			cout << "line " << line << " has more than one :"<<endl;
			continue;
		}
		
		vector<string> clipcount = ofSplitString(line, "\t",true,true);
		if(clipcount.size() != 2){
			cout << "line " << line << " has more than one tab"<<endl;
			continue;
		}
		
		string associatedKeyword = association[1];
		string subtopic = ofSplitString(association[0],"\t",true,true)[1];
		associations[subtopic] = associatedKeyword;
		cout << "associated " << subtopic << " with " << associatedKeyword << endl;
	}
		
	map<string, vector<ofVec2f> > keywordPositionLists;

	keywordCentroids.clear();
	vector<string>& keywords = parser->getContentKeywords();
	
	
	for(int k = 0; k < keywords.size(); k++){
		
		string keyword = keywords[k];
		if(associations.find(keyword) == associations.end()){
			continue;
		}
		
		//use original keyword to pull clips
		vector<CloudsClip> clips = parser->getClipsWithKeyword(keyword);
		
		//but map it with associated keyword
		keyword = associations[keyword];

		for( int i = 0; i < clips.size(); i++){
			if( clips[i].networkPosition2d != ofVec2f(0, 0) ){
				keywordPositionLists[keyword].push_back( clips[i].networkPosition2d );
			}
		}
	}
	
	//calculate the centers
	map<string, vector<ofVec2f> >::iterator it;
	for(it = keywordPositionLists.begin(); it != keywordPositionLists.end(); it++){
					
		ofVec2f centroid(0,0);
		for( int i = 0; i < it->second.size(); i++){
			centroid += it->second[i];
		}
		
		centroid /= it->second.size();

		keywordCentroids.push_back( make_pair(it->first, centroid) );
	}

	ofVec2f minPoint, maxPoint;
	minPoint = maxPoint = keywordCentroids[0].second;
	//normalize all network positions
	for(int i = 0; i < keywordCentroids.size(); i++){
		maxPoint.x = MAX(maxPoint.x,keywordCentroids[i].second.x);
		maxPoint.y = MAX(maxPoint.y,keywordCentroids[i].second.y);
		
		minPoint.x = MIN(minPoint.x,keywordCentroids[i].second.x);
		minPoint.y = MIN(minPoint.y,keywordCentroids[i].second.y);
	}
	
	for(int i = 0; i < keywordCentroids.size(); i++){
		keywordCentroids[i].second.x = ofMap(keywordCentroids[i].second.x, minPoint.x, maxPoint.x, 0.0, 1.0);
		keywordCentroids[i].second.y = ofMap(keywordCentroids[i].second.y, minPoint.y, maxPoint.y, 0.0, 1.0);
	}
//		cout << "clipName is " << clipLinkName << endl;

}

void CloudsSVGParser::draw(){
	for(int i = 0; i < keywordCentroids.size(); i++){
		if(keywordCentroids[i].second != ofVec2f(0,0)){
			ofDrawBitmapString(keywordCentroids[i].first,
							   keywordCentroids[i].second*ofVec2f(ofGetWidth(), ofGetHeight()));
		}
	}
}

void CloudsSVGParser::exportFile(string path){
	
	ofBuffer textDump;
	textDump.append("<g id=\text\"");
	for(int i = 0; i < keywordCentroids.size(); i++){
	
		char texttag[1024];
		sprintf(texttag,"<text x=\"%f\" y=\"%f\" font-family=\"Verdana\" font-size=\"55\" fill=\"white\">%s</text>\n",
				keywordCentroids[i].second.x * 100, keywordCentroids[i].second.y * 100, keywordCentroids[i].first.c_str());
		textDump.append(texttag);
	}
	textDump.append("</g>");
	
	ofBufferToFile(path, textDump);
}


