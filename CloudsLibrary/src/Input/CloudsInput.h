//
//  CloudsInput.h
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//

#pragma once

#include "ofMain.h"

class CloudsInputEvents;
class CloudsInput
{
  public:
	CloudsInput();
	
	virtual void enable() = 0;
	virtual void disable() = 0;
	
	CloudsInputEvents& getEvents();
	
  protected:

	bool enabled;
	CloudsInputEvents* events;
	
};

void SetCloudsInput(ofPtr<CloudsInput> input);
ofPtr<CloudsInput> GetCloudsInput();
