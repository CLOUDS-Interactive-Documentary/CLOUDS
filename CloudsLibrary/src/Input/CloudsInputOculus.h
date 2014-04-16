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
	CloudsInputOculus();
    
	virtual void enable();
	virtual void disable();
	
	void update(ofEventArgs& data);
    
    void drawCursor(CloudsCursorMode mode, ofVec3f& pos, bool bDragged, float focus);
    
    float cursorSize;
};

void SetCloudsInputOculus();
