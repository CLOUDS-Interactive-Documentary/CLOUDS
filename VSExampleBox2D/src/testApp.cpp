#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	int sr = 44100;
	int nbufs = 2; // you can use more for more processing but latency will suffer
	int nchans = 2; // stereo
	int framesize = 512; // sigvs
//	s_audio_outbuf = (short*)malloc(nchans*framesize*sizeof(short)); // audio buffer (interleaved)
	
	// initialize RTcmix
///	rtcmixmain();
//	maxmsp_rtsetparams(sr, nchans, framesize, NULL, NULL);
	
	// initialize OF audio streaming
	ofSoundStreamSetup(nchans, 0, sr, framesize, nbufs);
	
	ofSoundStreamStart();
	exampleBox2D.setup();
	exampleBox2D.playSystem();
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
	exampleBox2D.exit();
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