#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	parser.setup("../../../CloudsLibrary/data/fcpxml/");
    parser.parseLinks("../../../CloudsLibrary/data/links/clouds_link_db.xml");

	storyEngine.setup();
	storyEngine.network = &parser;
	storyEngine.maxTimesOnTopic = 2;
	
	player.setup(storyEngine);
	
	float randomClip = ofRandom(parser.getAllClips().size() );
	storyEngine.seedWithClip(parser.getAllClips()[ int(randomClip) ]);
	
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