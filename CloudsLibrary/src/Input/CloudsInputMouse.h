//
//  CloudsInputMouse.h
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//

#pragma once

#include "CloudsInput.h"

class CloudsInputMouse : public CloudsInput
{
  public:
	CloudsInputMouse();
    
	virtual void enable();
	virtual void disable();
	
	void mouseMoved(ofMouseEventArgs& data);
	void mousePressed(ofMouseEventArgs& data);
	void mouseDragged(ofMouseEventArgs& data);
	void mouseReleased(ofMouseEventArgs& data);
    
    void drawCursorDefault(CloudsCursorMode mode, ofVec3f& pos, bool bDragged, float focus, float fadeOut);
    
    float cursorDownSize;
    float cursorUpSize;
};

void SetCloudsInputMouse();