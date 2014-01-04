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
	previousState = TRANSITION_INTERVIEW_IDLE;
	currentState = TRANSITION_INTERVIEW_IDLE;
}

void CloudsTransitionController::transitionFromIntro(float outDuration, float inDuration){
	
	currentState = TRANSITION_INTRO_OUT;
	
	transitioning = true;
	triggeredMidpoint = false;
	newState = true;
	
	transitionStartTime = ofGetElapsedTimef();
	transitionOutCompleteTime = transitionStartTime + outDuration;
	transitionInCompleteTime  = transitionStartTime + outDuration + inDuration;
}

void CloudsTransitionController::transitionToVisualSystem(float outDuration, float inDuration){

	currentState = TRANSITION_INTERVIEW_OUT;

	transitioning = true;
	triggeredMidpoint = false;
	newState = true;
	
	transitionStartTime = ofGetElapsedTimef();
	transitionOutCompleteTime = transitionStartTime + outDuration;
	transitionInCompleteTime  = transitionStartTime + outDuration + inDuration;
}

void CloudsTransitionController::transitionToInterview(float outDuration, float inDuration){

	currentState = TRANSITION_VISUALSYSTEM_OUT;

	transitioning = true;
	triggeredMidpoint = false;
	newState = true;
	
	transitionStartTime = ofGetElapsedTimef();
	transitionOutCompleteTime = transitionStartTime + outDuration;
	transitionInCompleteTime  = transitionStartTime + outDuration + inDuration;
}

float CloudsTransitionController::getFadeValue(){
	if(transitioning){
		if(fadingOut()){
			return 1. - percentTransitionOut;
		}
		else{
			return percentTransitionIn;
		}
	}
	return 1.0;
}

float CloudsTransitionController::getInterviewTransitionPoint(){
	if(currentState == TRANSITION_INTERVIEW_OUT){
		return percentTransitionOut;
	}
	else if(currentState == TRANSITION_INTERVIEW_IN){
		return percentTransitionIn;
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
		
		percentTransitionOut = ofMap(ofGetElapsedTimef(), transitionStartTime, transitionOutCompleteTime, 0.0, 1.0, true);
		percentTransitionIn  = ofMap(ofGetElapsedTimef(), transitionOutCompleteTime, transitionInCompleteTime, 0.0, 1.0, true);
		
//		cout << "	OUT PERCENT " << percentTransitionOut << " IN PERCENT " << percentTransitionIn << endl;
		
		if(!triggeredMidpoint && percentTransitionOut >= 1.0){
			//TRIGGER MIDPOINT
			newState = true;
			triggeredMidpoint = true;
			getNextState();
		}
		if(percentTransitionIn >= 1.0){
			//TRIGGER FINISHED
			newState = true;
			transitioning = false;
			getNextState();
		}
	}
}

CloudsTransitionState CloudsTransitionController::getCurrentState(){
	return currentState;
}
CloudsTransitionState CloudsTransitionController::getPreviousState(){
	return previousState;
}

string CloudsTransitionController::getCurrentStateDescription(){
	switch(currentState){
		case TRANSITION_INTERVIEW_IDLE:
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

bool CloudsTransitionController::isStateNew(){
	bool s = newState;
	newState = false;
	return s;
}

bool CloudsTransitionController::fadingOut(){
	return currentState == TRANSITION_VISUALSYSTEM_OUT ||
		   currentState == TRANSITION_INTERVIEW_OUT ||
		   currentState == TRANSITION_INTRO_OUT;
}

//move to the next state
CloudsTransitionState CloudsTransitionController::getNextState(){
	previousState = currentState;
	switch(currentState){
		case TRANSITION_INTERVIEW_IDLE:
			break;
		case TRANSITION_INTERVIEW_OUT:
			currentState = TRANSITION_VISUALSYSTEM_IN;
			break;
		case TRANSITION_VISUALSYSTEM_IN:
			currentState = TRANSITION_INTERVIEW_IDLE;
			break;
		case TRANSITION_VISUALSYSTEM_OUT:
			currentState = TRANSITION_INTERVIEW_IN;
			break;
		case TRANSITION_INTERVIEW_IN:
			currentState = TRANSITION_INTERVIEW_IDLE;
			break;
		case TRANSITION_INTRO_OUT:
			currentState = TRANSITION_INTERVIEW_IDLE; //This is where a TUNNEL will go - maybe make a loading screen here too?
			break;
	}

}
