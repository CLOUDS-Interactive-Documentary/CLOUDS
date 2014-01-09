//
//  CloudsInputOculus.h
//  CLOUDS
//
//  Created by Elie Zananiri on 1/8/2014.
//
//

#pragma once

#include "CloudsInput.h"

class CloudsInputOculus : public CloudsInput
{
public:
	
	virtual void enable();
	virtual void disable();
	
	void update(ofEventArgs& data);    
};

void SetCloudsInputOculus();
