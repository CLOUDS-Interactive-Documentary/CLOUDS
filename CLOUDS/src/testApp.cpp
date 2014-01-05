#include "testApp.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	ofToggleFullscreen();
	

	player.setup();
	//temp sound stuff
	//sound.setMasterAmp(1.0);
//	useScratch = false;
}

//--------------------------------------------------------------
//void testApp::actCreated(CloudsActEventArgs& args){
//	if(currentAct != NULL){
//		currentAct->unregisterEvents(&oscSender);
//	}
//	currentAct = args.act;
//	currentAct->registerEvents(&oscSender);
//}

//--------------------------------------------------------------
void testApp::update(){
//	player.getSharedVideoPlayer().maxVolume = sound.maxSpeakerVolume;
//	sound.update();
}

//--------------------------------------------------------------
void testApp::draw(){
//	sound.drawDebug();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
//	if(key == 'p'){
//		useScratch = !useScratch;
//		if(useScratch){
//			player.setUseScratch( true );
////			sound.setMasterAmp(0.0);
//		}
//		else{
//			player.setUseScratch( false );
////			sound.setMasterAmp(1.0);
//		}
//	}    
//    if( key == 'T'){
//        CloudsClip& clip = parser.getRandomClip(true,true);
//        player.setRandomQuestion(clip);
//        
//    }
    
}

////--------------------------------------------------------------
//void testApp::audioRequested(float * output, int bufferSize, int nChannels) {
//	mixer.fillBuffer(output,bufferSize,nChannels);
//}

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