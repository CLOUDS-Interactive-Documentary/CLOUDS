#include "testApp.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
  
	rgbd.setup();
	rgbd.playSystem();
	
	
	
}

//--------------------------------------------------------------
void testApp::update(){
}


//--------------------------------------------------------------
void testApp::draw(){
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::exit(){
	rgbd.exit();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	if(key == ' '){
		rgbd.playTestVideo();
	}
	
	if(key == 'Q'){
		ofDirectory qtestnodes( GetCloudsDataPath() + "HUD/QuestionNode_set05");
		qtestnodes.allowExt("png");
		qtestnodes.listDir();
		vector<string> testpaths;
		for(int i = 0; i < qtestnodes.numFiles(); i++)
			testpaths.push_back(qtestnodes.getPath(i));
		rgbd.addFakeQuestion(testpaths);
	}
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
