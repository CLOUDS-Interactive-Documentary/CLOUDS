//
//  CloudsInputEvents.cpp
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//

#include "CloudsInputEvents.h"
#include "CloudsInputMouse.h"

void CloudsSetInput(ofPtr<CloudsInput> input){
	
	if(cloudsInput != NULL)
		cloudsInput->disable();
	
	cloudsInput = input;
	
	if(cloudsInput != NULL)
		cloudsInput->enable();
};

ofPtr<CloudsInput> CloudsGetInput(){
	if(cloudsInput == NULL){
		CloudsSetInput( ofPtr<CloudsInput>( new CloudsInputMouse() ));
	}
	return cloudsInput;
};

template<class ListenerClass>
void CloudsRegisterInputEvents(ListenerClass * listener){
	ofAddListener(CloudsGetInput()->getEvents().interactionMoved, listener, &ListenerClass::interactionMoved);
	ofAddListener(CloudsGetInput()->getEvents().interactionStarted, listener, &ListenerClass::interactionMoved);
	ofAddListener(CloudsGetInput()->getEvents().interactionDragged, listener, &ListenerClass::interactionDragged);
	ofAddListener(CloudsGetInput()->getEvents().interactionEnded, listener, &ListenerClass::interactionEnded);
}

template<class ListenerClass>
void CloudsUnregisterInputEvents(ListenerClass * listener){
	ofRemoveListener(CloudsGetInput()->getEvents().interactionMoved, listener, &ListenerClass::interactionMoved);
	ofRemoveListener(CloudsGetInput()->getEvents().interactionStarted, listener, &ListenerClass::interactionMoved);
	ofRemoveListener(CloudsGetInput()->getEvents().interactionDragged, listener, &ListenerClass::interactionDragged);
	ofRemoveListener(CloudsGetInput()->getEvents().interactionEnded, listener, &ListenerClass::interactionEnded);
}