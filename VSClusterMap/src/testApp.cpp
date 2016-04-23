#include "testApp.h"
#include "ofxObjLoader.h"

//--------------------------------------------------------------
void testApp::setup(){
	
    /*
    long size;
    char *buf;
    char *ptr;
    
    size = pathconf(".", _PC_PATH_MAX);
    if ((buf = (char *)malloc((size_t)size)) != NULL){
        ptr = getcwd(buf, (size_t)size);
    }
    cout << ptr << endl;
    */

	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	parser.loadFromFiles();
	visualSystems.loadPresets();
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	storyEngine.printDecisions = false;
	storyEngine.combinedClipsOnly = false;
	storyEngine.setup();
	
	vector<CloudsClip*> startingNodes = parser.getClipsWithKeyword("#start");
	srand(ofGetSeconds());
    cout << "starting node size is " << startingNodes.size() << endl;
    
	ofRandomuf();
    ofRandomuf();
    ofRandomuf();
    
    int startNode = ofRandomuf()*startingNodes.size();
    cout << "START NODE index is " << startNode << endl;
    
	CloudsAct* act = storyEngine.buildAct(run, startingNodes[ startNode ]);
	run.topicHistory = act->getAllTopics();
	run.clipHistory = act->getAllClips();
	cout << "** TRAVERSAL INCLUDES:" << endl;
	for(int i = 0; i < run.clipHistory.size(); i++){
		cout << "	** " << run.clipHistory[i]->getID() << endl;
	}
    
//	clusterMap.forceScreenResolution(1920, 1080);
	clusterMap.setup();
#ifdef OCULUS_RIFT
	if(CloudsVisualSystem::getOculusRift().isHD()){
		clusterMap.loadPresetGUISFromName("FollowTraverse_OculusHD");
	}
	else{
		clusterMap.loadPresetGUISFromName("FollowTraverse_OculusSD");
	}
#else
//	clusterMap.loadPresetGUISFromName("FollowTraverse_Screen");
    clusterMap.loadPresetGUISFromName("TopicResearch_pretty");
#endif

	clusterMap.setRun(run);
	clusterMap.setAct(act);
	clusterMap.buildEntireCluster(parser);
	clusterMap.allocateFlickerTexture();
	
//	clusterMap.autoTraversePoints = true;
	clusterMap.populateDummyQuestions();
    
	clusterMap.playSystem();
	
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

}
//--------------------------------------------------------------
void testApp::exit(){
	clusterMap.exit();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
//    ofPixels p;
//    clusterMap.getSharedRenderTarget().readToPixels(p);
//    ofSaveImage(p, "CLUSTER_BACKGROUND.png");
    
    if(key == 'O'){
        ofxObjLoader::save("SAVE_ClusterNodes.obj", clusterMap.getNodeMesh());
        ofxObjLoader::save("SAVE_NetworkPaths.obj", clusterMap.getNetworkMesh());
    }
    
    if(key == '1'){
        clusterMap.setCurrentTopic("community");
    }
    else if(key == '2'){
        clusterMap.setCurrentTopic("bugs");
    }
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