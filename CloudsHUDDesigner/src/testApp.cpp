#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground(0);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
    
    //sets the HUD
    
	parser.loadFromFiles();
    
    std::set<string> topics(parser.getAllKeywords().begin(),
                            parser.getAllKeywords().end() );

	hud.setup();
    hud.setTopics(topics);
    
    empty.setup();
    empty.playSystem();
    empty.setDrawToScreen(false);

}

//--------------------------------------------------------------
void testApp::update(){
	hud.update();
    
	if(hud.isResetHit()){
		cout << "RESET!!!" << endl;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(128);
    
	ofDrawBitmapString(ofToString(ofGetFrameRate()), 20,20);
	hud.draw();

	ofEnableAlphaBlending();
	ofSetColor(255,0,0,100);
	ofSetColor(255);
}

//--------------------------------------------------------------
void testApp::exit(){
//	hud.saveGuiSettings();
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e){
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
	if(key == 'C'){
		hud.respondToClip( parser.getRandomClip() );
	}
	
	if(key == 'E'){
		vector<int> projectExampleIndecs;
		for(int i = 0; i < parser.getAllClips().size(); i++){
			if(parser.getAllClips()[i]->hasProjectExample){
				projectExampleIndecs.push_back(i);
			}
		}
		
		if(projectExampleIndecs.size() > 0){
			int exampleIndex = projectExampleIndecs[ ofRandom(projectExampleIndecs.size()) ];
			hud.respondToClip( parser.getAllClips()[exampleIndex] );
			cout << "SENT CLIP " << parser.getAllClips()[exampleIndex]->getLinkName() << " WITH EXAMPLE " << parser.getAllClips()[exampleIndex]->projectExampleTitle << endl;
		}
	}
	
	if(key == ' '){
		hud.animateOn(CLOUDS_HUD_ALL);
	}
    
    if(key == '1'){
		hud.animateOn(CLOUDS_HUD_HOME);
	}
    
    if(key == '2'){
		hud.animateOn(CLOUDS_HUD_LOWER_THIRD);
	}
    
    if(key == '3'){
		hud.animateOn(CLOUDS_HUD_QUESTION);
	}
    
    if(key == '4'){
		hud.animateOn(CLOUDS_HUD_PROJECT_EXAMPLE);
	}
    
    if(key == '5'){
		hud.animateOn(CLOUDS_HUD_PAUSE);
        //hud.pause();
	}
    
    if(key == '6'){
		hud.animateOn(CLOUDS_HUD_NEXT);
        //hud.pause();
	}
    
    if(key == '7'){

//		hud.animateOn(CLOUDS_HUD_RESEARCH_LIST);
		hud.animateOn(CLOUDS_RESEARCH);
		hud.animateOn(CLOUDS_RESEARCH_RESUME);
        //hud.pause();
	}
    
    if(key == '8'){
//        hud.animateOn(CLOUDS_HUD_ABOUT);
    }

    if(key == '9'){
        hud.animateOff();
    }
    
    if(key == 'h'){
//		hud.toggleGuis();
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