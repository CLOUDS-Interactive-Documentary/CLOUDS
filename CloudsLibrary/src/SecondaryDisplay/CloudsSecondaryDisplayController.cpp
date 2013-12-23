//
//  CloudsSecondaryDisplayController.cpp
//  CloudsSecondaryDisplay
//
//  Created by James George on 12/23/13.
//
//

#include "CloudsSecondaryDisplayController.h"

CloudsSecondaryDisplayController::CloudsSecondaryDisplayController(){
	
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
	
	clusterMap.setup();
	clusterMap.playSystem();
	
	receiver.setup(1236);
}

void CloudsSecondaryDisplayController::update(){
	while(receiver.hasWaitingMessages()){
		ofxOscMessage m;
		receiver.getNextMessage(&m);
		
//		...
	
	}
}

void CloudsSecondaryDisplayController::drawOverlay(){
	
}
