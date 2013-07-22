#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	ofToggleFullscreen();
    displayGui = false;
	currentAct = NULL;
	
	parser.loadFromFiles();
	
	visualSystems.loadPresets();
	
	storyEngine.setup();
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	
	storyEngine.maxTimesOnTopic = 4;
	storyEngine.printDecisions = true;
	

	ofEnableSmoothing();
	
    float randomClip = ofRandom(parser.getAllClips().size() );
	CloudsClip& clip = parser.getRandomClip(false,false);
	rebuildAct = false;
    
	ofLogNotice() << clip.getLinkName() << " Started with question " << clip.getStartingQuestion() << endl;
	
	ofAddListener(storyEngine.getEvents().actCreated, this, &testApp::actCreated);
	storyEngine.buildAct( clip );
    
}

//--------------------------------------------------------------
void testApp::actCreated(CloudsActEventArgs& args){
	
	if(currentAct != NULL){
		currentAct->unregisterEvents(this);
		delete currentAct;
	}

	currentAct = args.act;
	currentAct->registerEvents(this);
	currentAct->play();
    currentAct->getTimeline().enableEvents();
}

//--------------------------------------------------------------
void testApp::actBegan(CloudsActEventArgs& args){
	cout << "act began" << endl;
}

//--------------------------------------------------------------
void testApp::actEnded(CloudsActEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::clipBegan(CloudsClipEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::topicChanged(string& newTopic){
	
}
void testApp:: questionAsked(CloudsQuestionEventArgs& args){
    
}

void testApp::preRollRequested(CloudsPreRollEventArgs& clip){
    
}
//--------------------------------------------------------------
void testApp::update(){
	ofShowCursor();
}

//--------------------------------------------------------------
void testApp::draw(){

    if(currentAct != NULL){
		currentAct->drawDebug();
	}
}

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
    
    if(key == ' '){
        displayGui = ! displayGui;
        storyEngine.displayGui(displayGui);
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