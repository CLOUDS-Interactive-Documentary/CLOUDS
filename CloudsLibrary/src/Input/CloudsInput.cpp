//
//  CloudsInput.cpp
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//

#include "CloudsInput.h"
#include "CloudsInputEvents.h"
#include "CloudsInputMouse.h"
#ifdef KINECT_INPUT
#include "CloudsInputKinectOSC.h"
#endif
#ifdef OCULUS_RIFT
#include "CloudsInputOculus.h"
#endif
#ifdef TOUCH_INPUT
#include "CloudsInputTouchOSC.h"
#endif

CloudsInput::CloudsInput(){
	enabled = false;
	events = new CloudsInputEvents();
	currentPosition = ofVec3f();
	lastPosition = ofVec3f();
    bleedOffset = ofVec3f();
	dragging = false;
}

CloudsInputEvents& CloudsInput::getEvents(){
	return *events;
}

map<int, CloudsInteractionEventArgs>& CloudsInput::getInputPoints(){
    return inputPoints;
}

void CloudsInput::interactionMoved(ofVec3f pos, bool primary, bool dragged, int actionType, int playerId, float focus){
	CloudsInteractionEventArgs args(pos+bleedOffset, primary, dragged, actionType, playerId, focus);
    inputPoints[playerId] = args;
	ofNotifyEvent(getEvents().interactionMoved, args, this);	
}

void CloudsInput::interactionStarted(ofVec3f pos, bool primary, bool dragged, int actionType, int playerId, float focus){
	CloudsInteractionEventArgs args(pos+bleedOffset, primary, dragged, actionType, playerId, focus);
    inputPoints[playerId] = args;
	ofNotifyEvent(getEvents().interactionStarted, args, this);
}

void CloudsInput::interactionDragged(ofVec3f pos, bool primary, bool dragged, int actionType, int playerId, float focus){
	CloudsInteractionEventArgs args(pos+bleedOffset, primary, dragged, actionType, playerId, focus);
    inputPoints[playerId] = args;
	ofNotifyEvent(getEvents().interactionDragged, args, this);
}

void CloudsInput::interactionEnded(ofVec3f pos, bool primary, bool dragged, int actionType, int playerId, float focus){
	CloudsInteractionEventArgs args(pos+bleedOffset, primary, dragged, actionType, playerId, focus);
    inputPoints[playerId] = args;
	ofNotifyEvent(getEvents().interactionEnded, args, this);
}

bool CloudsInput::isDragging(){
	return dragging;
}
float CloudsInput::getPositionX(){
	return currentPosition.x;
}
float CloudsInput::getPositionY(){
	return currentPosition.y;
}
float CloudsInput::getPositionZ(){
	return currentPosition.z;
}
ofVec3f CloudsInput::getPosition(){
	return currentPosition;
}
ofVec3f CloudsInput::getPreviousPosition(){
	return currentPosition;
}

void CloudsInput::setBleedPixels(int b){
    bleedOffset = ofVec3f(0,0,0);
    bleed = b;
}

