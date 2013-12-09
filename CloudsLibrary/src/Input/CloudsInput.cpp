//
//  CloudsInput.cpp
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//

#include "CloudsInput.h"
#include "CloudsInputEvents.h"
#include "CloudsInputMouse.h"

CloudsInput::CloudsInput(){
	enabled = false;
	events = new CloudsInputEvents();
	currentPosition = ofVec3f();
	dragging = false;
}


CloudsInputEvents& CloudsInput::getEvents(){
	return *events;
}

void CloudsInput::interactionMoved(ofVec3f pos, int actionType, int playerId){
//	if(dragging)
//		ofLogError("CloudsInput::interactionMoved") << "Dragging logic inconsistent. called Moved when Dragging";
	
	currentPosition = pos;
	CloudsInteractionEventArgs args(pos, actionType, playerId);
	ofNotifyEvent(getEvents().interactionMoved, args, this);	
}

void CloudsInput::interactionStarted(ofVec3f pos, int actionType, int playerId){
//	if(dragging)
//		ofLogError("CloudsInput::interactionStarted") << "Dragging logic inconsistent. calld Start when Dragging";
	
	currentPosition = pos;
	dragging = true;
	CloudsInteractionEventArgs args(pos, actionType, playerId);
	ofNotifyEvent(getEvents().interactionStarted, args, this);
}

void CloudsInput::interactionDragged(ofVec3f pos, int actionType, int playerId){
//	if(!dragging)
//		ofLogError("CloudsInput::interactionDragged") << "Dragging logic inconsistent. calld Drag before start";
	
	currentPosition = pos;	
	CloudsInteractionEventArgs args(pos, actionType, playerId);
	ofNotifyEvent(getEvents().interactionDragged, args, this);
}

void CloudsInput::interactionEnded(ofVec3f pos, int actionType, int playerId){
//	if(!dragging)
//		ofLogError("CloudsInput::interactionEnded") << "Dragging logic inconsistent. calld End while not Dragging";
	
	dragging = false;
	currentPosition = pos;
	CloudsInteractionEventArgs args(pos, actionType, playerId);
	ofNotifyEvent(getEvents().interactionEnded, args, this);
}

bool CloudsInput::isDragging(){
	return dragging;
}
float CloudsInput::getPositionX(){
	return currentPosition.x;
}
float CloudsInput::getPositionY(){
	return currentPosition.y;
}
float CloudsInput::getPositionZ(){
	return currentPosition.z;
}
ofVec3f CloudsInput::getPosition(){
	return currentPosition;
}

///////////// //STATICS
static ofPtr<CloudsInput> cloudsInput;
void SetCloudsInput(ofPtr<CloudsInput> input){
	
	if(cloudsInput != NULL)
		cloudsInput->disable();
	
	cloudsInput = input;
	
	if(cloudsInput != NULL)
		cloudsInput->enable();
}

ofPtr<CloudsInput> GetCloudsInput(){
	if(cloudsInput == NULL){
		SetCloudsInput( ofPtr<CloudsInput>( new CloudsInputMouse() ));
	}
	return cloudsInput;
}

//global accessors
bool GetCloudsInputPressed(){
	return GetCloudsInput()->isDragging();
}
float GetCloudsInputX(){
	return GetCloudsInput()->getPosition().x;
}
float GetCloudsInputY(){
	return GetCloudsInput()->getPosition().y;
}
float GetCloudsInputZ(){
	return GetCloudsInput()->getPosition().z;
}
ofVec3f GetCloudsInputPosition(){
	return GetCloudsInput()->getPosition();
}
