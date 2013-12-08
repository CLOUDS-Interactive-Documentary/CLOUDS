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

class CloudsInput;

class CloudsInteractionEventArgs : public ofEventArgs {
public:
	CloudsInteractionEventArgs(ofVec3f position, int actionType = 0, int playerId = 0)
		: position(position), actionType(actionType), playerId(playerId){}
	
	ofVec3f position;
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

static ofPtr<CloudsInput> cloudsInput;
static void CloudsSetInput(ofPtr<CloudsInput> input);
static ofPtr<CloudsInput> CloudsGetInput();

template<class ListenerClass>
static void CloudsRegisterInputEvents(ListenerClass * listener);

template<class ListenerClass>
static void CloudsUnregisterInputEvents(ListenerClass * listener);
