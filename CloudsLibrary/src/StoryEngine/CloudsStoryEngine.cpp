//
//  CloudsStoryEngine.cpp
//  CloudsFCPParser
//
//  Created by James George on 3/17/13.
//
//

#include "CloudsStoryEngine.h"


CloudsStoryEngine::CloudsStoryEngine(){
	network = NULL;
	visualSystems = NULL;

	hasclip = false;
	isSetup = false;
	printDecisions = true;
	combinedClipsOnly = false;
	totalFramesWatched = 0;
	waitingForNextClip = false;
	watchingVisualSystem = false;
	
	fixedClipDelay = 5;
	maxTimesOnTopic = 4;
}

CloudsStoryEngine::~CloudsStoryEngine(){
	if(isSetup){
		ofRemoveListener(ofEvents().update, this, &CloudsStoryEngine::update);
	}
}

void CloudsStoryEngine::setup(){
	if(!isSetup){
		ofAddListener(ofEvents().update, this, &CloudsStoryEngine::update);
		isSetup = true;
	}
}

void CloudsStoryEngine::update(ofEventArgs& args){
	if(waitingForNextClip && nextClipTime < ofGetElapsedTimef()){
		playNextClip();
	}
}

void CloudsStoryEngine::seedWithClip(CloudsClip& seed){
	
	clipHistory.clear();
	topicHistory.clear();
	peopleVisited.clear();
	allNextClips.clear();
	
	hasclip = false;
	freeTopic = false;
	
	//freeTopic = true;
	currentTopic = seed.keywords[ ofRandom(seed.keywords.size()) ];
	//select a random topic from the clip
	
	CloudsStoryEventArgs args(seed,allNextClips,currentTopic);
	ofNotifyEvent(events.storyBegan,args);
		
	loadClip( seed );	
}

bool CloudsStoryEngine::playNextClip(){
	
	waitingForNextClip = false;
	
	//CHECK PRECONDITION
	if(network == NULL){
		ofLogError("Clouds Visualizer or Database is null! exiting select clip");
		return false;
	}
	
	//We need a clip to be able to select a new one.
	if(!hasclip){
		ofLogError("Cannot select new clip without a seed");
		return false;
	}
	
	if(atDeadEnd()){
		return false;
	}
	
	//Do a weighted selection based on the random value
	int randomClip = ofRandom( validNextClips.size() );
	cout << "SELECTED CLIP:" << randomClip << "/" << validNextClips.size();
	cout << " " << validNextClips[randomClip].getLinkName() << endl;
		
	loadClip( validNextClips[randomClip] );

	return true;
}

void CloudsStoryEngine::loadClip(CloudsClip& clip){

	hasclip = true;
	
	timesOnTopic++;
	if(timesOnTopic >= maxTimesOnTopic){
		freeTopic = true;
	}

	if(hasclip && (freeTopic || currentTopic == "") ){
		chooseNewTopic(clip);
	}

	checkVisualSystems();
		
	currentClip = clip;
	clipHistory.push_back( clip );
	peopleVisited[ clip.person ]++;
	
	populateNextClips();
	
	CloudsStoryEventArgs args(currentClip, allNextClips, currentTopic);
	ofNotifyEvent(events.clipBegan,args);
}

bool CloudsStoryEngine::clipEnded(){

	totalFramesWatched += (currentClip.endFrame - currentClip.startFrame);
	
	CloudsStoryEventArgs args(currentClip,allNextClips,currentTopic);
	args.timeUntilNextClip = getNextClipDelay();
	ofNotifyEvent(events.clipEnded, args, this);
	if(atDeadEnd()){
		ofNotifyEvent(events.storyEnded, args, this);
	}
	else{
		waitingForNextClip = true;
		nextClipTime = ofGetElapsedTimef() + args.timeUntilNextClip;
	}
}

