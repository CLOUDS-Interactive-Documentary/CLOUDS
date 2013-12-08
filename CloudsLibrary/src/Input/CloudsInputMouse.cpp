//
//  CloudsInputMouse.cpp
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//

#include "CloudsInputMouse.h"

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
	CloudsInteractionEventArgs args(ofVec3f(data.x,data.y,0), data.button);
	ofNotifyEvent(events.interactionMoved, args, this);
}

void CloudsInputMouse::mousePressed(ofMouseEventArgs& data){
	CloudsInteractionEventArgs args(ofVec3f(data.x,data.y,0), data.button);
	ofNotifyEvent(events.interactionStarted, args, this);
}

void CloudsInputMouse::mouseDragged(ofMouseEventArgs& data){
	CloudsInteractionEventArgs args(ofVec3f(data.x,data.y,0), data.button);
	ofNotifyEvent(events.interactionDragged, args, this);
}

void CloudsInputMouse::mouseReleased(ofMouseEventArgs& data){
	CloudsInteractionEventArgs args(ofVec3f(data.x,data.y,0), data.button);
	ofNotifyEvent(events.interactionEnded, args, this);
}
