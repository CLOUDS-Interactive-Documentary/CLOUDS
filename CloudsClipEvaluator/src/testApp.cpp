#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
    ofEnableSmoothing();
	
	currentAct = NULL;
	rebuildAct = false;
	
	parser.loadFromFiles();
	parser.loadMediaAssets();
    
	visualSystems.loadPresets();
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	
	storyEngine.setup();
    
    vector<CloudsClip*> clips = storyEngine.getStartingQuestions();
    random_shuffle(clips.begin(), clips.end());
    
    for(int i = 0; i < clips.size(); i++){
        
        CloudsClip* clip = clips[i];
        string topic = clip->getAllQuestionTopicPairs().begin()->first;
        CloudsRun run;
        exploreAct(clip,topic,true,run,0,10);

    }
    
    set<CloudsClip*>::iterator it;
    int clipid = 0;
    for(it = traversedClips.begin(); it != traversedClips.end(); it++){
        cout << clipid++ << "/" << traversedClips.size() << " " << (*it)->getLinkName() << endl;
    }
    
    ofBuffer shellscript;
    shellscript.append("#!/bin/bash\n");
    for(it = traversedClips.begin(); it != traversedClips.end(); it++){
        CloudsClip* clip = *it;
        if(clip->hasMediaAsset){
            string mediaFile = ofToDataPath(clip->combinedVideoPath,true);
            string xmlFile   = ofToDataPath(clip->combinedCalibrationXMLPath,true);
            string containingFolder = ofFilePath::addTrailingSlash(ofFilePath::getEnclosingDirectory( mediaFile));
            
            shellscript.append(string("echo \"copying ") + ofFilePath::getFileName(mediaFile) + "\"\n" );
            shellscript.append(string("cp \"") + mediaFile + "\" \"" + containingFolder + "__keepers/\"\n" );
            shellscript.append(string("cp \"") + xmlFile   + "\" \"" + containingFolder + "__keepers/\"\n\n"  );
        }
    }
    
    ofBufferToFile(GetCloudsDataPath()+"copyscript.sh", shellscript);
    
	//ofAddListener(storyEngine.getEvents().actCreated, this, &testApp::actCreated);
}

//--------------------------------------------------------------
void testApp::exploreAct(CloudsClip* clip, string topic, bool playSeed, CloudsRun run, int depth, int maxDepth){
    if(depth == maxDepth){
        return;
    }
    
    if(topic == ""){
        ofLogError("TOPIC IS BLANK");
    }
    
    CloudsAct* act = storyEngine.buildAct(storyEngine.runTest, clip, topic, playSeed);
    copy( act->getAllClips().begin(), act->getAllClips().end(), inserter( traversedClips, traversedClips.end() ) );

    for(int c = 0; c < act->getAllQuestions().size(); c++){
        if(traversedClips.find(act->getAllQuestions()[c].first) == traversedClips.end()){
            cout << "TAKING QUESTION  " << act->getAllQuestions()[c].first->getLinkName() << " with TOPIC " <<  act->getAllQuestions()[c].second << " DEPTTH " << depth+1 << "/" << maxDepth << endl;
            exploreAct(act->getAllQuestions()[c].first, act->getAllQuestions()[c].second, true, run, depth+1, maxDepth);
        }
    }

    if(act->getAllClips().size() != 0){
        cout << "CONTINUING ACT  " << act->getAllClips().back()->getLinkName() << " with TOPIC " <<  act->getAllTopics().back() << " DEPTH " << depth+1 << "/" << maxDepth << endl;
    
        exploreAct(act->getAllClips().back(), act->getAllTopics().back(), false, run, depth+1, maxDepth);
    }
    
    delete act;
}

//--------------------------------------------------------------
void testApp::actCreated(CloudsActEventArgs& args){
	
	if(currentAct != NULL){
		currentAct->unregisterEvents(this);
		delete currentAct;
	}

	currentAct = args.act;
	currentAct->registerEvents(this);
	currentAct->play();
    currentAct->getTimeline().enableEvents();
}

//--------------------------------------------------------------
void testApp::actBegan(CloudsActEventArgs& args){
}

//--------------------------------------------------------------
void testApp::actEnded(CloudsActEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::clipBegan(CloudsClipEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::topicChanged(CloudsTopicEventArgs& newTopic){
	
}

void testApp::questionProposed(CloudsQuestionEventArgs& args){
    
}

void testApp::questionSelected(CloudsQuestionEventArgs& args){
    
}

void testApp::preRollRequested(CloudsPreRollEventArgs& clip){
    
}
//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    if(currentAct != NULL){
		currentAct->drawDebug();
	}
}

//--------------------------------------------------------------
void testApp::exit(){
    storyEngine.saveGuiSettings();
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    ofxUIButton* b = (ofxUIButton*) e.widget;
    if(name == "BUILD ACT" &&  b->getValue() ){
        rebuildAct = true;
    }

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if(key == 'h'){
        storyEngine.toggleGuis();
    }
    else if(key =='f'){
        ofToggleFullscreen();
    }
	if(key == 't'){
		storyEngine.positionGuis();
	}
	if(key == 'S'){
		storyEngine.saveGuiSettings();
	}

#ifndef CLOUDS_NO_OSC
	if(key == 'E'){
		vector<int> projectExampleIndecs;
		for(int i = 0; i < parser.getAllClips().size(); i++){
			if(parser.getAllClips()[i].hasProjectExample){
				projectExampleIndecs.push_back(i);
			}
		}
		
		if(projectExampleIndecs.size() > 0){
			int exampleIndex = projectExampleIndecs[ ofRandom(projectExampleIndecs.size()) ];
			oscSender.sendClip( parser.getAllClips()[exampleIndex]);
			cout << "SENT CLIP " << parser.getAllClips()[exampleIndex].getLinkName() << " WITH EXAMPLE " << parser.getAllClips()[exampleIndex].projectExampleTitle << endl;
		}
	}
	
	if(key == 'C'){
		oscSender.sendClip( parser.getAllClips()[ ofRandom(parser.getAllClips().size()) ] );
	}
#endif

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