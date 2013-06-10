#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
    
	if(ofDirectory("../../../CloudsData/").exists()){
		parser.parseLinks("../../../CloudsData/links/clouds_link_db.xml");
		parser.setup("../../../CloudsData/fcpxml/");
        parser.parseClusterMap("../../../CloudsData/gephi/CLOUDS_test_5_26_13.SVG");
	}
	else{
		cout << "SETTING UP IN DATA DIRECTORY" << endl;
		parser.parseLinks("clouds_link_db.xml");
		parser.setup("xml");
	}}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	//draw svg
    ofBackground(0);
    for(int i=0; i<parser.allClips.size();i++){
        ofSetColor(parser.allClips[i].cluster.Color);
        ofCircle(parser.allClips[i].cluster.Centre.x*ofGetWidth(),parser.allClips[i].cluster.Centre.y*ofGetHeight(),parser.allClips[i].cluster.Radius*2);
        
    }
    //highlight people, topics, nodes
    
    //per topic, what is it's radius
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    for(int i=0; i<parser.allClips.size();i++){
    
        cout<<i<<"::"<<parser.allClips[i].cluster.Color.getHex()<<endl;
    }
}

//--------------------------------------------------------------
void testApp::exit(){
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