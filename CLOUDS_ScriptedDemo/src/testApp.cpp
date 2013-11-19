#include "testApp.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	ofToggleFullscreen();
    parser.loadFromFiles();
	
	if(!ofFile::doesFileExist(getDataPath() + "CloudsMovieDirectory.txt")){
		ofSystemAlertDialog("Could not find movie file path. \
							Create a file called CloudsMovieDirectory.txt \
							that contains one line, the path to your movies folder");
	}

	parser.setCombinedVideoDirectory(ofBufferFromFile(getDataPath() + "CloudsMovieDirectory.txt").getText());
	
	//visualSystems.populateVisualSystems();
	visualSystems.loadPresets();
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	storyEngine.printDecisions = false;
	storyEngine.combinedClipsOnly = true;
	storyEngine.setup();
	sound.setup(storyEngine);
	
	player.setup();
	player.setStoryEngine(storyEngine);
    player.setRun(run);
	player.getClusterMap().buildEntireCluster(parser);
	

	////////SEED WITH RANDOM CLIP
//	srand( ofGetSeconds()*1000 );
//	CloudsClip& clip = parser.getRandomClip(false,false);
//	storyEngine.buildAct(run, clip, clip.getKeywords()[0] );
	////////SEED WITH RANDOM CLIP
	
	//////////////SHOW INTRO
//	vector<CloudsClip> startingNodes = parser.getClipsWithKeyword("#start");
//	//safe guard delete any starters that don't have questions
//	for(int i = startingNodes.size()-1; i >= 0; i--){
//		if(!startingNodes[i].hasQuestion() ) {
//			ofLogError() << "Clip " << startingNodes[i].getID() << " is labeled as #start but has no question, removing.";
//			startingNodes.erase(startingNodes.begin() + i);
//		}
//		else if(!startingNodes[i].hasCombinedVideo){
//			ofLogError() << "Clip " << startingNodes[i].getID() << " has no combined video file, removing.";
//			startingNodes.erase(startingNodes.begin() + i);
//		}
//		else{
//			cout << " Adding Clip " << startingNodes[i].getID() << " with question " << startingNodes[i].getQuestions()[0] << endl;
//		}
//	}
//	
//	cout << "Starting with " << startingNodes.size() << endl;
//	player.showIntro(startingNodes);
	//////////////SHOW INTRO
	
	//temp sound stuff
//	sound.setMasterAmp(1.0);
//	useScratch = false;
	
	CloudsVisualSystemPreset& LIA = visualSystems.getPresetForSystem( "Lia", "LIA_01" );
	CloudsVisualSystemPreset& vectormath = visualSystems.getPresetForSystem( "ExampleVectorMath", "WhiteLineOC");
//
    CloudsVisualSystemPreset& yellowTailPreset = visualSystems.getPresetForSystem( "YellowTail", "YellowTailOC");
   // CloudsVisualSystemPreset& spaghetti = visualSystems.getPresetForSystem( "OpenP5Spaghetti", "BlueScribblesOC");
    
    CloudsVisualSystemPreset& spaghetti = visualSystems.getPresetForSystem( "OpenP5Spaghetti", "BlueSmoothScribblesOC");
    
    CloudsVisualSystemPreset& flock = visualSystems.getPresetForSystem( "ExampleOpenSteer", "BasicOC");
    
    CloudsVisualSystemPreset& blobby = visualSystems.getPresetForSystem( "MandalaComponents", "PureFormOC");
    
    CloudsVisualSystemPreset& pulse = visualSystems.getPresetForSystem( "MandalaComponents", "JM_pulse1OC");
    
    CloudsVisualSystemPreset& maze = visualSystems.getPresetForSystem( "MazeGenerator", "JM_flyoverOC");
    
    CloudsVisualSystemPreset& connectors = visualSystems.getPresetForSystem( "Connectors", "Rotating_BoxOC");
    
    CloudsVisualSystemPreset& ocean_gmuk = visualSystems.getPresetForSystem( "Ocean", "g_gentleWavesOC");
    
    CloudsVisualSystemPreset& ocean_regular = visualSystems.getPresetForSystem( "Ocean", "NightOceanOC");
    
    CloudsVisualSystemPreset& neurons = visualSystems.getPresetForSystem( "Neurons", "MezzyNeuronsOC");

    CloudsAct* act = new CloudsAct();
	vector<string> clipIds;
	clipIds.push_back("Shantell - Coding gesturally");
