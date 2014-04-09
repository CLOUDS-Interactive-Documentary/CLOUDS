//
//  CloudsInputTouchOSC.cpp
//  Ripples
//
//  Created by Elie Zananiri on 2014-03-24.
//
//

#include "CloudsInputTouchOSC.h"
#include "CloudsInputEvents.h"

//--------------------------------------------------------------
int CloudsInputTouchOSC::kListenPort          = 12345;
int CloudsInputTouchOSC::kNumFramesForRemoval = 60;
int CloudsInputTouchOSC::kPollThreshold       = 5;

//--------------------------------------------------------------
CloudsInputTouchOSC::CloudsInputTouchOSC()
: primaryIdx(-1)
, posResetLerpPct(0.1f)
, posSetLerpPct(0.3f)
, posSetInstantThreshold(20)
, posOutOfBoundsStart(0)
, posOutOfBoundsDelay(2000)
{
    
}

//--------------------------------------------------------------
void CloudsInputTouchOSC::enable()
{
	if (!enabled) {
        receiver.setup(kListenPort);
        ofAddListener(ofEvents().update, this, &CloudsInputTouchOSC::update);
		enabled = true;
	}
}

//--------------------------------------------------------------
void CloudsInputTouchOSC::disable()
{
	if (enabled) {
//        receiver.shutdown();
        ofRemoveListener(ofEvents().update, this, &CloudsInputTouchOSC::update);
		enabled = false;
	}
}

