#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	secondaryDisplay.setup();
    
    debug = false;
}

//--------------------------------------------------------------
void testApp::update(){
	secondaryDisplay.update();
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
    
    if(key == 'R'){
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