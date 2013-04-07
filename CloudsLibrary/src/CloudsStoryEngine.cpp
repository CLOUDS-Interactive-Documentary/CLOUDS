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

void CloudsStoryEngine::seedWithClip(ClipMarker& seed){
	
	
	clipHistory.clear();
	topicHistory.clear();
	peopleVisited.clear();
	
	freeTopic = true;
	
	loadClip( seed );
	
}

ClipMarker& CloudsStoryEngine::getCurrentClip(){
	return currentClip;
}

vector<ClipMarker>& CloudsStoryEngine::getClipHistory(){
	return clipHistory;
}

void CloudsStoryEngine::loadClip(ClipMarker& clip){

	if(hasclip && (freeTopic || currentTopic == "") ){
		chooseNewTopic(clip);
	}
	
	hasclip = true;
	
	timesOnTopic++;
	if(timesOnTopic > maxTimesOnTopic){
		freeTopic = true;
	}
	
	currentClip = clip;
	clipHistory.push_back( clip );
	peopleVisited[ clip.person ]++;
	
	//visualizer->addLinksToPhysics( clip );
}


void CloudsStoryEngine::chooseNewTopic(ClipMarker& upcomingClip){
	vector<string> topics = network->getSharedKeywords(currentClip, upcomingClip);
	ofRandomize(topics);
	
	bool topicSwitched = false;
	cout << "TOPIC SWITCH: SWITCHING FROM " << currentTopic << endl;;
	if(topics.size() > 0){
		timesOnTopic = 0;
		freeTopic = false;
		for(int i = 0; i < topics.size(); i++){
			int historyBack = MIN(topicHistory.size(), topicTimeoutPeriod);
			list<string>::iterator it = topicHistory.end();
			advance(it, -5);
			
			list<string>::iterator findIter = find(it, topicHistory.end(), topics[i] );
			if(findIter == topicHistory.end() ){
				currentTopic = topics[ i ];
				topicSwitched = true;
				break;
			}
			else{
				cout << "	TOPIC SWITCH: ALREADY VISITED " << *findIter << endl;
			}
		}
		
		topicHistory.push_back(currentTopic);
	}
	else{
		cout << "	TOPIC SWITCH: NO SHARED TOPICS" << endl;
	}
	
	if(topicSwitched){
		cout << "	TOPIC SWITCH TO " << currentTopic << endl;
	}
	else{
		cout << "	FAILED TO SWITCH TOPIC " << currentTopic << endl;
	}
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
	
//	if(printDecisions){
//		cout << "REALTED CLIPS:" << endl;
//		for(int i = 0; i < related.size(); i++){
//			cout << "	\"" << related[i].getLinkName() << "\""
//				 << " Topics: [" << ofJoinString(related[i].keywords, ",") << "]" << endl;
//		}
//	}

	cout << "RELATED CLIPS TO: " << currentTopic << " ASSIGNING VALUES:" << endl;
	int totalPoints = 0;
	vector< pair<int, ClipMarker> > clipScores;
	for(int i = 0; i < related.size(); i++){
		ClipMarker& m = related[ i ];
		int score = scoreForClip( m );
		if(score != 0){
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
		
		// if we are at a dead end because of the lack of clips,
		// recurse the call with an open topic
		// May still be dead from exhausted clips or topics on the same node
		if(!freeTopic){
			freeTopic = true;
			return selectNewClip();
		}
		else{
			ofLogError("Dead end found at clip " + currentClip.getLinkName());
		}
		
		 false;
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
		if(printDecisions) cout << "	REJECTED Clip " << clip.getLinkName() << ": same person" << endl;
		return 0;
	}
	
	if(!freeTopic && !ofContains(clip.keywords, currentTopic) ){
		if(printDecisions) cout << "	REJECTED Clip " << clip.getLinkName() << ": not on topic " << currentTopic << endl;
		return 0;
	}

	//reject any nodes we've seen already
	if(historyContainsClip(clip)){
		if(printDecisions) cout << "	REJECTED Clip " << clip.getLinkName() << ": already visited" << endl;
		return 0;
	}
	
	int score = 10;
	
	//STUB
	//
	
	if(printDecisions) cout << "	ACCEPTED Clip " << clip.getLinkName() << " with score " << score << endl;
	
	return score;
}

bool CloudsStoryEngine::historyContainsClip(ClipMarker& m){
	string clipLinkName = m.getLinkName();
	for(int i = 0; i < clipHistory.size(); i++){
		if(clipLinkName == clipHistory[i].getLinkName()){
			return true;
		}
	}
	return false;
}
