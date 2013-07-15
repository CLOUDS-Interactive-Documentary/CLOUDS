#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	ofToggleFullscreen();
	
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
//--------------------------------------------------------------
void testApp::update(){
    
    if(rebuildAct){
        rebuildAct = false;
        CloudsClip& clip = parser.getRandomClip(false,false);
		storyEngine.buildAct( clip );
    }

//	sound.update();
	ofShowCursor();
}

//--------------------------------------------------------------
void testApp::draw(){

//	cout << "APP DRAW" << endl;
	
//	sound.drawDebug();
	
	//storyEngine.drawStoryEngineDebug();
    if(currentAct != NULL){
		currentAct->drawDebug();
	}
	
//    storyEngine.getAct().drawActDebug();
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