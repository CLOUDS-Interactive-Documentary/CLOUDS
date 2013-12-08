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

void CloudsGetInput(ofPtr<CloudsInput> input){
	if(cloudsInput == NULL){
		CloudsSetInput( ofPtr<CloudsInput>( new CloudsInputMouse() ));
	}
	return cloudsInput;
};
