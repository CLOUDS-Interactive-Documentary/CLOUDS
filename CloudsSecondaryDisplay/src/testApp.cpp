#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	secondaryDisplay.setup();
    testAllClips = false;
    timer = true;
    debug = false;
    currentTestClip = 0;
    startTime = 0;
}

//--------------------------------------------------------------
void testApp::update(){
	secondaryDisplay.update();
    
    if(testAllClips && timer){
        //wait for 3 seconds
        secondaryDisplay.respondToClip( secondaryDisplay.parser.getAllClips()[currentTestClip] );
        //goto next clip
        timer = false;
        startTime = ofGetElapsedTimef();
        cout<<"Secondary Display::currentTestClip = "<<currentTestClip<<endl;
    }
    
    if(testAllClips && ofGetElapsedTimef() - startTime >= 3.5f){
        timer = true;
        if(currentTestClip == secondaryDisplay.parser.getAllClips().size()-1)currentTestClip=0;
        else currentTestClip++;
    }

    
}

//--------------------------------------------------------------
void testApp::draw(){
	secondaryDisplay.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'd'){
        debug = !debug;
        secondaryDisplay.debug = debug;
    }
    else if(key == 'c'){
        secondaryDisplay.color = !secondaryDisplay.color;
    }
    else if(key == OF_KEY_RIGHT){
        secondaryDisplay.tx += .1;
        cout << "tx: "<<secondaryDisplay.tx<<endl;
    }
    else if(key == OF_KEY_LEFT){
        secondaryDisplay.tx -= .1;
        cout << "tx: "<<secondaryDisplay.tx<<endl;
    }
    
    if(key == 'a') testAllClips = true;
    if(key == 's') secondaryDisplay.respondToClip( secondaryDisplay.parser.getAllClips()[0] );
    
	
	if(key == 'C'){
		secondaryDisplay.respondToClip( secondaryDisplay.parser.getRandomClip() );
	}
	
	if(key == 'E'){
		vector<int> projectExampleIndecs;
		for(int i = 0; i < secondaryDisplay.parser.getAllClips().size(); i++){
			if(secondaryDisplay.parser.getAllClips()[i].hasProjectExample){
				projectExampleIndecs.push_back(i);
			}
		}
		
		if(projectExampleIndecs.size() > 0){
			int exampleIndex = projectExampleIndecs[ ofRandom(projectExampleIndecs.size()) ];
			secondaryDisplay.respondToClip( secondaryDisplay.parser.getAllClips()[exampleIndex] );
//			cout << "SENT CLIP " << parser.getAllClips()[exampleIndex].getLinkName() << " WITH EXAMPLE " << parser.getAllClips()[exampleIndex].projectExampleTitle << endl;
		}
	}
    
    if(key == 'S'){
        //reload shader
        secondaryDisplay.reloadShader();
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