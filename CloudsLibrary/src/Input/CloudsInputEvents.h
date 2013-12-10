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
	CloudsInteractionEventArgs(ofVec3f position, bool primary = false, int actionType = 0, int playerId = 0)
		: position(position), primary(primary), actionType(actionType), playerId(playerId){}
	
	ofVec3f position;
    bool primary;
	int actionType; //will signal grip vs finger, or left click right click
	int playerId; //0 for mouse, 0 or 1 for kinect based on the person
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