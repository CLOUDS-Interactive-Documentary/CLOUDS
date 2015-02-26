#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
    ofEnableSmoothing();
	
	parser.loadFromFiles();
	parser.loadMediaAssets();

	currentClip = 0;
	stringstream questions;

	for(int i = 0; i < parser.getAllClips().size(); i++){
		CloudsClip* clip = parser.getAllClips()[i];
		if(clip->isLanguageCompatible() && clip->hasMediaAsset){
			subtitleClips.push_back(clip);
			cout << "added clip " << clip->getLinkName() << endl;
			if(clip->hasQuestion()){
				map<string,string>::iterator it;
				for( it = clip->getAllQuestionTopicPairs().begin(); it != clip->getAllQuestionTopicPairs().end(); it++){
					questions << clip->getLinkName() << "	" << it->first << "	" << it->second << endl;
				}
			}
		}
	}
	
	ofBufferToFile(GetCloudsDataPath() + "TRANSLATED_question_topics.txt", ofBuffer(questions.str()));

	cout << "Found " << subtitleClips.size() << endl;

	//double check there are no subtitles without clips
	ofDirectory dir(GetCloudsDataPath() + "language/" + GetLanguage() + "/subtitles/");
	dir.allowExt("srt");
	dir.listDir();

	cout << "Found " << dir.size() << " srt files" << endl;

	for(int i = 0; i < dir.size(); i++){
		string movieFilePath = GetCloudsDataPath(true) + "media/" + ofFilePath::removeExt(dir.getName(i)) + ".mov";
		if(!ofFile(movieFilePath).exists()){
			cout << "NO FILE " << movieFilePath << endl;
		}
	}


	playCurrentClip();
}


//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
	player.getPlayer().draw(0,0);
	player.drawSubtitles(ofGetWidth()/2.0, ofGetHeight()*0.7);
	
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == OF_KEY_LEFT){
		currentClip = (currentClip + 1) % subtitleClips.size();
		playCurrentClip();
	}
	else if(key == OF_KEY_RIGHT){
		currentClip--;
		if(currentClip < 0) currentClip += subtitleClips.size();
		playCurrentClip();
	}
}

//--------------------------------------------------------------
void testApp::playCurrentClip(){
	CloudsClip* clip = subtitleClips[currentClip];
	player.setup(clip->combinedVideoPath,
				 clip->combinedCalibrationXMLPath,
				 clip->getSubtitlesPath(),
				 1.0, clip->getSpeakerVolume());
	player.swapAndPlay();
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