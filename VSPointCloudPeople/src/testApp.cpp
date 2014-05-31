
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
	
	ofSetVerticalSync(true);
	shouldPlayTestVideo = false;
	ofSetLogLevel(OF_LOG_NOTICE);
    
	CloudsSpeaker::populateSpeakers();

	//parser.loadFromFiles();
	//parser.loadMediaAssets();
	rgbd.setup();

	hud.setup();
#ifdef OCULUS_RIFT
    // Link the HUD.
    rgbd.hud = &hud;
    rgbd.setupHUDGui();
#endif

	//rgbd.forceScreenResolution(1920*2,1080*2);
	//rgbd.setDrawToScreen(false);
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
		rgbd.playTestVideo();
		CloudsClip* clip = new CloudsClip();
		clip->person = "Jen";
		hud.respondToClip(clip);
		CloudsQuestionEventArgs args(clip, "WHAT'S YOUR QUESTION?", "topic");
		hud.questionSelected(args);

		//CloudsClip* clip = parser.getRandomClip(true);
		//rgbd.playTestVideo(clip);
		//hud.respondToClip(clip);
	}
}


//--------------------------------------------------------------
void testApp::draw(){
	//CloudsVisualSystem::getRGBDVideoPlayer().getTextureReference().draw(0,0);
	//rgbd.selfPostDraw(0,0,1920,1080);
	rgbd.getSharedRenderTarget().getTextureReference().draw(0,0,1920,1080);
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
//		ofDirectory qtestnodes( GetCloudsDataPath() + "HUD/QuestionNode_set05");
//		qtestnodes.allowExt("png");
//		qtestnodes.listDir();
//		vector<string> testpaths;
//		for(int i = 0; i < qtestnodes.numFiles(); i++)
//			testpaths.push_back(qtestnodes.getPath(i));
//		rgbd.addFakeQuestion(testpaths);
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