void CloudsStoryEngine::chooseNewTopic(CloudsClip& upcomingClip){
	
	//TO CHOOSE A NEW TOPIC WE WANT TO FIND THE MOST "SIMILAR" TOPIC
	//this means the topic that shares the highest percent of clips with the current topic
	//This will prioritize
	vector<string> topics = network->getSharedKeywords(currentClip, upcomingClip);
	//vector<string> topics = upcomingClip.keywords;
	bool topicSwitched = false;

	if(topics.size() == 0){
		cout << "	TOPIC SWITCH: NO SHARED TOPICS" << endl;
		return;
	}

	cout << "Switching topic from " << currentTopic << " with " << topics.size() << " shared topics" << endl;
	string winningTopic = "";
	float highScore = 0;
	for(int i = 0; i < topics.size(); i++){
		
		if(ofContains(topicHistory, topics[i])){
			continue;
		}
		
		int sharedClips = network->getNumberOfSharedClips(currentTopic,topics[i]);
		int totalClips = network->getNumberOfClipsWithKeyword(topics[i]);
		float score = 1.0 * sharedClips / totalClips;
		cout << "TOPIC " << topics[i] << " SCORE " << score << " : " << sharedClips << "/" << totalClips << endl;
		if(highScore <= score){
			currentTopic = topics[i];
			highScore = score;
			topicSwitched = true;
		}
	}
	
	topicHistory.push_back(currentTopic);
	if(topicSwitched){
		timesOnTopic = 0;
		freeTopic = false;
		cout << "	TOPIC SWITCH TO " << currentTopic << endl;
		if(watchingVisualSystem){
			CloudsVisualSystemEventArgs args(currentVisualSystem);
			ofNotifyEvent(events.visualSystemEnded, args);
			watchingVisualSystem = false;
		}
		CloudsStoryEventArgs args(currentClip, allNextClips, currentTopic);
		ofNotifyEvent(events.topicChanged, args);

	}
	else{
		cout << "	FAILED TO SWITCH TOPIC " << currentTopic << endl;
	}
}

void CloudsStoryEngine::checkVisualSystems(){
	if(visualSystems == NULL){
		return;
	}
	
	if(timesOnTopic == 2){
		CloudsVisualSystemPreset preset = visualSystems->getRandomVisualSystem();
		CloudsVisualSystemEventArgs args(visualSystems->getRandomVisualSystem());
		ofNotifyEvent(events.visualSystemBegan, args);
		cout << "SHOW VISUAL SYSTEM!" << endl;
		watchingVisualSystem = true;
	}
	
	if(timesOnTopic == maxTimesOnTopic - 1){
		watchingVisualSystem = false;
		CloudsVisualSystemEventArgs args(currentVisualSystem);
		ofNotifyEvent(events.visualSystemEnded, args);
	}
}

bool CloudsStoryEngine::populateNextClips(){
	
	//get all the adjascent clips, assign weights to them and select
	vector<CloudsClip> nextClips;
	if(freeTopic){
		nextClips = network->getClipsWithKeyword(currentClip.keywords);
	}
	else{
		nextClips = network->getClipsWithKeyword(currentTopic);	
	}
	
	vector<CloudsLink>& links = network->getLinksForClip( currentClip );
	
	cout << "RELATED CLIPS TO: " << currentTopic << " " << nextClips.size() << " AND " << links.size() << " LINKS. ASSIGNING VALUES:" << endl;
	
	for(int i = 0; i < links.size(); i++){
		nextClips.push_back( network->getClipWithLinkName(links[i].targetName) );
	}
	
//	if(printDecisions){
//		cout << "REALTED CLIPS:" << endl;
//		for(int i = 0; i < related.size(); i++){
//			cout << "	\"" << related[i].getLinkName() << "\""
//				 << " Topics: [" << ofJoinString(related[i].keywords, ",") << "]" << endl;
//		}
//	}
	
	totalPoints = 0;

	validNextClips.clear();
	allNextClips.clear();

	int topScore = 0;
	for(int i = 0; i < nextClips.size(); i++){
		CloudsClip& m = nextClips[ i ];
		int score = scoreForClip( m );
		if(score != 0){
			totalPoints += score;
			if(score > topScore) topScore = score;
			m.currentScore = score;
			allNextClips.push_back(m);
		}
	}
	
	//if no clips were valid, return false
	if(totalPoints == 0) {
		cout << "DEAD END ENCOUNTRED, FREEING TOPIC" << endl;
		
		// if we are at a dead end because of the lack of clips,
		// recurse the call with an open topic
		// May still be dead from exhausted clips or topics on the same node
		
		if(!freeTopic){
			freeTopic = true;
			return populateNextClips();
		}
				
		ofLogError("Dead end found at clip " + currentClip.getLinkName());
		return false;
	}
	

	for(int i = 0; i < allNextClips.size(); i++){
		if(allNextClips[i].currentScore == topScore){
			validNextClips.push_back( allNextClips[i] );
		}
	}
	
	if(printDecisions){
		cout << "VALID CLIPS WITH SCORE " << topScore << endl;
		for(int i = 0; i < validNextClips.size(); i++){
			cout << "	" << validNextClips[i].getLinkName() << endl;
		}
	}
	
	return true;
}

