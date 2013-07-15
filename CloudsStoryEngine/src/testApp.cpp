#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	ofToggleFullscreen();

	parser.loadFromFiles();
	
	visualSystems.loadPresets();
	
	storyEngine.setup();
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	
	storyEngine.maxTimesOnTopic = 4;
	storyEngine.printDecisions = true;
	
//	player.simplePlaybackMode = true;
//	player.setup(storyEngine);
//	sound.setup(storyEngine);

	ofEnableSmoothing();
	
    float randomClip = ofRandom(parser.getAllClips().size() );
	CloudsClip& clip = parser.getRandomClip(false,false);
	rebuildAct = false;
    
    
    gui = new ofxUISuperCanvas("STORY ENGINE PARAMS", OFX_UI_FONT_MEDIUM);
    gui->addSpacer();
    gui->addLabel("VS :");
    gui->addSlider("MAX VS RUNTIME", 0, 480,&storyEngine.systemMaxRunTime);
    gui->addSlider("MAX VS GAPTIME", 0, 60, &storyEngine.maxVisualSystemGapTime);
    gui->addSlider("LONG CLIP THRESHOLD", 0, 240, &storyEngine.longClipThreshold);
    gui->addSlider("LONG CLIP FAD IN %", 0.0, 1.0, &storyEngine.longClipFadeInPercent);
    gui->addSpacer();
    
    gui->addLabel("CLIP: ");
    gui->addSlider("ACT LENGTH", 60, 1200, &storyEngine.actLength);
    gui->addButton("BUILD ACT", false);
    gui->autoSizeToFitWidgets();


    clipGui = new ofxUISuperCanvas("CLIP STORY SCORE PARAMETERS", OFX_UI_FONT_MEDIUM);
    clipGui->setPosition(gui->getRect()->width, 0);
	clipGui->addSpacer();
    clipGui->addSlider("CURRENT TOPICS IN COMMON MULTIPLIER", 0, 50, storyEngine.topicsInCommonMultiplier);
    clipGui->addSlider("TOPICS IN COMMON WITH HISTORY MULTIPLIER", 0, 10, storyEngine.topicsinCommonWithPreviousMultiplier);
    clipGui->addSlider("SAME PERSON SUPPRESSION FACTOR", 0, 10, storyEngine.samePersonOccuranceSuppressionFactor);
    clipGui->addSlider("LINK FACTOR",0,50,storyEngine.linkFactor);
    clipGui->addSlider("DICHOTOMIES FACTOR", 0,10,storyEngine.dichomoiesFactor);
    clipGui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);
    ofAddListener(clipGui->newGUIEvent, this, &testApp::guiEvent);
    
    
	ofLogNotice() << clip.getLinkName() << " Started with question " << clip.getStartingQuestion() << endl;
	
	storyEngine.seedWithClip( clip );


}

//--------------------------------------------------------------
void testApp::update(){
    
    if(rebuildAct){
        rebuildAct =false;
        CloudsClip& clip = parser.getRandomClip(false,false);
        storyEngine.seedWithClip( clip );
        //storyEngine.getAct().clearAct();
    }

//	sound.update();
	ofShowCursor();
}

//--------------------------------------------------------------
void testApp::draw(){

//	cout << "APP DRAW" << endl;
	
//	sound.drawDebug();
	
	//storyEngine.drawStoryEngineDebug();
    
    storyEngine.getAct().drawActDebug();
}

void testApp::exit(){
    delete gui;
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