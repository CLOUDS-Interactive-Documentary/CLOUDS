#include "testApp.h"

using namespace ofxCv;
//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
//    player.loadMovie("/Volumes/Macintosh HD/Users/apple/Documents/Projects Current/Clouds/Surveillance_debug_footage/Store_Slow3_BW.mp4");
//    player.play();
//    player.update();
//    cout<<player.getTotalNumFrames()<<endl;
    faceTracker.setup();
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
//    player.draw(0, 0, player.getWidth(), player.getHeight());

//	for(int i = 0; i <     player.getTotalNumFrames(); i++){
//        cout<<"frame : "<< i<<" of "<<player.getTotalNumFrames()<<endl;
//		player.setFrame(i);
//		faceTracker.update(toCv(player));//.tarck(video);
//        
//
//		player.draw(0, 0, player.getWidth(), player.getHeight());
//		faceTracker.draw(true);
//
//	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}
void testApp:: createPNGFaceTrackSequence(string videoPath){
    string videoName = " ";
    vector<string> strings = ofSplitString(videoPath, "/");
    videoName = strings[strings.size() - 1];
    ofDirectory(videoName).create();
    cout<<"facetrackin on :"<<videoName<<endl;

    
	player.loadMovie(videoPath);    //player.play();

    fbo.allocate(player.getWidth(), player.getHeight());


	for(int i = 0; i <     player.getTotalNumFrames(); i++){

		player.setFrame(i);
        player.update(  );
		faceTracker.update(toCv(player));


        
		fbo.begin();
		player.draw(0, 0, player.getWidth(), player.getHeight());
//        ofPushMatrix();
        faceTracker.getImageMesh().drawWireframe();

        if(faceTracker.getFound()){
            ofPushMatrix();
            ofTranslate(faceTracker.getPosition().x, faceTracker.getPosition().y);
            ofScale(5,5,5);
            ofDrawAxis(faceTracker.getScale());
            
            ofPopMatrix();
            cout<<"face found"<<endl;
        }
        


//        faceTracker.getMeanObjectMesh().drawWireframe();
//        faceTracker.getImageMesh().drawAce


		fbo.end();

		ofPixels p;
		fbo.readToPixels(p);
        
		char filename[1024];
		sprintf(filename, "%s/file_%05d.png", videoName.c_str(), i);
        cout<<"saving image : "<<filename<<" of "<<player.getTotalNumFrames()<<endl;
       
        ofSaveImage(p,filename);
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
    ofGetElapsedTimeMillis();
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
    cout<<dragInfo.files[0]<<endl;
    createPNGFaceTrackSequence(dragInfo.files[0]);
//    cout<<createPNGFaceTrackSequence(dragInfo.files[0])<<endl;
}