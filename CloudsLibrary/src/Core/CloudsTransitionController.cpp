//
//  CloudsTransitionController.cpp
//  CLOUDS
//
//  Created by James George on 1/3/14.
//
//

#include "CloudsTransitionController.h"

CloudsTransitionController::CloudsTransitionController(){
	
	transitioning = false;
	triggeredMidpoint = false;
	newState = false;
    
	previousState = TRANSITION_INTRO_IN;
	currentState = TRANSITION_IDLE;
	
	fadeOutStates.push_back(TRANSITION_INTERVIEW_OUT);
	fadeOutStates.push_back(TRANSITION_VISUALSYSTEM_OUT);
	fadeOutStates.push_back(TRANSITION_INTRO_OUT);
    fadeOutStates.push_back(TRANSITION_INTERLUDE_OUT);
	fadeOutStates.push_back(TRANSITION_QUESTION_OUT);
	fadeOutStates.push_back(TRANSITION_CLUSTERMAP_OUT);
	fadeOutStates.push_back(TRANSITION_CLUSTERMAP_OUT);
	fadeOutStates.push_back(TRANSITION_EXPLORE_MAP_OUT);
	fadeOutStates.push_back(TRANSITION_EXPLORE_PEOPLE_OUT);
	fadeOutStates.push_back(TRANSITION_EXPLORE_VISUALS_OUT);
	
	fadeInStates.push_back(TRANSITION_VISUALSYSTEM_IN);
	fadeInStates.push_back(TRANSITION_INTERVIEW_IN);
    fadeInStates.push_back(TRANSITION_INTERLUDE_IN);
	fadeInStates.push_back(TRANSITION_QUESTION_IN);
	fadeInStates.push_back(TRANSITION_INTRO_IN);
	fadeInStates.push_back(TRANSITION_CLUSTERMAP_IN);
	fadeInStates.push_back(TRANSITION_EXPLORE_MAP_IN);
	fadeInStates.push_back(TRANSITION_EXPLORE_PEOPLE_IN);
	fadeInStates.push_back(TRANSITION_EXPLORE_VISUALS_IN);
    
    
	reciprocalTransitions[TRANSITION_INTRO_IN] = TRANSITION_INTRO_OUT;
	reciprocalTransitions[TRANSITION_INTRO_OUT] = TRANSITION_INTRO_IN;
	reciprocalTransitions[TRANSITION_INTERVIEW_IN] =TRANSITION_INTERVIEW_OUT;
	reciprocalTransitions[TRANSITION_INTERVIEW_OUT] =TRANSITION_INTERVIEW_IN;
	reciprocalTransitions[TRANSITION_VISUALSYSTEM_IN] = TRANSITION_VISUALSYSTEM_OUT;
	reciprocalTransitions[TRANSITION_VISUALSYSTEM_OUT] = TRANSITION_VISUALSYSTEM_IN;
 	reciprocalTransitions[TRANSITION_QUESTION_IN] = TRANSITION_QUESTION_OUT;
	reciprocalTransitions[TRANSITION_QUESTION_OUT] = TRANSITION_QUESTION_IN;
    reciprocalTransitions[TRANSITION_INTERLUDE_IN] = TRANSITION_INTERLUDE_OUT;
    reciprocalTransitions[TRANSITION_INTERLUDE_OUT] = TRANSITION_INTERLUDE_IN;
	reciprocalTransitions[TRANSITION_CLUSTERMAP_IN] = TRANSITION_CLUSTERMAP_OUT;
	reciprocalTransitions[TRANSITION_CLUSTERMAP_OUT] = TRANSITION_CLUSTERMAP_IN;
	reciprocalTransitions[TRANSITION_EXPLORE_MAP_IN] = TRANSITION_EXPLORE_MAP_OUT;
	reciprocalTransitions[TRANSITION_EXPLORE_MAP_OUT] = TRANSITION_EXPLORE_MAP_IN;
	reciprocalTransitions[TRANSITION_EXPLORE_PEOPLE_IN] = TRANSITION_EXPLORE_PEOPLE_OUT;
	reciprocalTransitions[TRANSITION_EXPLORE_PEOPLE_OUT] = TRANSITION_EXPLORE_PEOPLE_IN;
	reciprocalTransitions[TRANSITION_EXPLORE_VISUALS_IN] = TRANSITION_EXPLORE_VISUALS_OUT;
	reciprocalTransitions[TRANSITION_EXPLORE_VISUALS_OUT] = TRANSITION_EXPLORE_VISUALS_IN;
    //FAIL safe
    reciprocalTransitions[TRANSITION_IDLE] = TRANSITION_IDLE;

}

