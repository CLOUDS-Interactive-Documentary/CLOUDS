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
	TRANSITION_IDLE             = 0,
	TRANSITION_INTRO_IN         = 1,
	TRANSITION_INTRO_OUT        = 2,
	TRANSITION_INTERVIEW_IN     = 3,
	TRANSITION_INTERVIEW_OUT    = 4,
	TRANSITION_VISUALSYSTEM_IN  = 5,
	TRANSITION_VISUALSYSTEM_OUT = 6,
	TRANSITION_QUESTION_IN      = 7,
	TRANSITION_QUESTION_OUT     = 8,
    TRANSITION_INTERLUDE_IN     = 9,
    TRANSITION_INTERLUDE_OUT    = 10,
	TRANSITION_CLUSTERMAP_IN    = 11,
	TRANSITION_CLUSTERMAP_OUT   = 12,
	TRANSITION_EXPLORE_MAP_IN   = 13,
	TRANSITION_EXPLORE_MAP_OUT  = 14,
	TRANSITION_EXPLORE_PEOPLE_IN    = 15,
	TRANSITION_EXPLORE_PEOPLE_OUT   = 16,
	TRANSITION_EXPLORE_VISUALS_IN   = 17,
	TRANSITION_EXPLORE_VISUALS_OUT  = 18
    
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
	
    void transitionToIntro(float inDuration);
	void transitionFromIntro(float transitionOutDuration);
    
	void transitionToClusterMap(float transitionDuration);
	void transitionFromClusterMap(float transitionDuration);
	
    void transitionToFirstInterview(float transitionDuration);
	void transitionToFirstVisualSystem(float transitionOutDuration);
    
	void transitionToVisualSystem(float transitionOutDuration, float transitionInDuration);
	void transitionToInterview(float transitionOutDuration, float transitionInDuration);
    
	void transitionToInterlude(float inDuration, float outDuration);
	void transitionFromInterlude(float outDuration);
    
	void transitionWithQuestion(float outDuration, float portalDuration);

   	void transitionToExploreMap(float inDuration, float outDuration);
   	void transitionFromExploreMap(float inDuration);

   	void transitionToExplorePeople(float inDuration, float outDuration);
   	void transitionFromExplorePeople(float inDuration);
    
   	void transitionToExploreVisuals(float inDuration, float outDuration);
    void transitionFromExploreVisuals(float inDuration);
    
    void transitionBackToAct(float inDuration, float outDuration);
    
	void update();
	
    bool isTransitioning();
    
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
	CloudsTransitionState getPendingState();
	CloudsTransitionState getCurrentState();
	CloudsTransitionState getPreviousState();
	string getPendingStateDescription();
	string getCurrentStateDescription();
    string getPreviousStateDescription();
	string getStateDescription(CloudsTransitionState state);
    
  protected:
	
	deque<CloudsTransitionQueueEntry> stateQueue;
	vector<CloudsTransitionQueueEntry> queueHistory;
	
	void queueState(CloudsTransitionState state, float transitionDuration);
	CloudsTransitionQueueEntry currentQueue;
	void startTransition();
	
	vector<CloudsTransitionState> fadeOutStates;
	vector<CloudsTransitionState> fadeInStates;
	
    void queueReciprocal(CloudsTransitionState state, float duration);
    
    map<CloudsTransitionState,CloudsTransitionState> reciprocalTransitions;
    
	CloudsTransitionState currentState;
	CloudsTransitionState previousState;
	CloudsTransitionState pendingState;
	float transitionStartTime;
	
	float transitionInCompleteTime;
	float transitionOutCompleteTime;
	void confirmEmpty(string transitionName);
	
	bool newState;
	CloudsTransitionState getNextState();
    
    int previousFadeVal;
	
};
