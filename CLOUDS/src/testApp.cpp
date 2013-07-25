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
	
	storyEngine.setup();
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	storyEngine.combinedClipsOnly = true;
	
	player.setup();
	player.setStoryEngine(storyEngine);
	sound.setup(storyEngine);
	
	srand( ofGetSeconds()*1000 );
	CloudsClip& clip = parser.getRandomClip(true,true);
	
	ofLogNotice() << clip.getLinkName() << " Started with question " << clip.getStartingQuestion() << endl;
	map<string,string> questionsAndTopics = clip.getAllQuestionTopicPairs();
	if(questionsAndTopics.size() > 0){
		storyEngine.buildAct(clip, questionsAndTopics.begin()->first );
	}
	
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