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
}


CloudsInputEvents& CloudsInput::getEvents(){
	return *events;
}

static ofPtr<CloudsInput> cloudsInput;
void SetCloudsInput(ofPtr<CloudsInput> input){
	
	if(cloudsInput != NULL)
		cloudsInput->disable();
	
	cloudsInput = input;
	
	if(cloudsInput != NULL)
		cloudsInput->enable();
};

ofPtr<CloudsInput> GetCloudsInput(){
	if(cloudsInput == NULL){
		SetCloudsInput( ofPtr<CloudsInput>( new CloudsInputMouse() ));
	}
	return cloudsInput;
};

