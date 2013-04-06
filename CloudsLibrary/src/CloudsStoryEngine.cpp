//
//  CloudsStoryEngine.cpp
//  CloudsFCPParser
//
//  Created by James George on 3/17/13.
//
//

#include "CloudsStoryEngine.h"

CloudsStoryEngine::CloudsStoryEngine(){
	visualizer = NULL;
	network = NULL;
	
	hasclip = false;
	printDecisions = true;
}

CloudsStoryEngine::~CloudsStoryEngine(){
	
}

void CloudsStoryEngine::setup(){
	
}

void CloudsStoryEngine::seedWithClip(ClipMarker& seed, string topic){
	
	history.clear();

	//regen
	if(topic == ""){
		currentTopic = seed.keywords[ ofRandom(seed.keywords.size()) ];
	}
	else{
		currentTopic = topic;
	}

	loadClip( seed );
}


void CloudsStoryEngine::loadClip(ClipMarker& clip){

	hasclip = true;
	currentClip = clip;
	history.push_back( clip );
	
	//visualizer->addLinksToPhysics( clip );
}

bool CloudsStoryEngine::selectNewClip(){
	
	//CHECK PRECONDITION
	if(visualizer == NULL || network == NULL){
		ofLogError("Clouds Visualizer or Database is null! exiting select clip");
		return false;
	}
	
	//We need a clip to be able to select a new one.
	//call SeedWithClip() before calling selectNewClip()
	if(!hasclip){
		ofLogError("Cannot select new clip without a seed");
		return false;
	}
	
	//get all the adjascent clips, assign weights to them and select
	vector<ClipMarker> related = network->getClipsWithKeyword(currentClip.keywords);
	
	if(printDecisions){
		cout << "REALTED CLIPS:" << endl;
		for(int i = 0; i < related.size(); i++){
			cout << "	\"" << related[i].getLinkName() << "\""
				 << " Topics: [" << ofJoinString(related[i].keywords, ",") << "]" << endl;
		}
	}

	cout << "ASSIGNING VALUES:" << endl;
	int totalPoints = 0;
	vector< pair<int, ClipMarker> > clipScores;
	for(int i = 0; i < related.size(); i++){
		ClipMarker& m = related[ i ];
		int score = scoreForClip( m );
		if(score != 0){
			if(printDecisions) cout << "Clip " << m.getLinkName() << " assigned score of " << score << endl;
			clipScores.push_back( make_pair(totalPoints, m) );
			totalPoints += score;
		}
	}
	
	if(printDecisions){
		cout << "VALID CLIPS:" << endl;
		for(int i = 0; i < clipScores.size(); i++){
			cout << "	" << clipScores[i].second.getLinkName() << endl;
		}
	}
	
	//if no clips were valid, return false
	if(totalPoints == 0){
		ofLogError("Dead end found at clip " + currentClip.getLinkName());
		return false;
	}

	
	//Do a weighted selection based on the random value
	int randomOption = ofRandom(totalPoints);
	int selection;
	for(selection = 1; selection < clipScores.size(); selection++){
		if(clipScores[selection].first > randomOption){
			break;
		}
	}
	selection--;

	cout << "SELECTED CLIP:" << selection << "/" << clipScores.size() << " "
		 << clipScores[selection].second.getLinkName() << endl;
	
	//load the given clip
	loadClip( clipScores[selection].second );
	
	return true;
}

// nice reference on picking random weighted objcts
// http://www.perlmonks.org/?node_id=577433

int CloudsStoryEngine::scoreForClip(ClipMarker& clip){
	//rejection criteria
	if(clip.person == currentClip.person){
		if(printDecisions) cout << "	Clip " << clip.getLinkName() << " rejected: same person " << endl;
		return 0;
	}
	
	if(!ofContains(clip.keywords, currentTopic) ){
		if(printDecisions) cout << "	Clip " << clip.getLinkName() << " rejected: not on topic " << currentTopic << endl;
		return 0;
	}

	//STUB
	//
	
	return 10;
}
