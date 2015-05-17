//
//  CloudsInputMouse.h
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//

#pragma once

#include "CloudsInput.h"

#include "ofxTween.h"

// TODO: Merge this enum with the k4w::ViewerState
enum CloudsViewerState 
{
    ViewerState_None        = 0,
    ViewerState_OutOfRange  = 1,
    ViewerState_PresentIdle = 2,
    ViewerState_Interacting = 3
};

class CloudsInputMouse : public CloudsInput
{
  public:
	CloudsInputMouse();
    
	void enable();
	void disable();
	
	void update(ofEventArgs& args);

	void mouseMoved(ofMouseEventArgs& data);
	void mousePressed(ofMouseEventArgs& data);
	void mouseDragged(ofMouseEventArgs& data);
	void mouseReleased(ofMouseEventArgs& data);
    void mouseScrolled(ofMouseEventArgs& data);

	void drawFeedback(float width, float height, bool drawBrightText);
	float feedbackAlpha;
    ofTrueTypeFont feedbackFont;
    string feedbackPrompt;
    ofxTween feedbackTween;
    
    void drawCursorDefault(CloudsCursorMode mode, ofVec3f& pos, bool bDragged, float focus, float fadeOut);

	CloudsViewerState viewerState;
	float lastMouseEventTime;
	float mouseEventIdleInterval;
    
    float cursorDownSize;
    float cursorUpSize;
};

void SetCloudsInputMouse();