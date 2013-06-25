//
//  CloudsVisualizationStandin.cpp
//  CLOUDS
//
//  Created by James George on 4/23/13.
//
//

#include "CloudsVisualSystemStandin.h"
#include "ofxXmlSettings.h"
#include "CloudsStoryEngine.h"

CloudsVisualSystemStandin::CloudsVisualSystemStandin(){
	systemSelected = false;
	currentVisualSystem = NULL;
}

string CloudsVisualSystemStandin::getSystemName(){
	return "STAND IN SYSTEM";
}

void CloudsVisualSystemStandin::setup(){
	
	//pasre the XML file to get videos
	ofxXmlSettings settings;
	if(!settings.loadFile("../../../CloudsData/visualsystems/visualsystems.xml")){
		ofLogError() << "failed to load stand in visual systems";
		return;
	}
	settings.pushTag("broll");
	
	int numSystems = settings.getNumTags("system");
	for(int i = 0; i < numSystems; i++){
		settings.pushTag("system", i);
		CloudsFakeVS cvs;
		cvs.name = settings.getValue("name", "no-name");
		cvs.keywords = ofSplitString(settings.getValue("tag", ""), ",", true, true) ;
		cvs.filePath = settings.getValue("filename", "");
		if(cvs.filePath != ""){
			
			cout << "	added system " << cvs.name << endl;

 			string fileType = ofToLower( ofFilePath::getFileExt(cvs.filePath));
			if(fileType == "png" || fileType == "jpg"){
				cvs.image.loadImage("../../../CloudsData/visualsystems/"+cvs.filePath);
			}
			else if(fileType == "mov" || fileType == "mp4"){
				cvs.player.loadMovie("../../../CloudsData/visualsystems/"+cvs.filePath);
			}
			else{
				cout << "	no asset for system " << cvs.name << endl;
			}
		}
		
		fakeVisualSystems.push_back(cvs);
		for(int i = 0; i < cvs.keywords.size(); i++){
			keywordToSystems[ cvs.keywords[i] ].push_back( cvs );
		}
		allKeywords.insert(cvs.keywords.begin(),cvs.keywords.end());
		settings.popTag();
	}
	
	//copy keyword set into relevant keywords
	copy(allKeywords.begin(), allKeywords.end(), inserter(relevantKeywords, relevantKeywords.begin()));
	
	cout << "found " << allKeywords.size() << " keywords for " << fakeVisualSystems.size() << " visual systems" << endl;
	
	cout << "relevantKeywords keywords: " << endl;
	for(int i = 0; i < relevantKeywords.size(); i++) cout << relevantKeywords[i] << " rel" << endl;
	
}

void CloudsVisualSystemStandin::begin(){
	vector<CloudsFakeVS>& systems = keywordToSystems[getCurrentKeyword()];
	if(systems.size() == 0){
		ofLogError() << "selected invalid keyword " << getCurrentKeyword();
		return;
	}
	
	systemSelected = true;
	currentVisualSystem = &systems[int(ofRandom( systems.size() ))];
	if(currentVisualSystem->player.isLoaded()){
		currentVisualSystem->player.setLoopState(OF_LOOP_NORMAL);
		currentVisualSystem->player.play();
	}
}

void CloudsVisualSystemStandin::end(){
	if(currentVisualSystem != NULL){
		currentVisualSystem->player.stop();
	}
	currentVisualSystem = NULL;
	systemSelected = false;
}

//these events are registered only when running the simulation
void CloudsVisualSystemStandin::update(ofEventArgs & args){

}

void CloudsVisualSystemStandin::draw(ofEventArgs & args){
	if(currentVisualSystem != NULL){
		currentVisualSystem->draw();
	}
}

bool CloudsVisualSystemStandin::isReleventToKeyword(string keyword){
	return allKeywords.find(keyword) != allKeywords.end();
}

//INTERACTION EVENTS -- registered only
void CloudsVisualSystemStandin::keyPressed(ofKeyEventArgs & args){}
void CloudsVisualSystemStandin::keyReleased(ofKeyEventArgs & args){}

void CloudsVisualSystemStandin::mouseDragged(ofMouseEventArgs & args){}
void CloudsVisualSystemStandin::mouseMoved(ofMouseEventArgs & args){}
void CloudsVisualSystemStandin::mousePressed(ofMouseEventArgs & args){}
void CloudsVisualSystemStandin::mouseReleased(ofMouseEventArgs & args){}

//application exit, clean up and don't crash
void CloudsVisualSystemStandin::exit(ofEventArgs & args){
	
}

