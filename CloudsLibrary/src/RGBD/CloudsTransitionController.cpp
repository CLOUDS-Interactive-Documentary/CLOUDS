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
	previousState = TRANSITION_IDLE;
	currentState = TRANSITION_IDLE;
	
	fadeOutStates.push_back(TRANSITION_INTERVIEW_OUT);
	fadeOutStates.push_back(TRANSITION_VISUALSYSTEM_OUT);
	fadeOutStates.push_back(TRANSITION_INTRO_OUT);
	
	fadeInStates.push_back(TRANSITION_VISUALSYSTEM_IN);
	fadeInStates.push_back(TRANSITION_INTERVIEW_IN);
}

void CloudsTransitionController::confirmEmpty(){
	if(currentState != TRANSITION_IDLE){
		for(int i = 0; i < queueHistory.size(); i++){
			ofLogError("TRANSITION QUEUE HISTORY") << getStateDescription(queueHistory[i].state) << " : " << queueHistory[i].timeRange;
		}
		ofLogError("CloudsTransitionController::confirmEmpty") << "Current state is not IDLE on new transition: " << getStateDescription(currentState);
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

	confirmEmpty();

	queueState(TRANSITION_INTRO_OUT, outDuration);
	
	startTransition();

}

void CloudsTransitionController::transitionToFirstVisualSystem(float duration){

	confirmEmpty();
	
	queueState(TRANSITION_VISUALSYSTEM_IN, duration);

	startTransition();
}

void CloudsTransitionController::transitionToVisualSystem(float outDuration, float inDuration){

	confirmEmpty();
	
	cout << "TRANSITION POINTCLOUD --> VISUAL SYSTEM" << endl;
	
	queueState(TRANSITION_INTERVIEW_OUT, outDuration);
	queueState(TRANSITION_VISUALSYSTEM_IN, inDuration);

	startTransition();
}

void CloudsTransitionController::transitionToInterview(float outDuration, float inDuration){

	confirmEmpty();
	
	cout << "TRANSITION VISUAL SYSTEM --> POINTCLOUD" << endl;
	
	queueState(TRANSITION_VISUALSYSTEM_OUT, outDuration);
	queueState(TRANSITION_INTERVIEW_IN, inDuration);
	
	startTransition();
	
}

void CloudsTransitionController::transitionToClusterMap(float outDuration, float inDuration){
	
	confirmEmpty();
	
	//we are in a visual system
	if(getPreviousState() == TRANSITION_VISUALSYSTEM_IN){
		currentState = TRANSITION_VISUALSYSTEM_IN;
		queueState(TRANSITION_VISUALSYSTEM_OUT, outDuration);
	}
	//we are in an interview
	else if(getPreviousState() == TRANSITION_INTERVIEW_IN){
		queueState(TRANSITION_INTERVIEW_OUT, outDuration);
	}
	
	startTransition();
}

void CloudsTransitionController::transitionFromClusterMap(float transitionOutDuration, float transitionInDuration){
	//TODO: !!
}

void CloudsTransitionController::startTransition(){
	transitioning = true;
	getNextState();
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
			getNextState();
		}
	}
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

string CloudsTransitionController::getCurrentStateDescription(){
	return getStateDescription(currentState);
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
		transitioning = false;
	}
	else{
		currentQueue = stateQueue.front();
		currentState = stateQueue.front().state;
		stateQueue.pop_front();
	}

	//the next one
	currentQueue.timeRange.min = ofGetElapsedTimef();

	newState = true;
}

string CloudsTransitionController::getStateDescription(CloudsTransitionState state){
	switch(state){
		case TRANSITION_IDLE:
			return "TransitionIdle";
		case TRANSITION_INTERVIEW_OUT:
			return "TransitionInterviewOut";
		case TRANSITION_VISUALSYSTEM_IN:
			return "TransitionVisualSystemIn";
		case TRANSITION_VISUALSYSTEM_OUT:
			return "TransitionVisualSystemOut";
		case TRANSITION_INTERVIEW_IN:
			return "TransitionInterviewIn";
		case TRANSITION_INTRO_OUT:
			return "TransitionIntroOut";
			
		default:
			return "UNKNOWN STATE " + ofToString(int(currentState));
	}
}
