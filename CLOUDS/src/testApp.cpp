#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	ofToggleFullscreen();
	
	parser.setup(CloudsVisualSystem::getDataPath() + "fcpxml/");
    parser.parseLinks(CloudsVisualSystem::getDataPath() + "links/clouds_link_db.xml");
    parser.parseClusterMap(CloudsVisualSystem::getDataPath() + "gephi/CLOUDS_test_5_26_13.SVG");
    
	if(!ofFile::doesFileExist(CloudsVisualSystem::getDataPath() + "CloudsMovieDirectory.txt")){
		ofSystemAlertDialog("Could not find movie file path. Create a file called CloudsMovieDirectory.txt that contains one line, the path to your movies folder");
	}

	parser.setCombinedVideoDirectory(ofBufferFromFile(CloudsVisualSystem::getDataPath() + "CloudsMovieDirectory.txt").getText());
	
	visualSystems.populateVisualSystems();
	
	storyEngine.setup();
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	
	storyEngine.maxTimesOnTopic = 4;
	storyEngine.combinedClipsOnly = true;
	storyEngine.printDecisions = false;
	
	player.setup(storyEngine);
	sound.setup(storyEngine);
	
	float randomClip = ofRandom(parser.getAllClips().size() );
	CloudsClip& clip = parser.getRandomClip(true,false);
	
	ofLogNotice() << clip.getLinkName() << " Started with question " << clip.getStartingQuestion() << endl;
	
	storyEngine.seedWithClip( clip );
}

//--------------------------------------------------------------
void testApp::update(){
//	player.update();
	sound.update();
	ofShowCursor();
}

//--------------------------------------------------------------
void testApp::draw(){

//	cout << "APP DRAW" << endl;
	
	sound.drawDebug();
	
	//storyEngine.drawStoryEngineDebug();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == '1'){
		storyEngine.seedWithClip( parser.getClipWithLinkName("Paola - the tribe") );		
	}
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