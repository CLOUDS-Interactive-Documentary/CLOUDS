
#include "testApp.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsGlobal.h"
#include "CloudsSpeaker.h"

#ifdef KINECT_INPUT
#include "CloudsInputKinectOSC.h"
#endif

#ifdef OCULUS_RIFT
#include "CloudsInputOculus.h"
#endif

//--------------------------------------------------------------
void testApp::setup(){

	//ofSetWindowPosition(0,0);
#ifdef OCULUS_RIFT
//	ofSetWindowShape(1920*2,1080);
    ofSetVerticalSync(false);
	ofSetBackgroundAuto(false);
#else
	ofSetWindowShape(1920,1080);
	ofSetVerticalSync(true);
#endif
	shouldPlayTestVideo = false;
	ofSetLogLevel(OF_LOG_NOTICE);
    
	CloudsSpeaker::populateSpeakers();
	map<string,CloudsSpeaker>::iterator it;
	for(it = CloudsSpeaker::speakers.begin(); it != CloudsSpeaker::speakers.end(); it++){
		speakerKeys.push_back(it->first);
	}
	curSpeaker = 0;

	parser.loadFromFiles();
	hud.setup();
	
	////////////////////////////////////////////////
	//stringstream questions;
	//for(int i = 0; i < parser.getAllClips().size(); i++){
	//	CloudsClip* clip = parser.getAllClips()[i];
	//	if(clip->isLanguageCompatible() && clip->hasQuestion()){
	//		map<string,string>::iterator it;
	//		for( it = clip->getAllQuestionTopicPairs().begin(); it != clip->getAllQuestionTopicPairs().end(); it++){
	//			questions << clip->getLinkName() << "	" << it->first << "	" << it->second << "	" << (clip->hasSpecialKeyword("#oculus") || clip->hasSpecialKeyword("#start") ? "YES" : "NO") << endl;
	//		}
	//	}
	//}
	//ofBufferToFile(GetCloudsDataPath() + "TRANSLATED_question_topics.txt", ofBuffer(questions.str()));
	////////////////////////////////////////////////

	testVideoIndex = 0;
	rgbd.setup();

#ifdef OCULUS_RIFT
    // Link the HUD.
//    rgbd.hud = &hud;
//    rgbd.setupHUDGui();
#else
	rgbd.setDrawToScreen(false);

#endif
	CloudsVisualSystem::getRGBDVideoPlayer().showingLowerThirds = true;

	//rgbd.forceScreenResolution(1920*2,1080*2);
	//rgbd.addTransionEditorsToGui();
	rgbd.playSystem();

	hud.setHudEnabled(true);

	type = CloudsVisualSystem::FLY_THROUGH;
}

//--------------------------------------------------------------
void testApp::update(){

	//updateTransitions();
	hud.update();
	if(shouldPlayTestVideo){
		shouldPlayTestVideo = false;
		cout << "**** playing test video" << endl;
		rgbd.playTestVideo(testVideoIndex);

		CloudsClip* clip = new CloudsClip();
		if(testVideoIndex == 0){
			clip->person = "Zach";
		}
		else{		
			clip->person = "Higa";
		}
		hud.respondToClip(clip);
		CloudsQuestionEventArgs args(clip, "WHAT'S YOUR QUESTION?", "topic");
		//hud.questionSelected(args);
	}
}

