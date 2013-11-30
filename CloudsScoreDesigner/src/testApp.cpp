#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	ofToggleFullscreen();
    ofEnableSmoothing();
	
	currentAct = NULL;
	rebuildAct = false;
	
	parser.loadFromFiles();
	parser.setCombinedVideoDirectory(ofBufferFromFile(getDataPath() + "CloudsMovieDirectory.txt").getText());
	
	visualSystems.loadPresets();
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	
    storyEngine.combinedClipsOnly = false; // true if using a clips drive
	storyEngine.setup();
	storyEngine.printDecisions = false;
	storyEngine.toggleGuis(true);
    withVideo = false; // draw video?
    
    sound.setup(storyEngine);
    
	parser.printDichotomyRatios();
	
	//websockets.setup();
	
	ofAddListener(storyEngine.getEvents().actCreated, this, &testApp::actCreated);
}

//--------------------------------------------------------------
void testApp::actCreated(CloudsActEventArgs& args){
	
	if(currentAct != NULL){
		currentAct->getTimeline().stop();
		currentAct->unregisterEvents(this);
//		currentAct->unregisterEvents(&websockets);
		delete currentAct;
	}

	
	currentAct = args.act;
	currentAct->registerEvents(this);
//	currentAct->registerEvents(&websockets);
	
	currentAct->play();
    currentAct->getTimeline().enableEvents();
}

//--------------------------------------------------------------
void testApp::actBegan(CloudsActEventArgs& args){
//	cout << "act began" << endl;
}

//--------------------------------------------------------------
void testApp::actEnded(CloudsActEventArgs& args){
	cout << "ACT ENDED" << endl;
}

//--------------------------------------------------------------
void testApp::clipBegan(CloudsClipEventArgs& args){
	if(withVideo) player.swapAndPlay();
}

//--------------------------------------------------------------
void testApp::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::topicChanged(CloudsTopicEventArgs& newTopic){
	
}

void testApp::questionAsked(CloudsQuestionEventArgs& args){
    
}

void testApp::preRollRequested(CloudsPreRollEventArgs& clip){
	
	if(withVideo) player.setup(clip.preRollClip.combinedVideoPath,
				 clip.preRollClip.combinedCalibrationXMLPath);
	
}
//--------------------------------------------------------------
void testApp::update(){
//	ofShowCursor();
	//keepin it real
	storyEngine.maxTimesOnTopic = floor(storyEngine.maxTimesOnTopic);
    player.maxVolume = sound.maxSpeakerVolume;
    sound.update();

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
void testApp::draw(){
    if(currentAct != NULL){
		currentAct->drawDebug();
	}
    if(withVideo) {
        if(player.isPlaying()){
            player.getPlayer().draw(0,0,
                                    player.getPlayer().getWidth()*.25,
                                    player.getPlayer().getHeight()*.25);
        }
    }
}

//--------------------------------------------------------------
void testApp::exit(){
    storyEngine.saveGuiSettings();
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    ofxUIButton* b = (ofxUIButton*) e.widget;
    if(name == "BUILD ACT" &&  b->getValue() ){
        rebuildAct = true;
    }

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
	if(key =='f'){
        ofToggleFullscreen();
    }
	if(key == 't'){
		storyEngine.positionGuis();
	}
	if(key == 'S'){
		storyEngine.saveGuiSettings();
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