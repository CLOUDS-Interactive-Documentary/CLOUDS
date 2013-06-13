#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	ofToggleFullscreen();
	
	
	parser.setup(CloudsVisualSystem::getDataPath() + "fcpxml/");
    parser.parseLinks(CloudsVisualSystem::getDataPath() + "links/clouds_link_db.xml");
	if(!ofFile::doesFileExist(CloudsVisualSystem::getDataPath() + "CloudsMovieDirectory.txt")){
		ofSystemAlertDialog("Could not find movie file path. Create a file called CloudsMovieDirectory.txt that contains one line, the path to your movies folder");
	}
	parser.setCombinedVideoDirectory(ofBufferFromFile(CloudsVisualSystem::getDataPath() + "CloudsMovieDirectory.txt").getText());
	
	storyEngine.setup();
	storyEngine.network = &parser;
	storyEngine.maxTimesOnTopic = 4;
	storyEngine.combinedClipsOnly = true;
	
	player.setup(storyEngine);
	
	float randomClip = ofRandom(parser.getAllClips().size() );

	storyEngine.seedWithClip( parser.getRandomClip(true) );
	//storyEngine.seedWithClip( parser.getClipWithLinkName("Paola - the tribe") );
	
}

//--------------------------------------------------------------
void testApp::update(){
	player.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	player.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == '1'){
		storyEngine.seedWithClip( parser.getClipWithLinkName("Paola - the tribe") );		
	}
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