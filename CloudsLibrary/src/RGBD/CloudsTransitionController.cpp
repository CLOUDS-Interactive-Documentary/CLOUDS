//
//  CloudsTransitionController.cpp
//  CLOUDS
//
//  Created by James George on 1/3/14.
//
//

#include "CloudsTransitionController.h"


//void CloudsTransitionController::addControllerTween( string name, float startTime, float span, float startVal, float endVal, float *value ){
//	
//	CloudsPlaybackControllerTween t;
//	t.setup( name, startTime, span, startVal, endVal, value );
//	
//	controllerTweens.push_back( t );
//}
//

//void CloudsTransitionController::setup( string _name, float _startTime, float _span, float _startVal, float _endVal, float *_value){
//	name = _name;
//	
//	startTime = _startTime;
//	span = _span;
//	endTime = startTime + span;
//	
//	startVal = _startVal;
//	endVal = _endVal;
//	
//	value = _value;
//	
//	bStarted = false;
//	bEnded = false;
//	autoDelete = true;
//}

CloudsTransitionController::CloudsTransitionController(){
	transitioning = false;
	triggeredMidpoint = false;
	newState = false;
}

void CloudsTransitionController::transitionToVisualSystem(float outDuration, float inDuration){
	
	transitioning = true;
	triggeredMidpoint = false;
	currentState = TRANSITION_INTERVIEW_OUT;
	newState = true;
	
	transitionStartTime = ofGetElapsedTimef();
	transitionOutCompleteTime = transitionStartTime + outDuration;
	transitionInCompleteTime  = transitionStartTime + outDuration + inDuration;
}

void CloudsTransitionController::transitionToInterview(float outDuration, float inDuration){
	
	transitioning = true;
	triggeredMidpoint = false;
	currentState = TRANSITION_VISUALSYSTEM_OUT;
	newState = true;
	
	transitionStartTime = ofGetElapsedTimef();
	transitionOutCompleteTime = transitionStartTime + outDuration;
	transitionInCompleteTime  = transitionStartTime + outDuration + inDuration;
}

float CloudsTransitionController::getFadeValue(){
	if(fadingOut()){
		return 1. - percentTransitionOut;
	}
	else{
		return percentTransitionIn;
	}
}

void CloudsTransitionController::update() {
	if(transitioning){
		percentTransitionOut = ofMap(ofGetElapsedTimef(), transitionStartTime, transitionOutCompleteTime, 0, 1.0, true);
		percentTransitionIn  = ofMap(ofGetElapsedTimef(), transitionOutCompleteTime, transitionInCompleteTime, 0, 1.0, true);
		if(!triggeredMidpoint && percentTransitionIn >= 1.0){
			//TRIGGER MIDPOINT
			newState = true;
			triggeredMidpoint = true;
		}
		if(percentTransitionOut >= 0){
			//TRIGGER FINISHED
			newState = true;
			transitioning = false;
		}
	}
}

CloudsTransitionState CloudsTransitionController::getCurrentState(){
	return currentState;
}

bool CloudsTransitionController::isStateNew(){
	bool s = newState;
	newState = false;
	return s;
}

bool CloudsTransitionController::fadingOut(){
	return currentState == TRANSITION_VISUALSYSTEM_OUT || currentState == TRANSITION_VISUALSYSTEM_OUT;
}

//move to the next state
CloudsTransitionState CloudsTransitionController::getNextState(){
	return CloudsTransitionState( (int(currentState) + 1) % 4);
}
