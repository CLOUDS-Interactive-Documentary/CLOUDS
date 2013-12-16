#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	ofToggleFullscreen();
    ofEnableSmoothing();
	ofBackground(0,0,255);
//	currentAct = NULL;
//	rebuildAct = false;
//	
//	parser.loadFromFiles();
//	
//	visualSystems.loadPresets();
//	
//	storyEngine.parser = &parser;
//	storyEngine.visualSystems = &visualSystems;
//	
//	storyEngine.setup();
//	storyEngine.printDecisions = false;
//	storyEngine.toggleGuis();
//	
//	parser.printDichotomyRatios();
//	
//	websockets.setup();
	
//	ofAddListener(storyEngine.getEvents().actCreated, this, &testApp::actCreated);
	
	hud.setup();
}

//--------------------------------------------------------------
//void testApp::actCreated(CloudsActEventArgs& args){
//	
//	if(currentAct != NULL){
//		currentAct->unregisterEvents(this);
//		currentAct->unregisterEvents(&websockets);
//		delete currentAct;
//	}
//
//	
//	currentAct = args.act;
//	currentAct->registerEvents(this);
//	currentAct->registerEvents(&websockets);
//	
//	currentAct->play();
//    currentAct->getTimeline().enableEvents();
//}

////--------------------------------------------------------------
//void testApp::actBegan(CloudsActEventArgs& args){
////	cout << "act began" << endl;
//}
//
////--------------------------------------------------------------
//void testApp::actEnded(CloudsActEventArgs& args){
//	
//}
//
////--------------------------------------------------------------
//void testApp::clipBegan(CloudsClipEventArgs& args){
//	
//}
//
////--------------------------------------------------------------
//void testApp::visualSystemBegan(CloudsVisualSystemEventArgs& args){
//	
//}
//
////--------------------------------------------------------------
//void testApp::visualSystemEnded(CloudsVisualSystemEventArgs& args){
//	
//}
//
////--------------------------------------------------------------
//void testApp::topicChanged(CloudsTopicEventArgs& newTopic){
//	
//}
//
//void testApp::questionAsked(CloudsQuestionEventArgs& args){
//    
//}
//
//void testApp::preRollRequested(CloudsPreRollEventArgs& clip){
//    
//}

//--------------------------------------------------------------
void testApp::update(){
	hud.update();
//	ofShowCursor();
	//keepin it real
//	storyEngine.maxTimesOnTopic = floor(storyEngine.maxTimesOnTopic);
}

//--------------------------------------------------------------
void testApp::draw(){
//	ofBackground(mouseX);

	hud.draw();
//    if(currentAct != NULL){
//		currentAct->drawDebug();
//	}
}

//--------------------------------------------------------------
void testApp::exit(){
//    storyEngine.saveGuiSettings();
	hud.saveGuiSettings();
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
//    string name = e.widget->getName();
//    ofxUIButton* b = (ofxUIButton*) e.widget;
//    if(name == "BUILD ACT" &&  b->getValue() ){
//        rebuildAct = true;
//    }

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
	if(key == ' '){
		hud.animateOn(CLOUDS_HUD_QUESTION);
	}
	
    if(key == 'h'){
		hud.toggleGuis();
//        storyEngine.toggleGuis();
    }
	
    else if(key =='f'){
        ofToggleFullscreen();
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