void CloudsTransitionController::confirmEmpty(string transitionName){
	if(currentState != TRANSITION_IDLE){
		//PRINT ALL HISTORY
		//for(int i = 0; i < queueHistory.size(); i++){
		//	ofLogError("TRANSITION QUEUE HISTORY") << getStateDescription(queueHistory[i].state) << " : " << queueHistory[i].timeRange;
		//}
		//////////////////

		ofLogError("CloudsTransitionController::confirmEmpty") << "Current state " << getStateDescription(currentState) << " is not IDLE on new transition " << transitionName;
		for(int i = 0; i < stateQueue.size(); i++){
			ofLogError("TRANSITION QUEUE REMAINING") << getStateDescription(stateQueue[i].state);
		}
	}
	if(!stateQueue.empty()){
		ofLogError("CloudsTransitionController::confirmEmpty") << "State Queue is not empty";
		stateQueue.clear();
	}
}

void CloudsTransitionController::transitionFromIntro(float outDuration){

	confirmEmpty("transitionFromIntro");

	queueState(TRANSITION_INTRO_OUT, outDuration);
	
	startTransition();

}

void CloudsTransitionController::transitionToClusterMap(float transitionDuration){
	
	confirmEmpty("transitionToClusterMap");
    
	queueState(TRANSITION_CLUSTERMAP_IN, transitionDuration);
	
	startTransition();
}

void CloudsTransitionController::transitionFromClusterMap(float transitionDuration){
	
	confirmEmpty("transitionFromClusterMap");
	
	queueState(TRANSITION_CLUSTERMAP_OUT, transitionDuration);
	
	startTransition();
}

void CloudsTransitionController::transitionToIntro(float inDuration){
	
	confirmEmpty("transitionToIntro");

//    if(getPreviousState() == TRANSITION_INTERLUDE_IN){
//        queueState(TRANSITION_INTERLUDE_OUT, inDuration);
//    }
//    else if(getPreviousState() == TRANSITION_INTERVIEW_IN){
//        queueState(TRANSITION_INTERVIEW_OUT, inDuration);
//    }
//    else if(getPreviousState() == TRANSITION_VISUALSYSTEM_IN){
//        queueState(TRANSITION_VISUALSYSTEM_OUT, inDuration);
//    }
    queueReciprocal(getPreviousState(), inDuration);
    
	queueState(TRANSITION_INTRO_IN, inDuration);
	
	startTransition();
}


void CloudsTransitionController::transitionToFirstVisualSystem(float duration){

	confirmEmpty("transitionToFirstVisualSystem");

	queueState(TRANSITION_VISUALSYSTEM_IN, duration);

	startTransition();
}

void CloudsTransitionController::transitionToFirstInterview(float duration){

	confirmEmpty("transitionToFirstInterview");
    
	queueState(TRANSITION_INTERVIEW_IN, duration);
    
	startTransition();
  
}

void CloudsTransitionController::transitionToVisualSystem(float outDuration, float inDuration){

	confirmEmpty("transitionToVisualSystem");
	
//	cout << "TRANSITION POINTCLOUD --> VISUAL SYSTEM" << endl;
	
	queueState(TRANSITION_INTERVIEW_OUT, outDuration);
	queueState(TRANSITION_VISUALSYSTEM_IN, inDuration);

	startTransition();
}

void CloudsTransitionController::transitionToInterview(float outDuration, float inDuration){

	confirmEmpty("transitionToInterview");
	
//	cout << "TRANSITION VISUAL SYSTEM --> POINTCLOUD" << endl;
	
	queueState(TRANSITION_VISUALSYSTEM_OUT, outDuration);
	queueState(TRANSITION_INTERVIEW_IN, inDuration);
	
	startTransition();
	
}

