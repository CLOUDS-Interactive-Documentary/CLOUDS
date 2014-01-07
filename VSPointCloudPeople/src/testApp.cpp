
#include "testApp.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
  
	ofSetLogLevel(OF_LOG_NOTICE);
	
	rgbd.setup();
	rgbd.playSystem();
	
	type = CloudsVisualSystem::FLY_THROUGH;
}

//--------------------------------------------------------------
void testApp::update(){
	updateTransitions();
}


//--------------------------------------------------------------
void testApp::draw(){
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'O'){
		transitionController.transitionToVisualSystem(1.0, 1.0);
	}
	if(key == 'I'){
		transitionController.transitionToInterview(1.0, 1.0);
	}
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
void testApp::exit(){
	rgbd.exit();
}

void testApp::updateTransitions(){
	
	transitionController.update();
	
	float crossfadeValue = transitionController.getFadeValue();
	rgbd.visualSystemFadeValue = crossfadeValue;
	
	//cout << "visual system fade value is " << rgbd.visualSystemFadeValue << endl;
	
	if(transitionController.transitioning){
		ofLogNotice("testApp::updateTransitions") << transitionController.getCurrentStateDescription() << " TRANSITIONING: " << transitionController.getInterviewTransitionPoint();
	}
	
	rgbd.updateTransition( transitionController.getInterviewTransitionPoint() );
	
	if(transitionController.isStateNew()){
		
		if(transitionController.getCurrentState() == TRANSITION_INTERVIEW_OUT){
			
			ofLogNotice("testApp::updateTransitions") << "Going to INTERVIEW OUT";
			
			rgbd.startTransitionOut( type );
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
			rgbd.startTransitionIn( type );
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
