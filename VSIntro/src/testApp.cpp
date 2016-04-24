#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	#ifdef OCULUS_RIFT
    ofSetVerticalSync(false);
	ofSetBackgroundAuto(false);
	#endif

	//////////////SHOW INTRO
    parser.loadFromFiles();
	vector<CloudsClip*> startingNodes = parser.getClipsWithKeyword("#start");
	//safe guard delete any starters that don't have questions
	for(int i = startingNodes.size()-1; i >= 0; i--){
		if(!startingNodes[i]->hasQuestion() ) {
			ofLogError() << "Clip " << startingNodes[i]->getID() << " is labeled as #start but has no question, removing.";
			startingNodes.erase(startingNodes.begin() + i);
		}
		else if(!startingNodes[i]->hasMediaAsset){
			ofLogError() << "Clip " << startingNodes[i]->getID() << " has no media asset, removing.";
			startingNodes.erase(startingNodes.begin() + i);
		}
		else{
			cout << " Adding Clip " << startingNodes[i]->getID() << " with question " << startingNodes[i]->getQuestions()[0] << endl;
		}
	}
	
	cout << "Starting with " << startingNodes.size() << endl;
	
#ifdef OCULUS_RIFT
    
#endif
	intro.setStartQuestions(startingNodes);
	ofAddListener(intro.events.portalHoverBegan, this, &testApp::portalHoverBegan);
	ofAddListener(intro.events.portalHoverEnded, this, &testApp::portalHoverEnded);

	intro.setup();
#ifdef OCULUS_RIFT
	intro.loadPresetGUISFromName("Oculus_Pretty");
#else
	intro.loadPresetGUISFromName("TunnelWarp");
#endif

	//intro.setDrawToScreen(false);
	intro.playSystem();
	//////////////SHOW INTRO
    
//    ofHideCursor();
//    ofToggleFullscreen();
}

//--------------------------------------------------------------------
void testApp::portalHoverBegan(CloudsPortalEventArgs &args){
//    hud.questionHoverOn(args.question);
}

//--------------------------------------------------------------------
void testApp::portalHoverEnded(CloudsPortalEventArgs &args){
//	hud.questionHoverOff();
}


//--------------------------------------------------------------
void testApp::update(){
    if(ofGetFrameNum() == 100){
        intro.percentLoaded = 1.;
        intro.loadingFinished();
    }
    else{
        intro.percentLoaded = ofGetFrameNum() / 100.;
    }
}


//--------------------------------------------------------------
void testApp::draw(){
    float hudDistance = CloudsVisualSystem::subtitleHudZ;
    float hudScale = CloudsVisualSystem::subtitleHudScale;
    CloudsVisualSystem::getOculusRift().beginOverlay(hudDistance, hudScale, 1920, 1080);

	intro.selfDrawOverlay();

    CloudsVisualSystem::getOculusRift().endOverlay();

//	hud.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'H'){
        ofHideCursor();
    }
    else if(key == 'S'){
        ofShowCursor();
    }
}

//--------------------------------------------------------------
void testApp::exit(){
	intro.exit();
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
#ifdef OCULUS_RIFT
    // EZ: Override CloudsInputSystem just to get the thing started
    // since we can't click with Oculus input.
    ofMouseEventArgs args;
    args.x = x;
    args.y = y;
    args.button = button;
    intro.selfMousePressed(args);
#endif
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