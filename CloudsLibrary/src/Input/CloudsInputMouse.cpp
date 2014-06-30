//
//  CloudsInputMouse.cpp
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//

#include "CloudsInputMouse.h"
#include "CloudsInputEvents.h"
#include "CloudsGlobal.h"

CloudsInputMouse::CloudsInputMouse()
: viewerState(ViewerState_PresentIdle)
, lastMouseEventTime(0)
, mouseEventIdleInterval(5)
, feedbackAlpha(0)
, feedbackPrompt("USE THE MOUSE TO INTERACT")
, cursorDownSize(10)
, cursorUpSize(12)
{
	feedbackFont.loadFont(GetCloudsDataPath() + "font/Blender-BOOK.ttf", 19);
}

void CloudsInputMouse::enable(){
	if(!enabled){
		ofAddListener(ofEvents().update, this, &CloudsInputMouse::update);
		ofRegisterMouseEvents(this);
		enabled = true;
	}
}

void CloudsInputMouse::disable(){
	if(enabled){
		ofRemoveListener(ofEvents().update, this, &CloudsInputMouse::update);
		ofUnregisterMouseEvents(this);
		enabled = false;
	}
}

void CloudsInputMouse::update(ofEventArgs& args){
	float nowTime = ofGetElapsedTimef();
	if (nowTime - lastMouseEventTime > mouseEventIdleInterval) {
		viewerState = ViewerState_None;
	}
	else if (nowTime - lastMouseEventTime > 0.1f) {
		viewerState = ViewerState_PresentIdle;
	}
}

void CloudsInputMouse::mouseMoved(ofMouseEventArgs& data){
	lastMouseEventTime = ofGetElapsedTimef();
    viewerState = ViewerState_Interacting;
		
	lastPosition = currentPosition;
    currentPosition = ofVec3f(data.x, data.y, 0);
	interactionMoved(currentPosition, true, dragging, data.button);
}

void CloudsInputMouse::mousePressed(ofMouseEventArgs& data){
    lastMouseEventTime = ofGetElapsedTimef();
    viewerState = ViewerState_Interacting;
	
	currentPosition = ofVec3f(data.x, data.y, 0);
    dragging = true;
	interactionStarted(currentPosition, true, dragging, data.button);
}

void CloudsInputMouse::mouseDragged(ofMouseEventArgs& data){
    lastMouseEventTime = ofGetElapsedTimef();
    viewerState = ViewerState_Interacting;
	
	currentPosition = ofVec3f(data.x, data.y, 0);
	interactionDragged(currentPosition, true, dragging, data.button);
}

void CloudsInputMouse::mouseReleased(ofMouseEventArgs& data){
    lastMouseEventTime = ofGetElapsedTimef();
    viewerState = ViewerState_Interacting;
	
	dragging = false;
    currentPosition = ofVec3f(data.x, data.y, 0);
	interactionEnded(currentPosition, true, dragging, data.button);
}

void CloudsInputMouse::drawCursorDefault(CloudsCursorMode mode, ofVec3f& pos, bool bDragged, float focus, float fadeOut){
    selfDrawCursorDefault(mode, pos, bDragged, focus, fadeOut, bDragged? cursorDownSize:cursorUpSize);
}

void CloudsInputMouse::drawFeedback(float width, float height, bool drawBrightText){
	ofPushStyle();
	static ofxEasingQuad easingQuad;

	if ((!feedbackTween.isRunning() && feedbackTween.isCompleted())) {
		if (viewerState == ViewerState_None && feedbackAlpha != 255) {
			feedbackTween.setParameters(easingQuad, ofxTween::easeOut, feedbackAlpha, 255, 1000, 0);
			feedbackTween.start();
		}
		else if (viewerState != ViewerState_None && feedbackAlpha != 0) {
			feedbackTween.setParameters(easingQuad, ofxTween::easeOut, feedbackAlpha, 0, 1000, 0);
			feedbackTween.start();
		}
	}

	feedbackAlpha = feedbackTween.update();
	float positionX = (width - feedbackFont.stringWidth(feedbackPrompt)) / 2;
	float positionY = height - feedbackFont.stringHeight(feedbackPrompt) - 10;
	ofSetColor(drawBrightText ? ofColor::black : ofColor::white, feedbackAlpha);
    feedbackFont.drawString(feedbackPrompt, positionX, positionY);
	ofSetColor(drawBrightText ? ofColor::white : ofColor::black, feedbackAlpha);
    feedbackFont.drawString(feedbackPrompt, positionX-3, positionY-3);
	ofPopStyle();
}

void SetCloudsInputMouse()
{
    SetCloudsInput(ofPtr<CloudsInput>(new CloudsInputMouse()));
}
