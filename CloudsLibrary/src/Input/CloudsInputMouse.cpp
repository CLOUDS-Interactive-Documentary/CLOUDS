//
//  CloudsInputMouse.cpp
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//

#include "CloudsInputMouse.h"
#include "CloudsInputEvents.h"

void CloudsInputMouse::enable(){
	if(!enabled){
		ofRegisterMouseEvents(this);
		enabled = true;
	}
}

void CloudsInputMouse::disable(){
	if(enabled){
		ofUnregisterMouseEvents(this);
		enabled = false;
	}
}

void CloudsInputMouse::mouseMoved(ofMouseEventArgs& data){
    lastPosition = currentPosition;
    currentPosition = ofVec3f(data.x, data.y, 0);
	interactionMoved(currentPosition, true, data.button);
}

void CloudsInputMouse::mousePressed(ofMouseEventArgs& data){
    currentPosition = ofVec3f(data.x, data.y, 0);
    dragging = true;
	interactionStarted(currentPosition, true, data.button);
}

void CloudsInputMouse::mouseDragged(ofMouseEventArgs& data){
    currentPosition = ofVec3f(data.x, data.y, 0);
	interactionDragged(currentPosition, true, data.button);
}

void CloudsInputMouse::mouseReleased(ofMouseEventArgs& data){
    dragging = false;
    currentPosition = ofVec3f(data.x, data.y, 0);
	interactionEnded(currentPosition, true, data.button);
}

void SetCloudsInputMouse()
{
    SetCloudsInput(ofPtr<CloudsInput>(new CloudsInputMouse()));
}
