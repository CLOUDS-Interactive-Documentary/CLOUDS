#include "testApp.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	ofToggleFullscreen();
	
    parser.loadFromFiles();
	
	if(!ofFile::doesFileExist(getDataPath() + "CloudsMovieDirectory.txt")){
		ofSystemAlertDialog("Could not find movie file path. Create a file called CloudsMovieDirectory.txt that contains one line, the path to your movies folder");
	}

	parser.setCombinedVideoDirectory(ofBufferFromFile(getDataPath() + "CloudsMovieDirectory.txt").getText());
	
	visualSystems.populateVisualSystems();
	//visualSystems.loadPresets();
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	storyEngine.printDecisions = false;
	storyEngine.combinedClipsOnly = true;
	storyEngine.setup();
	
	player.setup();
	player.setStoryEngine(storyEngine);
	sound.setup(storyEngine);

	
	////////SEED WITH RANDOM CLIP
//	srand( ofGetSeconds()*1000 );
//	CloudsClip& clip = parser.getRandomClip(true,false);
//	
//	ofLogNotice() << clip.getLinkName() << " Started with question " << clip.getStartingQuestion() << endl;
//	map<string,string> questionsAndTopics = clip.getAllQuestionTopicPairs();
////	if(questionsAndTopics.size() > 0){
////		storyEngine.buildAct(clip, questionsAndTopics.begin()->first );
////	}
//	storyEngine.buildAct(clip, clip.getKeywords()[0] );
	////////SEED WITH RANDOM CLIP
	
	//////////////SHOW INTRO
	vector<CloudsClip> startingNodes = parser.getClipsWithKeyword("#start");
	//safe guard delete any starters that don't have questions
	for(int i = startingNodes.size()-1; i >= 0; i--){
		if(!startingNodes[i].hasQuestion() ) {
			ofLogError() << "Clip " << startingNodes[i].getID() << " is labeled as #start but has no question, removing.";
			startingNodes.erase(startingNodes.begin() + i);
		}
		else if(!startingNodes[i].hasCombinedVideo){
			ofLogError() << "Clip " << startingNodes[i].getID() << " has no combined video file, removing.";
			//startingNodes.erase(startingNodes.begin() + i);
		}
		else{
			cout << "Adding clip " << startingNodes[i].getID() << " with question " << startingNodes[i].getQuestionsVector()[0] << endl;
		}
	}
	cout << "Starting with " << startingNodes.size() << endl;
	player.showIntro(startingNodes);
	//////////////SHOW INTRO
}

//--------------------------------------------------------------
void testApp::update(){
	sound.update();
	ofShowCursor();
}

//--------------------------------------------------------------
void testApp::draw(){
	sound.drawDebug();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::audioRequested(float * output, int bufferSize, int nChannels) {
	
	ofAudioEventArgs args;
	args.buffer = output;
	args.bufferSize = bufferSize;
	args.nChannels = nChannels;
	
	ofNotifyEvent(ofEvents().audioRequested, args, this);
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