//--------------------------------------------------------------
void CloudsInputTouchOSC::update(ofEventArgs& args)
{
    // check for waiting messages
    while (receiver.hasWaitingMessages()) {
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
        bool bRecognized = false;
        
		if (m.getAddress().find("/clouds/touch") != string::npos) {
            bRecognized = true;
            
            // set up all the working vars
            TouchState newTouchState;
            lastOscFrame = ofGetFrameNum();
            
            vector<string> tokens = ofSplitString(m.getAddress(), "/", true);
            int idx = ofToInt(tokens[2]);
            
            // create a touch if it doesn't exist yet
            if (touches.find(idx) == touches.end()) {
                touches[idx] = new Touch();
                touches[idx]->idx = idx;
            }
            
            // update the position
            touches[idx]->inputPosition.set(ofVec3f(m.getArgAsFloat(1), m.getArgAsFloat(0)));
            mapCoords(touches[idx]);
            
            touches[idx]->activeFrames++;
            
            newTouchState = TouchState_Down;
            
            touches[idx]->poll[newTouchState]++;
            if (touches[idx]->poll[newTouchState] >= kPollThreshold) {
                // boom! new state achieved
                processTouchEvent(idx, touches[idx], newTouchState);
                touches[idx]->state = newTouchState;
                
                for (int i = 0; i < TouchState_Count; i++) {
                    if (i != newTouchState) {
                        touches[idx]->poll[i] = 0;
                    }
                }
            }
            else {
                // carry on with the same state
                processTouchEvent(idx, touches[idx], touches[idx]->state);
            }
            
            // refresh the update frame
            touches[idx]->lastUpdateFrame = lastOscFrame;

        }
        
        if (!bRecognized) {
			// display in the console
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for (int i = 0; i < m.getNumArgs(); i++) {
				// get the argument type
				msg_string += m.getArgTypeName(i);
				msg_string += ":";
				// display the argument - make sure we get the right type
				if (m.getArgType(i) == OFXOSC_TYPE_INT32) {
					msg_string += ofToString(m.getArgAsInt32(i));
				}
				else if (m.getArgType(i) == OFXOSC_TYPE_FLOAT) {
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if (m.getArgType(i) == OFXOSC_TYPE_STRING) {
					msg_string += m.getArgAsString(i);
				}
				else {
					msg_string += "unknown";
				}
                msg_string += " ";
			}
            ofLogError("CloudsInputTouchOSC") << "Unrecognized message: " << msg_string;
		}
    }
    
    // process any dead or inactive hands
    vector<int> toRemove;
    for (map<int, Touch *>::iterator it = touches.begin(); it != touches.end(); ++it) {
//        bool bDead = ABS(it->second->lastUpdateFrame - lastOscFrame) > kNumFramesForRemoval;
        bool bDead = ABS(it->second->lastUpdateFrame - ofGetFrameNum()) > kNumFramesForRemoval;
        if (bDead || it->second->state == TouchState_Up) {
            // make sure the hand is not mid-action when getting removed
            processTouchEvent(it->first, touches[it->first], TouchState_Up);
            
            // if the hand was the primary cursor, unlink it
            if (it->first == primaryIdx) {
                primaryIdx = -1;
            }
            
            if (inputPoints.find(it->first) != inputPoints.end()) {
                // remove the matching input point
                inputPoints.erase(it->first);
            }
            
            if (bDead) {
                // only remove dead touches
                // keep inactive touches to continue tracking their state
                toRemove.push_back(it->first);
            }
        }
    }
    for (int i = 0; i < toRemove.size(); i++) {
        delete touches[toRemove[i]];
        touches.erase(toRemove[i]);
    }
    
    // unlink the primary cursor if it's been inactive for too long
    if (primaryIdx > -1 && touches[primaryIdx]->activeFrames <= 0) {
        primaryIdx = -1;
    }
    
    // look for a new designated cursor if necessary
    if (primaryIdx == -1) {
        int candidateIdx = -1;
        int candidateActiveFrames = 0;
        float candidateY = -1;
        for (map<int, Touch *>::iterator it = touches.begin(); it != touches.end(); ++it) {
            // select this touch if it is active AND either:
            //  1. there is no candidate yet
            //  2. it is older than the current candidate
            //  3. it is as old the current candidate but higher up
            if (it->second->activeFrames >= kPollThreshold &&
                ((candidateIdx == -1) ||
                 (candidateActiveFrames < it->second->activeFrames) ||
                 (candidateActiveFrames == it->second->activeFrames &&
                  candidateY < it->second->inputPosition.y))) {
                     candidateIdx = it->first;
                     candidateActiveFrames = it->second->activeFrames;
                     candidateY = it->second->inputPosition.y;
                 }
        }
        primaryIdx = candidateIdx;
    }
    
    // set the current position using magic
    lastPosition = currentPosition;
    if (primaryIdx == -1) {
        if (posOutOfBoundsStart == 0) {
            posOutOfBoundsStart = ofGetElapsedTimeMillis();
        }
        if (ofGetElapsedTimeMillis() - posOutOfBoundsStart > posOutOfBoundsDelay) {
            // waited long enough, start heading back
            currentPosition.interpolate(ofVec3f(ofGetWidth() * 0.5, ofGetHeight() * 0.5), posResetLerpPct);
        }
        // else: keep waiting for it to come back
    }
    else {
        posOutOfBoundsStart = 0;
        if (currentPosition.distance(touches[primaryIdx]->screenPosition) <= posSetInstantThreshold) {
            currentPosition.set(touches[primaryIdx]->screenPosition);
        }
        else {
            currentPosition.interpolate(touches[primaryIdx]->screenPosition, posSetLerpPct);
        }
    }
}

//--------------------------------------------------------------
void CloudsInputTouchOSC::mapCoords(Touch *touch)
{
    touch->screenPosition.set(ofMap(touch->inputPosition.x, 0, 1, 0, ofGetWidth()),
                              ofMap(touch->inputPosition.y, 1, 0, 0, ofGetHeight()));
}

//--------------------------------------------------------------
void CloudsInputTouchOSC::processTouchEvent(int idx, Touch *touch, TouchState newState)
{
    bool bPrimary = (idx == primaryIdx);
    if (newState == TouchState_Down) {
        if (touch->state == TouchState_Down) {
            // matching state: continue
            interactionDragged(touch->screenPosition, bPrimary, TouchState_Down, idx);
        }
        else {
            // idle/inactive state: start
            if (bPrimary) dragging = true;
            interactionStarted(touch->screenPosition, bPrimary, TouchState_Down, idx);
            touch->state = TouchState_Down;
        }
    }
    else {  // (newState == TouchState_Up)
        if (touch->state != TouchState_Up) {
            // state mismatch: end previous
            if (bPrimary) dragging = false;
            interactionEnded(touch->screenPosition, bPrimary, touch->state, idx);
            touch->state = TouchState_Up;
        }
    }
}

//--------------------------------------------------------------
void SetCloudsInputTouchOSC()
{
    SetCloudsInput(ofPtr<CloudsInput>(new CloudsInputTouchOSC()));
}
