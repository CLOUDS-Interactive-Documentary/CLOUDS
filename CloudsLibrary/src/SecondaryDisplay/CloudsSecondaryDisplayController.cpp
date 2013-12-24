//
//  CloudsSecondaryDisplayController.cpp
//  CloudsSecondaryDisplay
//
//  Created by James George on 12/23/13.
//
//

#include "CloudsSecondaryDisplayController.h"


CloudsSecondaryDisplayController::CloudsSecondaryDisplayController(){
	hasSpeaker = false;
}

void CloudsSecondaryDisplayController::setup(){

	parser.loadFromFiles();
	visualSystems.loadPresets();
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	storyEngine.printDecisions = false;
	storyEngine.combinedClipsOnly = false;
	storyEngine.setup();
	
	vector<CloudsClip> startingNodes = parser.getClipsWithKeyword("#start");
	CloudsAct* act = storyEngine.buildAct(run, startingNodes[ ofRandom(startingNodes.size()) ]);
	
	run.topicHistory = act->getAllTopics();
	run.clipHistory = act->getAllClips();
	
	clusterMap.buildEntireCluster(parser);
	clusterMap.setRun(run);
	clusterMap.traverse();
	
	clusterMap.setDrawToScreen(false);
	
	clusterMap.setup();
	clusterMap.playSystem();

	receiver.setup(12346);
	
	exampleType.loadFont(GetCloudsDataPath() + "font/Blender-THIN.ttf");
    
    buildSDLayerSets();
    
}

void CloudsSecondaryDisplayController::update(){
	while(receiver.hasWaitingMessages()){
		ofxOscMessage m;
		receiver.getNextMessage(&m);
		
		//all the args sent for clip for reference
//0		m.addStringArg(clip.person);//final cut person id
//1		m.addStringArg(clip.getID());//clip id
//2		m.addFloatArg(clip.getDuration());//duraiton
//3		m.addStringArg(currentTopic); //topic
//4		m.addStringArg(clip.projectExampleTitle); //example
//5		m.addStringArg(lastQuestionAsked); //question

		if(m.getAddress() == "/clip"){
			hasSpeaker = true;
			currentSpeaker = CloudsSpeaker::speakers[m.getArgAsString(0)];
			currentClip = parser.getClipWithID(m.getArgAsString(1));
			string exampleId = m.getArgAsString(4);
			if(exampleId != ""){
				//need to do something smarter here
				currentExample = parser.getProjectExampleWithTitle(exampleId);
				if(currentExample.exampleVideos.size() > 0){
					
//					archivePlayer.loadMovie( currentExample.exampleVideos[0]);
				}
			}
		}
	}
}

void CloudsSecondaryDisplayController::drawOverlay(){
	
	clusterMap.selfPostDraw();
	
	//... everything else will happen here
	if(hasSpeaker){
		exampleType.drawString(currentSpeaker.firstName + " " + currentSpeaker.lastName, 30, 30);
	}
	else{
		exampleType.drawString("NO SPEAKER", 30, 30);
	}
    
    
    // ---------------- added
    
//    CloudsSDLayerSet currentSDLayer;
//	CloudsSDLayer* sdLayer;
//	
//	//QUESTION LAYER
//	currentSDLayer = CLOUDS_SD_QUESTION;
//    
//    ofDirectory SVGDir(GetCloudsDataPath() + "secondaryDisplay/SVG/PROJECTEX1");
//	SVGDir.allowExt("svg");
//	SVGDir.listDir();
//	for(int i = 0; i < SVGDir.numFiles(); i++){
//		cout << "Loading " << SVGDir.getName(i) << endl;
//		sdLayer = new CloudsSDLayer();
//		sdLayer->parse(SVGDir.getPath(i));
////		sdLayerSets[currentSDLayer].push_back( sdLayer );
////		allSDLayers.push_back(sdLayer);
//		
//		sdLayer->duration = 1.5;
//		sdLayer->delayTime = ofRandomuf();
//		
//		sdLayer->startPoint = ofVec2f(sdLayer->svg.getWidth(),0);
//		sdLayer->endPoint   = ofVec2f(0,sdLayer->svg.getHeight());
//		
//	}
    
	
//	//get info for a speaker
//	CloudsSpeaker::speakers["Kyl_CH"].twitterHandle;

	//TODO: draw speaker layout
	
	//TODO: overlay with project example when relevant
	
}





//---------- from CloudsSDController.cpp

//void CloudsSecondaryDisplayController::setup(){
//	buildLayerSets();
//}

void CloudsSecondaryDisplayController::buildSDLayerSets(){
	
    // configure layers
    CloudsSDLayerSet currentSDLayer;
	CloudsSDLayer* sdLayer;
	
	//QUESTION LAYER
	currentSDLayer = CLOUDS_SD_QUESTION;
    
    ofDirectory SVGDir(GetCloudsDataPath() + "secondaryDisplay/SVG/PROJECTEX1");
	SVGDir.allowExt("svg");
	SVGDir.listDir();
	for(int i = 0; i < SVGDir.numFiles(); i++){
		cout << "Loading " << SVGDir.getName(i) << endl;
		sdLayer = new CloudsSDLayer();
		sdLayer->parse(SVGDir.getPath(i));
        sdLayerSets[currentSDLayer].push_back( sdLayer );
        allSDLayers.push_back(sdLayer);
		
		sdLayer->duration = 1.5;
		sdLayer->delayTime = ofRandomuf();
		
		sdLayer->startPoint = ofVec2f(sdLayer->svg.getWidth(),0);
		sdLayer->endPoint   = ofVec2f(0,sdLayer->svg.getHeight());
        
        ofLogNotice() << ">>>>>>>>>>>>>>>>>>>>>> it's happening >>>>>>>>>>>>>>>>>>>>>>>>";
		
	}
}

    void CloudsSecondaryDisplayController::updateSDLayers(){
	for(int i = 0; i < allSDLayers.size(); i++){
		
		allSDLayers[i]->update();
	}
	
    //	home.update();
}

void CloudsSecondaryDisplayController::draw(){
	
	ofPushStyle();
	ofPushMatrix();
	ofEnableAlphaBlending();
	
    //	ofSetColor(255,255,255,ofGetMouseX());
	drawSDLayer(CLOUDS_SD_QUESTION);
	drawSDLayer(CLOUDS_SD_LOWER_THIRD);
	drawSDLayer(CLOUDS_SD_PROJECT_EXAMPLE);
	
    //	home.draw();
	
	ofPopMatrix();
	ofPopStyle();
}

void CloudsSecondaryDisplayController::drawSDLayer(CloudsSDLayerSet layer){
	for(int i = 0; i < sdLayerSets[layer].size(); i++){
		sdLayerSets[layer][i]->draw();
	}
}

void CloudsSecondaryDisplayController::animateOn(CloudsSDLayerSet layer){
    
	for(int i = 0; i < sdLayerSets[layer].size(); i++){
		sdLayerSets[layer][i]->start();
	}
}
