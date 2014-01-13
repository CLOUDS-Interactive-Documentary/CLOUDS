//
//  CloudsTransitionController.h
//  CLOUDS
//
//  Created by James George on 1/3/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofRange.h"

typedef enum {
	TRANSITION_IDLE = 0,
	TRANSITION_INTERVIEW_OUT = 1,
	TRANSITION_VISUALSYSTEM_IN = 2,
	TRANSITION_VISUALSYSTEM_OUT = 3,
	TRANSITION_INTERVIEW_IN = 4,
	TRANSITION_INTRO_OUT = 5,
    TRANSITION_CLUSTERMAP_IN = 7,
    TRANSITION_CLUSTERMAP_OUT = 8,
	TRANSITION_QUESTION_IN = 9,
	TRANSITION_QUESTION_OUT = 10,
	TRANSITION_INTRO_IN = 11
} CloudsTransitionState;

typedef struct {
	float startTime;
	float endTime;
	ofRange timeRange; //set when the queue is done, used to DEBUG
	CloudsTransitionState state;
} CloudsTransitionQueueEntry;

class CloudsTransitionController {
  public:
	CloudsTransitionController();
	
	void transitionFromIntro(float transitionOutDuration);
    void transitionToIntro(float inDuration);    
    void transitionToFirstInterview(float transitionDuration);
	void transitionToFirstVisualSystem(float transitionOutDuration);
	void transitionToVisualSystem(float transitionOutDuration, float transitionInDuration);
	void transitionToInterview(float transitionOutDuration, float transitionInDuration);
	void transitionToClusterMap(float inDuration,float outDuration);
	void transitionFromClusterMap(float inDuration);
	void transitionWithQuestion(float outDuration, float portalDuration);

	
	void update();
	
	float transitionPercent;

	float getInterviewTransitionPoint();
	
	bool transitioning;
	bool triggeredMidpoint;
	
	bool fadingOut(); ///are we fading out?
	bool fadedOut(); //did we just fade out?
	//returns 0 - 1.0 for use in alpha on the visual system texture
	//ramps down when fading out, then up when fading in
	float getFadeValue();
	
	bool isStateNew();
	CloudsTransitionState getCurrentState();
	CloudsTransitionState getPreviousState();
	string getCurrentStateDescription();
	string getStateDescription(CloudsTransitionState state);
	
  protected:
	
	deque<CloudsTransitionQueueEntry> stateQueue;
	vector<CloudsTransitionQueueEntry> queueHistory;
	
	void queueState(CloudsTransitionState state, float transitionDuration);
	CloudsTransitionQueueEntry currentQueue;
	void startTransition();
	
	vector<CloudsTransitionState> fadeOutStates;
	vector<CloudsTransitionState> fadeInStates;
	
	CloudsTransitionState currentState;
	CloudsTransitionState previousState;
	float transitionStartTime;
	
	float transitionInCompleteTime;
	float transitionOutCompleteTime;
	void confirmEmpty();
	
	bool newState;
	CloudsTransitionState getNextState();
	
};