int CloudsStoryEngine::scoreForClip(CloudsClip& clip){
	
	//rejection criteria -- flat out reject clips on some basis
	if(combinedClipsOnly && !clip.hasCombinedVideo){
		if(printDecisions) cout << "	REJECTED Clip " << clip.getLinkName() << ": no combined video file" << endl;
		return 0;
	}
    
    if(clip.cluster.Id == ""){
		if(printDecisions) cout << "	REJECTED Clip " << clip.getLinkName() << ": disconnect from cluster map =(" << endl;        
        return 0;
    }
    
	if(clip.person == currentClip.person){
		if(printDecisions) cout << "	REJECTED Clip " << clip.getLinkName() << ": same person" << endl;
		return 0;
	}
	
	bool containsCurrentTopic = ofContains(clip.keywords, currentTopic);
	if(!freeTopic && !containsCurrentTopic){
		if(printDecisions) cout << "	REJECTED Clip " << clip.getLinkName() << ": not on topic " << currentTopic << endl;
		return 0;
	}
	//maybe a bug... 
	else if(freeTopic && containsCurrentTopic){
		if(printDecisions) cout << "	REJECTED Clip " << clip.getLinkName() << ": same topic as before " << currentTopic << endl;
		return 0;
	}

	//reject any nodes we've seen already
	if(historyContainsClip(clip)){
		if(printDecisions) cout << "	REJECTED Clip " << clip.getLinkName() << ": already visited" << endl;
		return 0;
	}
		
	int occurrences = occurrencesOfPerson(clip.person, 20);
	if(occurrences > 4){
		if(printDecisions) cout << "	REJECTED Clip " << clip.getLinkName() << ": person appeared more than 4 times in the last 20 clips" << endl;
		return 0;
	}
	
	if(network->linkIsSuppressed(currentClip.getLinkName(), clip.getLinkName())) {
		if(printDecisions) cout << "	REJECTED clip " << clip.getLinkName() << ": link is suppressed" << endl;
		return 0;
	}
	
	//Base score
	int score = 10;
	int topicsInCommon = network->getSharedKeywords(currentClip, clip).size();
	score += (topicsInCommon-1)*10;

	bool link = network->clipLinksTo( currentClip.getLinkName(), clip.getLinkName() );
	//If this clip is a link weight it highly
	if( link ){
		score += 20;
	}

	//penalize for the person occurring
	score -= occurrences*4;
	
	if(printDecisions) cout << "	ACCEPTED " << (link ? "LINK " : "") << score << " Clip " << clip.getLinkName() << " occurrences " << occurrences << " and " << topicsInCommon << " topics in common" << endl;
	
	return MAX(score, 0);
}

void CloudsStoryEngine::drawStoryEngineDebug(){
	ofPushStyle();
	
	ofSetColor(255);
	string debugString = "";
	debugString += "Current Clip:    " + currentClip.getLinkName() + "\n";
	debugString += "Current Topic:   " + getCurrentTopic() + "\n";
	debugString += "Times on Topic:  " + ofToString(timesOnTopic) + "/" + ofToString(maxTimesOnTopic) + "\n";
	
	ofDrawBitmapString(debugString,25,25);
	ofPopStyle();
}

float CloudsStoryEngine::getTotalSecondsWatched(){
	return totalFramesWatched / 23.976;
}

CloudsClip& CloudsStoryEngine::getCurrentClip(){
	return currentClip;
}

vector<CloudsClip>& CloudsStoryEngine::getClipHistory(){
	return clipHistory;
}

string CloudsStoryEngine::getCurrentTopic(){
	return currentTopic;
}

int CloudsStoryEngine::getTimesOnTopic(){
	return timesOnTopic;
}

float CloudsStoryEngine::getNextClipDelay(){
	return fixedClipDelay;
}

bool CloudsStoryEngine::isWaiting(){
	return waitingForNextClip;
}

bool CloudsStoryEngine::atDeadEnd(){
	return validNextClips.size() == 0;
}

bool CloudsStoryEngine::historyContainsClip(CloudsClip& m){
	string clipLinkName = m.getLinkName();
	for(int i = 0; i < clipHistory.size(); i++){
		if(clipLinkName == clipHistory[i].getLinkName()){
			return true;
		}
	}
	return false;
}

int CloudsStoryEngine::occurrencesOfPerson(string person, int stepsBack){
	int occurrences = 0;
	int startPoint = clipHistory.size() - MIN(stepsBack, clipHistory.size() );
//	cout << "finding occurrences ... " << person << " " << clipHistory.size() << " steps back " << stepsBack << " start point " << startPoint << "/" << clipHistory.size() << endl;
	for(int i = startPoint; i < clipHistory.size(); i++){
//		cout << "COMPARING " << clipHistory[i].person << " to " << person << endl;
		if(clipHistory[i].person == person){
			occurrences++;
		}
	}
	return occurrences;
}

CloudsEvents& CloudsStoryEngine::getEvents(){
	return events;
}
