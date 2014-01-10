#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	parser.loadFromFiles();
	visualSystems.loadPresets();
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	storyEngine.printDecisions = false;
	storyEngine.combinedClipsOnly = false;
	storyEngine.setup();
	
	srand(ofGetSeconds());
	vector<CloudsClip> startingNodes = parser.getClipsWithKeyword("#start");
	CloudsAct* act = storyEngine.buildAct(run, startingNodes[ ofRandom(startingNodes.size()) ]);
	
	//act->triggerAllEvents();	
	run.topicHistory = act->getAllTopics();
	run.clipHistory = act->getAllClips();
	cout << "** TRAVERSAL INCLUDES:" << endl;
	for(int i = 0; i < run.clipHistory.size(); i++){
		cout << "	** " << run.clipHistory[i].getID() << endl;
	}
	
//	clusterMap.traverse();
	
	clusterMap.setup();
	clusterMap.setRun(run);
	clusterMap.buildEntireCluster(parser);
	clusterMap.playSystem();
	
//	for(int i = 0; i < parser.getAllClips().size(); i++){
//		cout << parser.getAllClips()[i].getID() << endl;
//	}
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

}
//--------------------------------------------------------------
void testApp::exit(){
	clusterMap.exit();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}