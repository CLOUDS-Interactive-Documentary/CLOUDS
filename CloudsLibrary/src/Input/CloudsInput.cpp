//
//  CloudsInput.cpp
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//

#include "CloudsInput.h"


CloudsInput::CloudsInput(){
	enabled = false;
}

CloudsInputEvents& CloudsInput::getEvents(){
	return events;
}