//	clipIds.push_back("Golan - make a mark");
	clipIds.push_back("Karsten - immediate feedback");
	clipIds.push_back("Shiffman - sketching with code");
	//clipIds.push_back("Shantell - work with zach");
	//clipIds.push_back("Maeda - last trick");
	//clipIds.push_back("Golan - yellow tail");
	clipIds.push_back("Shantell - drawing with computers");
	clipIds.push_back("Zach - Time slows down");
	//clipIds.push_back("Lauren - Lifelong project");
	clipIds.push_back("Marius - Continuous process of exploration");
	clipIds.push_back("Casey - tangle");
	clipIds.push_back("Golan - woah");
    clipIds.push_back("Vera - Playing with a system");
    clipIds.push_back("Shiffman - how far could we get?");
    clipIds.push_back("Casey - Systems theory");
    clipIds.push_back("Casey - Developing a microworld");
    clipIds.push_back("Kyle_CH - coding is 1");
    clipIds.push_back("Shiffman - reality will only get you so far");
    clipIds.push_back("Karolina - suspension of disbelief");
    clipIds.push_back("Lauren - real and virtual 1");
    clipIds.push_back("JTNimoy - immersion");
  
	float lastClipEndTime = 0;
	for(int i = 0; i < clipIds.size(); i++){
		CloudsClip& clip = parser.getClipWithLinkName(clipIds[i]);
		
        if(clipIds[i] == "Karsten - immediate feedback"){
//			act->addVisualSystem( vectormath, lastClipEndTime + 5, 30); //start the preset 5 seconds in, play for 80 seconds
		}
        
        if(clipIds[i] == "Zach - Time slows down"){
//			act->addVisualSystem( connectors, lastClipEndTime + 10, 30); //start the preset 5 seconds in, play for 80 seconds
		}
        
        if(clipIds[i] == "Golan - woah"){
//			act->addVisualSystem( blobby, lastClipEndTime+1, 7);
          //  act->addVisualSystem( pulse, lastClipEndTime+6, 5);
		}
        
        if(clipIds[i] == "Vera - Playing with a system"){
//            act->addVisualSystem( maze, lastClipEndTime+1, 15);
        }
    
        
        if(clipIds[i] == "Shiffman - how far could we get?"){
//          act->addVisualSystem( flock, lastClipEndTime+5, 20);
        }
    
        if(clipIds[i] == "Kyle_CH - coding is 1"){
//            act->addVisualSystem( ocean_gmuk, lastClipEndTime+1, 15);
        }
        
        if(clipIds[i] == "Lauren - real and virtual 1"){
//            act->addVisualSystem( neurons, lastClipEndTime+1, 15);
        }
        
        if(clipIds[i] == "JTNimoy - immersion"){
            act->addVisualSystem( ocean_regular, lastClipEndTime+4, 15);
        }
		
		lastClipEndTime = act->addClip(clip, "topic", lastClipEndTime+2);
	}
	
	//play the visual systems for some time
	act->addVisualSystem( ocean_gmuk,  0, 20 );

	storyEngine.setCustomAct(act);
	
	vector<CloudsClip> question;
	question.push_back( parser.getClipWithLinkName(clipIds[0]) );
	question.push_back( parser.getClipWithLinkName(clipIds[0]) );
	question.push_back( parser.getClipWithLinkName(clipIds[0]) );
	question.push_back( parser.getClipWithLinkName(clipIds[0]) );
	question.push_back( parser.getClipWithLinkName(clipIds[0]) );
	question.push_back( parser.getClipWithLinkName(clipIds[0]) );
	question.push_back( parser.getClipWithLinkName(clipIds[0]) );
	question.push_back( parser.getClipWithLinkName(clipIds[0]) );
	
	player.showIntro(question);
	
}

//--------------------------------------------------------------
void testApp::update(){
	player.getSharedVideoPlayer().maxVolume = sound.maxSpeakerVolume;
	
	sound.update();
	ofShowCursor();
}

//--------------------------------------------------------------
void testApp::draw(){
//	sound.drawDebug();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'p'){
		useScratch = !useScratch;
		if(useScratch){
			player.setUseScratch( true );
			sound.setMasterAmp(0.0);
		}
		else{
			player.setUseScratch( false );
			sound.setMasterAmp(1.0);
		}
	}
    
//    if( key == 'T'){
//        CloudsClip& clip = parser.getRandomClip(true,true);
//        player.setRandomQuestion(clip);
//        
//    }
    
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