void CloudsTransitionController::transitionToInterlude(float inDuration,float outDuration){
	
	confirmEmpty("transitionToInterlude");
	
//	if(getPreviousState() == TRANSITION_VISUALSYSTEM_IN){
////		currentState = TRANSITION_VISUALSYSTEM_IN; //JG why is this here
//		queueState(TRANSITION_VISUALSYSTEM_OUT, outDuration);
//	}
//	//we are in an interview
//	else if(getPreviousState() == TRANSITION_INTERVIEW_IN){
//		queueState(TRANSITION_INTERVIEW_OUT, outDuration);
//	}
    
    queueReciprocal(getPreviousState(), outDuration);
	
    queueState(TRANSITION_INTERLUDE_IN, inDuration);
    
	startTransition();
}

void CloudsTransitionController::transitionFromInterlude(float inDuration){
    
    confirmEmpty("transitionFromInterlude");
    
    queueState(TRANSITION_INTERLUDE_OUT, inDuration);
	
    startTransition();
    
}

void CloudsTransitionController::transitionWithQuestion(float outDuration, float portalDuration){
	
	confirmEmpty("transitionWithQuestion");
	
	queueState(TRANSITION_INTERVIEW_OUT, outDuration);
	
	startTransition();
}

void CloudsTransitionController::transitionToExploreMap(float inDuration,float outDuration){
	
	confirmEmpty("transitionToExploreMap");
	
//	//we are in a visual system
//	if(getPreviousState() == TRANSITION_VISUALSYSTEM_IN){
//		queueState(TRANSITION_VISUALSYSTEM_OUT, outDuration);
//	}
//	//we are in an interview
//	else if(getPreviousState() == TRANSITION_INTERVIEW_IN){
//		queueState(TRANSITION_INTERVIEW_OUT, outDuration);
//	}
//	else if(getPreviousState() == TRANSITION_INTRO_IN){
//        queueState(TRANSITION_INTRO_OUT, outDuration);
//    }
    
    queueReciprocal(getPreviousState(), outDuration);
        
    queueState(TRANSITION_EXPLORE_MAP_IN, inDuration);
	
	startTransition();
}

void CloudsTransitionController::transitionFromExploreMap(float inDuration){

    confirmEmpty("transitionFromExploreMap");
    
    queueState(TRANSITION_EXPLORE_MAP_OUT, inDuration);
	
    startTransition();
    
}


void CloudsTransitionController::transitionToExplorePeople(float inDuration, float outDuration){

    confirmEmpty("transitionToExplorePeople");
    
    queueReciprocal(getPreviousState(), outDuration);
    
    queueState(TRANSITION_EXPLORE_PEOPLE_IN, inDuration);
	
	startTransition();
}

void CloudsTransitionController::transitionFromExplorePeople(float inDuration){
    
    confirmEmpty("transitionFromExplorePeople");

    queueState(TRANSITION_EXPLORE_PEOPLE_OUT, inDuration);
	
    startTransition();
}

void CloudsTransitionController::queueReciprocal(CloudsTransitionState state, float duration){
    queueState(reciprocalTransitions[state], duration);
}

void CloudsTransitionController::startTransition(){
	transitioning = true;
	getNextState();
    update();
}

float CloudsTransitionController::getFadeValue(){
	if(transitioning){
		if(fadingOut()){
			return 1.0 - transitionPercent;
		}
		else{
			return transitionPercent;
		}
	}

	return fadedOut() ? 0.0 : 1.0;
}

float CloudsTransitionController::getInterviewTransitionPoint(){
	if(currentState == TRANSITION_INTERVIEW_OUT){
		return transitionPercent;
	}
	else if(currentState == TRANSITION_INTERVIEW_IN){
		return transitionPercent;
	}
	else if(currentState == TRANSITION_VISUALSYSTEM_IN){
		return 1.0;
	}
	else if(currentState == TRANSITION_VISUALSYSTEM_OUT){
		return 0.0;
	}
	return 0.0;
}

void CloudsTransitionController::update() {
	if(transitioning){
		
		transitionPercent = ofMap(ofGetElapsedTimef(), currentQueue.startTime, currentQueue.endTime, 0.0, 1.0, true);
		
		//started next state
		if(transitionPercent >= 1.0){
            transitionPercent = 0.0;
			getNextState();
		}
	}
}

