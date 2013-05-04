//
//  CloudsVisualizationStandin.cpp
//  CLOUDS
//
//  Created by James George on 4/23/13.
//
//

#include "CloudsVisualSystemStandin.h"
#include "ofxXmlSettings.h"

CloudsVisualSystemStandin::CloudsVisualSystemStandin(){
	systemSelected = false;
	currentVisualSystem = NULL;
}

string CloudsVisualSystemStandin::getSystemName(){
	return currentVisualSystem == NULL ? "no-system" : currentVisualSystem->name;
}

void CloudsVisualSystemStandin::setup(){
	
	//pasre the XML file to get videos
	ofxXmlSettings settings;
	settings.loadFile("../../CloudsData/visualsystems.xml");
	int numSystems = settings.getNumTags("visualsystem");
	for(int i = 0; i < numSystems; i++){
		settings.pushTag("visualsystem", i);
		CloudsFakeVS cvs;
		cvs.name = settings.getValue("name", "no-name");
		cvs.keywords = ofSplitString(settings.getValue("keywords", ""), ",", true, true) ;
		cvs.filePath = settings.getValue("filePath", "");
		if(cvs.filePath != ""){
 			string fileType = ofToLower( ofFilePath::getFileExt(cvs.filePath));
			if(fileType == "png" || fileType == "jpg"){
				cvs.image.loadImage(cvs.filePath);
			}
			else if(fileType == "mov" || fileType == "mp4"){
				cvs.player.loadMovie(cvs.filePath);
			}
		}
		
		fakeVisualSystems.push_back(cvs);
		allKeywords.insert(cvs.keywords.begin(),cvs.keywords.end());
		
		settings.popTag();
	}
	
	cout << "found " << allKeywords.size() << " keywords for " << fakeVisualSystems.size() << " visual systems" << endl;
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

