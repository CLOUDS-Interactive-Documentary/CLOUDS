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
	interactionMoved(ofVec3f(data.x,data.y,0), data.button);
}

void CloudsInputMouse::mousePressed(ofMouseEventArgs& data){
	interactionStarted(ofVec3f(data.x,data.y,0), data.button);
}

void CloudsInputMouse::mouseDragged(ofMouseEventArgs& data){
	interactionDragged(ofVec3f(data.x,data.y,0), data.button);
}

void CloudsInputMouse::mouseReleased(ofMouseEventArgs& data){
	interactionEnded(ofVec3f(data.x,data.y,0), data.button);
}