bool CloudsTransitionController::isTransitioning(){
    return transitioning;
}

void CloudsTransitionController::queueState(CloudsTransitionState state, float transitionDuration){
	CloudsTransitionQueueEntry cue;
	if(stateQueue.empty()){
		cue.startTime = ofGetElapsedTimef();
	}
	else{
		cue.startTime = stateQueue.back().endTime;
	}
	cue.endTime = cue.startTime + transitionDuration;
	cue.state = state;
	
	stateQueue.push_back(cue);
}

CloudsTransitionState CloudsTransitionController::getCurrentState(){
	return currentState;
}

CloudsTransitionState CloudsTransitionController::getPreviousState(){
	return previousState;
}

CloudsTransitionState CloudsTransitionController::getPendingState(){
	return pendingState;
}

string CloudsTransitionController::getPendingStateDescription(){
    return getStateDescription(pendingState);
}

string CloudsTransitionController::getCurrentStateDescription(){
	return getStateDescription(currentState);
}

string CloudsTransitionController::getPreviousStateDescription(){
	return getStateDescription(previousState);
}

bool CloudsTransitionController::isStateNew(){
	bool s = newState;
	newState = false;
	return s;
}

bool CloudsTransitionController::fadingOut(){
	return ofContains(fadeOutStates, currentState);
}

bool CloudsTransitionController::fadedOut(){
	return ofContains(fadeOutStates, previousState);
}

//move to the next state
CloudsTransitionState CloudsTransitionController::getNextState(){

	//push back the current time
	currentQueue.timeRange.max = ofGetElapsedTimef();
	queueHistory.push_back(currentQueue);
	previousState = currentState;

	if(stateQueue.empty()){
		currentQueue.state = TRANSITION_IDLE;
		currentState = TRANSITION_IDLE;
        pendingState = TRANSITION_IDLE;
		transitioning = false;
	}
	else{
		currentQueue = stateQueue.front();
		currentState = stateQueue.front().state;
        pendingState = stateQueue.back().state;
		stateQueue.pop_front();
	}

	//the next one
	currentQueue.timeRange.min = ofGetElapsedTimef();

	newState = true;
	return currentState;
}

string CloudsTransitionController::getStateDescription(CloudsTransitionState state){
	switch(state){
        case TRANSITION_IDLE:
			return "TransitionIdle";
        case TRANSITION_INTRO_IN:
			return "TransitionIntroIn";
        case TRANSITION_INTRO_OUT:
			return "TransitionIntroOut";
        case TRANSITION_INTERVIEW_IN:
			return "TransitionInterviewIn";
        case TRANSITION_INTERVIEW_OUT:
			return "TransitionInterviewOut";
        case TRANSITION_VISUALSYSTEM_IN:
			return "TransitionVisualSystemIn";
        case TRANSITION_VISUALSYSTEM_OUT:
			return "TransitionVisualSystemOut";
        case TRANSITION_QUESTION_IN:
			return "TransitionQuestionIn";
        case TRANSITION_QUESTION_OUT:
			return "TransitionQuestionOut";
        case TRANSITION_INTERLUDE_IN:
			return "TransitionInterludeIn";
        case TRANSITION_INTERLUDE_OUT:
			return "TransitionInterludeOut";
		case TRANSITION_CLUSTERMAP_IN:
			return "TransitionClusterMapIn";
		case TRANSITION_CLUSTERMAP_OUT:
			return "TransitionClusterMapOut";
        case TRANSITION_EXPLORE_MAP_IN:
			return "TransitionExploreMapIn";
        case TRANSITION_EXPLORE_MAP_OUT:
			return "TransitionExploreMapOut";
        case TRANSITION_EXPLORE_PEOPLE_IN:
			return "TransitionExplorePeopleIn";
        case TRANSITION_EXPLORE_PEOPLE_OUT:
			return "TransitionExplorePeopleOut";
        case TRANSITION_EXPLORE_VISUALS_IN:
			return "TransitionExploreVisualsIn";
        case TRANSITION_EXPLORE_VISUALS_OUT:
			return "TransitionExploreVisualsOut";
		default:
			return "UNKNOWN STATE " + ofToString(int(currentState));
	}
	return "UNKNOWN STATE " + ofToString(int(currentState));

}