//--------------------------------------------------------------
void testApp::draw(){

#ifdef OCULUS_RIFT
	
#else
	ofBackground(0);
	ofDisableDepthTest();
	ofEnableAlphaBlending();
	rgbd.selfPostDraw();
	ofSetColor(255);
	hud.draw();
	CloudsVisualSystem::getRGBDVideoPlayer().drawSubtitles();
	ofDisableDepthTest();
#endif



}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'O'){
		rgbd.StopEditTransitionMode();//<-- used to revert the camera  to the rgbd camera. it only matters in "Edit" mode
		transitionController.transitionToVisualSystem(1.0, 1.0);
	}
	if(key == 'I'){
		rgbd.StopEditTransitionMode();//<-- used to revert the camera  to the rgbd camera. it only matters in "Edit" mode
		transitionController.transitionToInterview(1.0, 1.0);
	}

	if(key == OF_KEY_LEFT){
		curSpeaker = (curSpeaker + 1) % speakerKeys.size();
		CloudsClip clip;
		clip.person = speakerKeys[curSpeaker];
		hud.respondToClip(&clip);
	}
	else if(key == OF_KEY_RIGHT){
		curSpeaker--;
		if(curSpeaker < 0) curSpeaker += speakerKeys.size();
		
		CloudsClip clip;
		clip.person = speakerKeys[curSpeaker];
		hud.respondToClip(&clip);	
	}
	else if(key == '1'){
		testVideoIndex = 0;
		shouldPlayTestVideo = true;
	}
	else if(key == '2'){
		testVideoIndex = 1;
		shouldPlayTestVideo = true;	
	}
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){
	if(key == ' '){
		shouldPlayTestVideo = true;
	}
	
	if(key == 'S'){
		ofFbo fbo;
		fbo.allocate(1920*2,1080*2,GL_RGB);
		fbo.begin();
		rgbd.selfPostDraw();
		fbo.end();
		ofPixels p;
		fbo.readToPixels(p);
		char filename[1024];
		sprintf(filename, "Screenshot_%02d_%02d_%02d_%02d_%02d.png", ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds());
		ofSaveImage(p,filename);
		cout << "Saving to " << filename << endl;
		//ofSaveScreen("TestFrame.png");
	}

	if(key == 'Q'){
		CloudsClip* clip = parser.getRandomClip(false,true);
		if(clip->hasQuestion()){
			rgbd.addQuestion(clip, clip->getTopicsWithQuestions()[0], clip->getQuestions()[0] );
		}
		else {
			cout << "clip " << clip->getLinkName() << " does not have a question!" << endl;
		}
	}

	if(key == 'm'){
		ofHideCursor();
	}
	if(key == 'M'){
		ofShowCursor();
	}
}
//--------------------------------------------------------------
void testApp::exit(){
	rgbd.exit();
}

void testApp::updateTransitions(){
	
	transitionController.update();
	
	float crossfadeValue = transitionController.getFadeValue();
	rgbd.visualSystemFadeValue = crossfadeValue;
	
//	cout << "\tCUR STATE:" << transitionController.getCurrentStateDescription() << endl
//         << "\tPREVIOUS STATE: " << transitionController.getPreviousStateDescription() << endl
//         << "\tFADE VALUE " << rgbd.visualSystemFadeValue << endl;
	
	if(transitionController.transitioning){
		ofLogNotice("testApp::updateTransitions") << transitionController.getCurrentStateDescription() << " TRANSITIONING: " << transitionController.getInterviewTransitionPoint();
	}
	
	rgbd.updateTransition( transitionController.getInterviewTransitionPoint() );
	
	if(transitionController.isStateNew()){
		
		if(transitionController.getCurrentState() == TRANSITION_INTERVIEW_OUT){
			
			ofLogNotice("testApp::updateTransitions") << "Going to INTERVIEW OUT";
			
			//rgbd.startTransitionOut( type );
			rgbd.startCurrentTransitionOut();
		}
		else if(transitionController.getCurrentState() == TRANSITION_VISUALSYSTEM_IN){
			
			ofLogNotice("testApp::updateTransitions") << "Going to VISUAL SYSTEM IN";
			
			rgbd.transtionFinished();
			rgbd.stopSystem();
		}
		else if(transitionController.getCurrentState() == TRANSITION_VISUALSYSTEM_OUT){
			// no need to do anything special, the crossfade value will take care of this
			ofLogNotice("testApp::updateTransitions") << "Going to VISUAL SYSTEM OUT";
		}
		else if(transitionController.getCurrentState() == TRANSITION_INTERVIEW_IN){
			
			ofLogNotice("testApp::updateTransitions") << "Going to INTERVIEW IN";
			
			rgbd.playSystem();
			//rgbd.startTransitionIn( type );
			rgbd.startCurrentTransitionIn();
		}
		else if(transitionController.getCurrentState() == TRANSITION_IDLE){
			
			ofLogNotice("testApp::updateTransitions") << "Going to IDLE";
			
			rgbd.transtionFinished();
		}
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
