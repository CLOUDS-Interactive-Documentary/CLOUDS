//
//  CloudsSecondaryDisplayController.cpp
//  CloudsSecondaryDisplay
//
//  Created by James George on 12/23/13.
//
//

#include "CloudsSecondaryDisplayController.h"
#include "CloudsGlobal.h"

CloudsSecondaryDisplayController::CloudsSecondaryDisplayController(){
	hasSpeaker = false;
	playingMovie = false;
}

void CloudsSecondaryDisplayController::setup(){

	parser.loadFromFiles();
	visualSystems.loadPresets();
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	storyEngine.printDecisions = false;
	storyEngine.combinedClipsOnly = false;
	storyEngine.setup();
	
//	vector<CloudsClip> startingNodes = parser.getClipsWithKeyword("#start");
//	CloudsAct* act = storyEngine.buildAct(run, startingNodes[ ofRandom(startingNodes.size()) ]);
//	
//	run.topicHistory = act->getAllTopics();
//	run.clipHistory = act->getAllClips();
	
	clusterMap.buildEntireCluster(parser);
	clusterMap.setRun(run);

//	clusterMap.traverse();
	clusterMap.forceScreenResolution(1920, 1080);
	clusterMap.setDrawToScreen(false);
	
	clusterMap.setup();
	clusterMap.playSystem();

	receiver.setup(12346);
	
	exampleType.loadFont(GetCloudsDataPath() + "font/Blender-THIN.ttf");
    
	loadSVGs();
 
	displayTarget.allocate(1920, 1080, GL_RGB);
}

/*LOADING SVG LAYOUT files from Sarah*/
void CloudsSecondaryDisplayController::loadSVGs(){
	ofDirectory svgs(GetCloudsDataPath() + "secondaryDisplay/SVG/PROJECTEX1/");
	svgs.allowExt("svg");
	svgs.listDir();
	for(int i = 0; i < svgs.numFiles(); i++){
		testAllLayout.push_back(CloudsSVGMesh());
		testAllLayout.back().load(svgs.getPath(i));
	}
}

void CloudsSecondaryDisplayController::update(){
	
	//TODO: fix with perma preset
	clusterMap.incrementalTraversalMode = true;
	
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
			
			clusterMap.traverseToClip(currentClip);
			
			string exampleId = m.getArgAsString(4);
			if(exampleId != ""){
				//need to do something smarter here
				currentExample = parser.getProjectExampleWithTitle(exampleId);
				if(currentExample.exampleVideos.size() > 0){
					playingMovie = archivePlayer.loadMovie( currentExample.exampleVideos[0]);
					if(playingMovie){
						archivePlayer.setLoopState(OF_LOOP_NONE);
						archivePlayer.play();
					}
				}
			}
			else{
				playingMovie = false;
				archivePlayer.stop();
			}
		}
	}
	
	
	if(playingMovie){
		archivePlayer.update();
	}
}

/* WORK MOSTLY IN HERE - JK */
void CloudsSecondaryDisplayController::draw(){
	
	displayTarget.begin();
	
	clusterMap.selfPostDraw();

	//DEBUG
	if(hasSpeaker){
		exampleType.drawString(currentSpeaker.firstName + " " + currentSpeaker.lastName, 30, 30);
	}
	else{
		exampleType.drawString("NO SPEAKER", 30, 30);
	}
    //END DEBUG
	
	if(playingMovie){
		archivePlayer.draw(ofGetWidth()/2 - archivePlayer.getWidth()/2,
						   ofGetHeight()/2 - archivePlayer.getHeight()/2);
		playingMovie = archivePlayer.isPlaying();
	}
    
	for(int i = 0; i < testAllLayout.size(); i++){
		testAllLayout[i].draw();
	}
	
	displayTarget.end();
	
	ofRectangle screenRect(0,0,ofGetWidth(), ofGetHeight());
	ofRectangle targetRect(0,0,displayTarget.getWidth(),displayTarget.getHeight());
	targetRect.scaleTo(screenRect);
	displayTarget.getTextureReference().draw(targetRect);
	
    // ---------------- added
    
//	//get info for a speaker
//	CloudsSpeaker::speakers["Kyl_CH"].twitterHandle;

	//TODO: draw speaker layout
	
	//TODO: overlay with project example when relevant
	
}
