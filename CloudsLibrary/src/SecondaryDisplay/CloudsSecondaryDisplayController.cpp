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
	
//	//get info for a speaker
//	CloudsSpeaker::speakers["Kyl_CH"].twitterHandle;

	//TODO: draw speaker layout
	
	//TODO: overlay with project example when relevant
	
}
