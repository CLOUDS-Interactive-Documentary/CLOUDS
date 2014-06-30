//
//  CloudsInputOculus.h
//  CLOUDS
//
//  Created by Elie Zananiri on 1/8/2014.
//
//

#pragma once

#ifdef OCULUS_RIFT

#include "CloudsInput.h"

class CloudsInputOculus : public CloudsInput
{
public:
	CloudsInputOculus();
    
	virtual void enable();
	virtual void disable();
	
	void update(ofEventArgs& data);
    
    void drawCursorDefault(CloudsCursorMode mode, ofVec3f& pos, bool bDragged, float focus, float fadeOut);
    
    float cursorSize;
};

void SetCloudsInputOculus();

#endif
