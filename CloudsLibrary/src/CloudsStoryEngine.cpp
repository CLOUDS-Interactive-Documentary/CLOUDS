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
	printDecisions = true;
}

CloudsStoryEngine::~CloudsStoryEngine(){
	
}

void CloudsStoryEngine::setup(){
	
}

void CloudsStoryEngine::selectNewClip(){
	
	if(visualizer == NULL){
		return;
	}
	
		
	int totalPoints = 0;
	vector< pair<int, ClipMarker> > clipScores;
	for(int i = 0; i < visualizer->currentOptionClips.size(); i++){
		ClipMarker& m = visualizer->currentOptionClips[ i ];
		int score = scoreForClip( m );
		if(score != 0){
			if(printDecisions) cout << "Clip " << m.getLinkName() << " assigned score of " << score << endl;
			clipScores.push_back( make_pair(totalPoints, m) );
			totalPoints += score;
		}
	}
		
	int randomOption = ofRandom(totalPoints);
	int selection;
	for(selection = 1; selection < clipScores.size(); selection++){
		if(clipScores[selection].first > randomOption){
			break;
		}
	}
	selection--;
	
	//cout << "picking random of " << randomOption << "/" << visualizer->currentOptionClips.size() << endl;
	
	visualizer->addLinksToPhysics( clipScores[selection].second );

//	if(visualizer->currentOptionClips.size() > 0){

	
	
}

// nice reference on picking random weighted objcts
// http://www.perlmonks.org/?node_id=577433

int CloudsStoryEngine::scoreForClip(ClipMarker& clip){
	//rejection criteria
	if(clip.person == currentPerson){
		if(printDecisions) cout << "Clip " << clip.getLinkName() << " rejected: same person " << endl;
		return 0;
	}
	
	if(currentTopic != "" && !ofContains(clip.keywords, currentTopic) ){
		if(printDecisions) cout << "Clip " << clip.getLinkName() << " rejected: not on topic " << endl;
		return 0;
	}
	
	return 10;
}
