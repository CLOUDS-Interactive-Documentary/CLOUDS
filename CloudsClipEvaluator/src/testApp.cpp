#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
    ofEnableSmoothing();
	
	currentAct = NULL;
	//rebuildAct = false;
	
	parser.loadFromFiles();
	parser.loadMediaAssets();
    
	visualSystems.loadPresets();
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	
	storyEngine.setup();
    
    for(int run = 0; run < 10; run++){
        vector<CloudsClip*> clips = storyEngine.getStartingQuestions();
        random_shuffle(clips.begin(), clips.end());
        
        for(int i = 0; i < clips.size(); i++){
            
            CloudsClip* clip = clips[i];
            string topic = clip->getAllQuestionTopicPairs().begin()->first;
            CloudsRun run;
            exploreAct(clip,topic,true,run,0,10);

        }
        
    }
    
    map<CloudsClip*,int>::iterator it;
    int clipid = 0;
    for(it = traversedClips.begin(); it != traversedClips.end(); it++){
        cout << "(" << it->second << ") " << clipid++ << "/" << traversedClips.size() << " " << it->first->getLinkName() << endl;
    }
    
    cout << "INCLUDED CLIPS" << endl;
    ofBuffer shellscript;
    shellscript.append("#!/bin/bash\n");
    for(it = traversedClips.begin(); it != traversedClips.end(); it++){
        CloudsClip* clip = it->first;
        if(clip->hasMediaAsset){
            string mediaFile = ofToDataPath(clip->combinedVideoPath,true);
            string xmlFile   = ofToDataPath(clip->combinedCalibrationXMLPath,true);
            string containingFolder = ofFilePath::addTrailingSlash(ofFilePath::getEnclosingDirectory( mediaFile));
            
            shellscript.append(string("echo \"copying ") + ofFilePath::getFileName(mediaFile) + "\"\n" );
            shellscript.append(string("cp \"") + mediaFile + "\" \"" + containingFolder + "__keepers/\"\n" );
            shellscript.append(string("cp \"") + xmlFile   + "\" \"" + containingFolder + "__keepers/\"\n\n");
            
            //cout << clip->getLinkName()<<endl;
        }
    }
    
    ofBufferToFile(GetCloudsDataPath()+"copyscript.sh", shellscript);
    
    cout << "EXCLUDED CLIPS" << endl;
    for(int i = 0; i < parser.getAllClips().size(); i++){
        if(traversedClips.find(parser.getAllClips()[i]) == traversedClips.end()){
            cout << parser.getAllClips()[i]->getLinkName() << endl;
        }
    }
    
}

//--------------------------------------------------------------
void testApp::exploreAct(CloudsClip* clip, string topic, bool playSeed, CloudsRun run, int depth, int maxDepth){
    if(depth == maxDepth){
        return;
    }
    
    if(topic == ""){
        ofLogError("TOPIC IS BLANK");
        return;
    }
    
    CloudsAct* act = storyEngine.buildAct(storyEngine.runTest, clip, topic, playSeed);
    //copy( act->getAllClips().begin(), act->getAllClips().end(), inserter( traversedClips, traversedClips.end() ) );
    for(int i = 0; i < act->getAllClips().size(); i++){
        traversedClips[act->getAllClips()[i]]++;
    }
    
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
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){


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