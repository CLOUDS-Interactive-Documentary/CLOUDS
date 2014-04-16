//
//  CloudsInputEvents.h
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//


////////////////////// INPUT
#pragma once

#include "ofMain.h"
#include "CloudsInput.h"

//class CloudsInput;

class CloudsInteractionEventArgs : public ofEventArgs {
public:
    CloudsInteractionEventArgs()
    	: position(ofVec3f()), primary(false), dragged(false), actionType(-1), playerId(-1), focus(1.0f){}
	CloudsInteractionEventArgs(ofVec3f position, bool primary = false, bool dragged=false, int actionType = 0, int playerId = 0, float focus = 1.0f)
		: position(position), primary(primary), dragged(dragged), actionType(actionType), playerId(playerId), focus(focus){}
	
	ofVec3f position;
    bool primary;
    bool dragged;
	int actionType; //will signal grip vs finger, or left click right click
	int playerId; //0 for mouse, 0, 1, 2 or 3 for kinect based on the hand
    float focus;  //accuracy of the pointer, 0-1 for kinect, 1 for everything else
};

class CloudsInputEvents {
public:
	ofEvent<CloudsInteractionEventArgs> interactionMoved;
	ofEvent<CloudsInteractionEventArgs> interactionStarted;
	ofEvent<CloudsInteractionEventArgs> interactionDragged;
	ofEvent<CloudsInteractionEventArgs> interactionEnded;
};

template<class ListenerClass>
void CloudsRegisterInputEvents(ListenerClass * listener){
	ofAddListener(GetCloudsInput()->getEvents().interactionMoved, listener, &ListenerClass::interactionMoved);
	ofAddListener(GetCloudsInput()->getEvents().interactionStarted, listener, &ListenerClass::interactionStarted);
	ofAddListener(GetCloudsInput()->getEvents().interactionDragged, listener, &ListenerClass::interactionDragged);
	ofAddListener(GetCloudsInput()->getEvents().interactionEnded, listener, &ListenerClass::interactionEnded);
}

template<class ListenerClass>
void CloudsUnregisterInputEvents(ListenerClass * listener){
	ofRemoveListener(GetCloudsInput()->getEvents().interactionMoved, listener, &ListenerClass::interactionMoved);
	ofRemoveListener(GetCloudsInput()->getEvents().interactionStarted, listener, &ListenerClass::interactionStarted);
	ofRemoveListener(GetCloudsInput()->getEvents().interactionDragged, listener, &ListenerClass::interactionDragged);
	ofRemoveListener(GetCloudsInput()->getEvents().interactionEnded, listener, &ListenerClass::interactionEnded);
}