void CloudsInput::selfDrawCursorDefault(CloudsCursorMode mode, ofVec3f& pos, bool bDragged, float focus, float size)
{
	
    if (mode == CURSOR_MODE_NONE) return;
    // EZ: Don't draw INACTIVE cursors for now
    if (mode == CURSOR_MODE_INACTIVE) return;
    
    // EZ: Uncomment this line to test focus using the mouse
    //focus = ofMap(ofDist(pos.x, pos.y, ofGetWidth()/2, ofGetHeight()/2), 50, 400, 1.0f, 0.0f, true);
    
    ofPushStyle();
    ofDisableLighting();
	ofEnableAlphaBlending();
	
    if (mode == CURSOR_MODE_INACTIVE) {
        size *= 0.5f;
        ofSetLineWidth(3);
        ofSetColor(213, 69, 62, 192 * focus);
        ofLine(pos.x - size, pos.y - size, pos.x + size, pos.y + size);
        ofLine(pos.x - size, pos.y + size, pos.x + size, pos.y - size);
    }
    else if (mode == CURSOR_MODE_DRAW) {
        ofSetLineWidth(2);
        ofNoFill();
        ofSetColor(255, 255, 255, 192);
        ofCircle(pos, size);
//        ofFill();
//        ofSetColor(255, 255, 255, 64);
//        ofCircle(pos, size * focus);
    }
    else {  // mode == CURSOR_MODE_CAMERA
        ofSetLineWidth(2);
        
//        static const float kCoreRadius = 0.2f;
//        float lineLength = size * ofMap(focus, 0.0f, 1.0f, kCoreRadius, (1.0f - kCoreRadius));
        float lineLength;
        if (focus >= 0) {
            ofSetColor(255, 255, 255, 192);
            lineLength = ofMap(focus, 0.0f, 1.0f, 1, size);
        }
        else {
            if (focus < -0.8f) {
                ofColor flashColor(255);
                flashColor.lerp(ofColor(255, 0, 0), cosf(ofGetElapsedTimef() * 0.1f));
                ofSetColor(flashColor, MAX(35, 192 * focus * -1));
            }
            else {
                ofSetColor(255, 255, 255, MAX(35, 192 * focus * -1));
            }
            lineLength = size;
        }
        ofLine(pos.x - size, pos.y, pos.x - size + lineLength, pos.y);
        ofLine(pos.x + size, pos.y, pos.x + size - lineLength, pos.y);
        ofLine(pos.x, pos.y - size, pos.x, pos.y - size + lineLength);
        ofLine(pos.x, pos.y + size, pos.x, pos.y + size - lineLength);

//        if (bDragged) {
//            ofNoFill();
//            ofCircle(pos, lineLength);
//        }
//        else {  // !bDragged
//            ofSetColor(255, 255, 255, 64);
            ofFill();
            ofCircle(pos, 1);
//        }
    }
	ofEnableLighting();
    ofPopStyle();
}

///////////// //STATICS
static ofPtr<CloudsInput> cloudsInput;
void SetCloudsInput(ofPtr<CloudsInput> input){
	
	if(cloudsInput != NULL)
		cloudsInput->disable();
	
	cloudsInput = input;
	
	if(cloudsInput != NULL)
		cloudsInput->enable();
}

ofPtr<CloudsInput> GetCloudsInput(){
	if(cloudsInput == NULL){
#if defined(KINECT_INPUT)
        SetCloudsInputKinect();
#elif defined(OCULUS_RIFT)
        SetCloudsInputOculus();
//        SetCloudsInputMouse();
#elif defined(TOUCH_INPUT)
        SetCloudsInputTouchOSC();
#else
        SetCloudsInputMouse();
#endif
	}
	return cloudsInput;
}

//global accessors
bool GetCloudsInputPressed(){
	return GetCloudsInput()->isDragging();
}
float GetCloudsInputX(){
	return GetCloudsInput()->getPosition().x;
}
float GetCloudsInputY(){
	return GetCloudsInput()->getPosition().y;
}
float GetCloudsInputZ(){
	return GetCloudsInput()->getPosition().z;
}
ofVec3f GetCloudsInputPosition(){
	return GetCloudsInput()->getPosition();
}

map<int, CloudsInteractionEventArgs>& GetCloudsInputPoints(){
    return GetCloudsInput()->getInputPoints();
}

ofVec3f GetCloudsPreviousInputPosition(){
	return GetCloudsInput()->getPreviousPosition();
}
float GetCloudsPreviousInputX(){
	return GetCloudsPreviousInputPosition().x;
}
float GetCloudsPreviousInputY(){
	return GetCloudsPreviousInputPosition().y;
}
float GetCloudsPreviousInputZ(){
	return GetCloudsPreviousInputPosition().z;
}

void SetBleedPixels(int bleed){
    GetCloudsInput()->setBleedPixels(bleed);
}
