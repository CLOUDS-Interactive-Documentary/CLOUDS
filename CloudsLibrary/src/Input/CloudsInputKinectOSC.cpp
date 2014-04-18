//
//  CloudsInputKinectOSC.cpp
//  YellowTail
//
//  Created by Elie Zananiri on 12/8/13.
//  
//

#include "CloudsInputKinectOSC.h"
#include "CloudsInputEvents.h"

int kListenPort          = 12345;
int kNumFramesForRemoval = 60;
int kPollThreshold       = 5;

//--------------------------------------------------------------
CloudsInputKinectOSC::CloudsInputKinectOSC(float activeThresholdY, float activeThresholdZ)
: activeThresholdY(activeThresholdY)
, activeThresholdZ(activeThresholdZ)
, primaryIdx(-1)
, jointLerpPct(0.3f)
, focusRange(0.2f)
, bClampToBounds(true)
, bDoDebug(false)
, viewerState(k4w::ViewerState_None)
, viewerIdleTime(0)
, boundsMin(-0.5f, -0.7f, 1.0f)
, boundsMax( 0.5f, -0.2f, 2.0f)
, posResetLerpPct(0.1f)
, posSetLerpPct(0.3f)
, posSetInstantThreshold(20)
, posOutOfBoundsStart(0)
, posOutOfBoundsDelay(2000)
, cursorDownSizeMin(5)
, cursorDownSizeMax(10)
, cursorUpSizeMin(8)
, cursorUpSizeMax(16)
{

}

//--------------------------------------------------------------
void CloudsInputKinectOSC::enable()
{
	if (!enabled) {
        receiver.setup(kListenPort);
        ofAddListener(ofEvents().update, this, &CloudsInputKinectOSC::update);
		enabled = true;
	}
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::disable()
{
	if (enabled) {
//        receiver.shutdown();
        ofRemoveListener(ofEvents().update, this, &CloudsInputKinectOSC::update);
		enabled = false;
	}
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::update(ofEventArgs& args)
{
    if (receiver.hasWaitingMessages()) {
        // set viewer state to its lowest form, then bring it up as needed
        viewerState = k4w::ViewerState_None;
    }
    
    // check for waiting messages
    while (receiver.hasWaitingMessages()) {
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
        bool bRecognized = false;
        
		if (m.getAddress() == "/playerData") {
            bRecognized = true;
            
            // set up all the working vars
            k4w::HandState newHandState;
            lastOscFrame = ofGetFrameNum();

            float activeThresholdPosY = ofMap(activeThresholdY, 0, 1, 1, -1);
            bool bDoProximity = (activeThresholdZ != 1);
            float activeThresholdPosZ = ofMap(activeThresholdZ, 0, 1, 0, -1);
            float zRef = bDoProximity? activeThresholdPosZ : 0;

            int i = 0;
			int idx = m.getArgAsInt32(i++);
                        
            // process the body data
            
            // create a body if it doesn't exist yet
            if (bodies.find(idx) == bodies.end()) {
                bodies[idx] = new k4w::Body();
                bodies[idx]->idx = idx;
            }
            
            // update the head joint
            bodies[idx]->headJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->headJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->headJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i+0), 
                                                                     m.getArgAsFloat(i+1), 
                                                                     m.getArgAsFloat(i+2)), jointLerpPct);
            i += 3;  // can't i++ in method with multiple parameters because VS is stupid and processes them backwards

            // update the neck joint
            bodies[idx]->neckJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->neckJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->neckJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i+0), 
                                                                     m.getArgAsFloat(i+1),
                                                                     m.getArgAsFloat(i+2)), jointLerpPct);
            i += 3;  // can't i++ in method with multiple parameters because VS is stupid and processes them backwards

            // update the spine shoulder joint
            bodies[idx]->spineShoulderJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->spineShoulderJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->spineShoulderJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i+0), 
                                                                              m.getArgAsFloat(i+1),
                                                                              m.getArgAsFloat(i+2)), jointLerpPct);
            i += 3;  // can't i++ in method with multiple parameters because VS is stupid and processes them backwards

            // update the spine mid joint
            bodies[idx]->spineMidJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->spineMidJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->spineMidJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i+0), 
                                                                         m.getArgAsFloat(i+1),
                                                                         m.getArgAsFloat(i+2)), jointLerpPct);
            i += 3;  // can't i++ in method with multiple parameters because VS is stupid and processes them backwards

            // update the spine base joint
            bodies[idx]->spineBaseJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->spineBaseJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->spineBaseJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i+0), 
                                                                          m.getArgAsFloat(i+1),
                                                                          m.getArgAsFloat(i+2)), jointLerpPct);
			i += 3;  // can't i++ in method with multiple parameters because VS is stupid and processes them backwards

			// update the left shoulder joint
            bodies[idx]->shoulderLeftJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->shoulderLeftJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->shoulderLeftJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i+0), 
                                                                             m.getArgAsFloat(i+1),
                                                                             m.getArgAsFloat(i+2)), jointLerpPct);
            i += 3;  // can't i++ in method with multiple parameters because VS is stupid and processes them backwards

            // update the right shoulder joint
            bodies[idx]->shoulderRightJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->shoulderRightJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->shoulderRightJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i+0), 
                                                                              m.getArgAsFloat(i+1),
                                                                              m.getArgAsFloat(i+2)), jointLerpPct);
            i += 3;  // can't i++ in method with multiple parameters because VS is stupid and processes them backwards

			// update the left elbow joint
            bodies[idx]->elbowLeftJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->elbowLeftJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->elbowLeftJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i+0), 
                                                                          m.getArgAsFloat(i+1),
                                                                          m.getArgAsFloat(i+2)), jointLerpPct);
            i += 3;  // can't i++ in method with multiple parameters because VS is stupid and processes them backwards

            // update the right elbow joint
            bodies[idx]->elbowRightJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->elbowRightJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->elbowRightJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i+0), 
                                                                           m.getArgAsFloat(i+1),
                                                                           m.getArgAsFloat(i+2)), jointLerpPct);
            i += 3;  // can't i++ in method with multiple parameters because VS is stupid and processes them backwards

            // calculate the head to spine base length for mapping height
            float mappingWidth = MAX(bodies[idx]->spineShoulderJoint.inputPosition.distance(bodies[idx]->shoulderLeftJoint.inputPosition), 
                                     bodies[idx]->spineShoulderJoint.inputPosition.distance(bodies[idx]->shoulderRightJoint.inputPosition));
            float mappingHeight = (bodies[idx]->headJoint.inputPosition.distance(bodies[idx]->spineMidJoint.inputPosition)
                                   + bodies[idx]->spineMidJoint.inputPosition.distance(bodies[idx]->spineBaseJoint.inputPosition) * 0.5f) * 0.5f;
            float neckOverlapWidth = mappingWidth * 0.2f;
            
            // these are commented out because we now have two origins (left and right hand), so the x-map would not make sense
            //mapCoords(bodies[idx]->headJoint, bodies[idx]->spineShoulderJoint.inputPosition, mappingLength);
            //mapCoords(bodies[idx]->neckJoint, bodies[idx]->spineShoulderJoint.inputPosition, mappingLength);
            //mapCoords(bodies[idx]->spineShoulderJoint, bodies[idx]->spineShoulderJoint.inputPosition, mappingLength);
            //mapCoords(bodies[idx]->spineMidJoint, bodies[idx]->spineShoulderJoint.inputPosition, mappingLength);
            //mapCoords(bodies[idx]->spineBaseJoint, bodies[idx]->spineShoulderJoint.inputPosition, zRef, mappingWidth, mappingHeight);
            
            // check if the body is within the tracked bounds
            bool bBodyOutOfBounds = (bodies[idx]->spineBaseJoint.inputPosition.x < boundsMin.x ||
                                     bodies[idx]->spineBaseJoint.inputPosition.y < boundsMin.y ||
                                     bodies[idx]->spineBaseJoint.inputPosition.z < boundsMin.z ||
                                     bodies[idx]->spineBaseJoint.inputPosition.x > boundsMax.x ||
                                     bodies[idx]->spineBaseJoint.inputPosition.y > boundsMax.y ||
                                     bodies[idx]->spineBaseJoint.inputPosition.z > boundsMax.z);
            
            // refresh the update frame and age
            bodies[idx]->lastUpdateFrame = lastOscFrame;
            bodies[idx]->age++;
            
            // process the hand data
            for (int j = 0; j < 2; j++) {
                int handIdx = idx * 2 + j;
                
                // create a hand if it doesn't exist yet
                if (hands.find(handIdx) == hands.end()) {
                    hands[handIdx] = new k4w::Hand();
                    hands[handIdx]->idx = handIdx;
                    hands[handIdx]->bodyIdx = idx;
                }
                
                // update the hand joint
                hands[handIdx]->handJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
                hands[handIdx]->handJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
                hands[handIdx]->handJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i+0), 
                                                                            m.getArgAsFloat(i+1), 
                                                                            m.getArgAsFloat(i+2)), jointLerpPct);
                i += 3;  // can't i++ in method with multiple parameters because VS is stupid and processes them backwards

                // set the custom origin and bounds
                ofVec3f origin = bodies[idx]->headJoint.inputPosition;
                if (hands[handIdx]->handJoint.type == k4w::JointType_HandLeft) {
                    origin.x -= mappingWidth - neckOverlapWidth;
                }
                else {
                    origin.x += mappingWidth - neckOverlapWidth;
                }
                origin.y -= mappingHeight;
                hands[handIdx]->trackingBounds.setFromCenter(origin, mappingWidth * 2, mappingHeight * 2);
                
                if (bClampToBounds) {
                    // clamp to left, right, and top edges
                    hands[handIdx]->handJoint.clampedPosition.x = ofClamp(hands[handIdx]->handJoint.inputPosition.x, origin.x - mappingWidth, origin.x + mappingWidth);
                    hands[handIdx]->handJoint.clampedPosition.y = MIN(hands[handIdx]->handJoint.inputPosition.y, origin.y + mappingHeight);
                }
				else {
					hands[handIdx]->handJoint.clampedPosition.x = hands[handIdx]->handJoint.inputPosition.x;
					hands[handIdx]->handJoint.clampedPosition.y = hands[handIdx]->handJoint.inputPosition.y;
				}
				hands[handIdx]->handJoint.clampedPosition.z = hands[handIdx]->handJoint.inputPosition.z;
                
                // map the input to local and screen coordinates
                mapHandCoords(hands[handIdx]->handJoint, origin, zRef, mappingWidth, mappingHeight);
                
                bool bHandOutOfBounds = (hands[handIdx]->handJoint.clampedPosition.x < hands[handIdx]->trackingBounds.getMinX() ||
                                         hands[handIdx]->handJoint.clampedPosition.x > hands[handIdx]->trackingBounds.getMaxX() ||
                                         hands[handIdx]->handJoint.clampedPosition.y < hands[handIdx]->trackingBounds.getMinY() ||
                                         hands[handIdx]->handJoint.clampedPosition.y > hands[handIdx]->trackingBounds.getMaxY());
                
                // set the new hand state, and calculate active frames while we're at it
                if (bBodyOutOfBounds ||
                    bHandOutOfBounds ||
                    hands[handIdx]->handJoint.trackingState != k4w::TrackingState_Tracked) {
                    
                    // out of bounds or inactive, discard
                    hands[handIdx]->activeFrames = MAX(0, MIN(kPollThreshold, hands[handIdx]->activeFrames - 1));
                    
                    newHandState = k4w::HandState_NotTracked;
                }
                else {
                    // in bounds and active
                    hands[handIdx]->activeFrames++;
                    
                    // process the event
//                    if (hands[handIdx]->activeFrames == 0) {
//                        // out of bounds or inactive, discard
//                        newHandState = k4w::HandState_NotTracked;
//                    }
//                    else 
                    if (bDoProximity) {
                        // ignore the incoming state and use proximity
//                        if (hands[handIdx]->handJoint.trackingState < k4w::TrackingState_Tracked) {
//                            // the hand is inferred or not tracked, discard that shit
//                            newHandState = k4w::HandState_NotTracked;
//                        }
//                        else 
                        if (hands[handIdx]->handJoint.localPosition.z < activeThresholdPosZ) {
                            newHandState = k4w::HandState_Lasso;
                        }
                        else {
                            newHandState = k4w::HandState_Open;
                        }
                        
                    }
                    else {
                        newHandState = (k4w::HandState)m.getArgAsInt32(i);
                    }
                }
                i++;  // bump the message index
                
                hands[handIdx]->poll[newHandState]++;
                if (hands[handIdx]->poll[newHandState] >= kPollThreshold) {
                    // boom! new state achieved
                    processHandEvent(handIdx, hands[handIdx], newHandState);
                    hands[handIdx]->handJoint.handState = newHandState;
                    
                    for (int k = 0; k < k4w::HandState_Count; k++) {
                        if (k != newHandState) {
                            hands[handIdx]->poll[k] = 0;
                        }
                    }
                }
                else {
                    // carry on with the same state
                    processHandEvent(handIdx, hands[handIdx], hands[handIdx]->handJoint.handState);
                }
                
                // refresh the update frame
                hands[handIdx]->lastUpdateFrame = lastOscFrame;
            }
            
            // bump up the viewer state as there's someone detected
            // we'll bump to "interacting" outside the loop further down...
            if (viewerState == k4w::ViewerState_None) {
                // upgrayedd!
                viewerState = k4w::ViewerState_OutOfRange;
            }
            if (viewerState < k4w::ViewerState_PresentIdle && !bBodyOutOfBounds) {
                // upgrayedd!
                viewerState = k4w::ViewerState_PresentIdle;
            }
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
            ofLogError("CloudsInputKinectOSC") << "Unrecognized message: " << msg_string;
		}
	}
    
    // remove any dead bodies
    vector<int> toRemove;
    for (map<int, k4w::Body *>::iterator it = bodies.begin(); it != bodies.end(); ++it) {
        if (ABS(it->second->lastUpdateFrame - lastOscFrame) > kNumFramesForRemoval) {
            toRemove.push_back(it->first);
        }
    }
    for (int i = 0; i < toRemove.size(); i++) {
        delete bodies[toRemove[i]];
        bodies.erase(toRemove[i]);
    }
    
    // process any dead or inactive hands
    toRemove.clear();
    for (map<int, k4w::Hand *>::iterator it = hands.begin(); it != hands.end(); ++it) {
        bool bDead = ABS(it->second->lastUpdateFrame - lastOscFrame) > kNumFramesForRemoval;
        if (bDead || it->second->actionState == k4w::ActionState_Inactive) {
            // make sure the hand is not mid-action when getting removed
            processHandEvent(it->first, hands[it->first], k4w::HandState_Unknown);
            
            // if the hand was the primary cursor, unlink it 
            if (it->first == primaryIdx) {
                primaryIdx = -1;
            }
            
            if (inputPoints.find(it->first) != inputPoints.end()) {
                // remove the matching input point
                inputPoints.erase(it->first);
            }
            
            if (bDead) {
                // only remove dead hands
                // keep inactive hands to continue tracking their state
                toRemove.push_back(it->first);
            }
        }
    }
    for (int i = 0; i < toRemove.size(); i++) {
        delete hands[toRemove[i]];
        hands.erase(toRemove[i]);
    }
    
    // unlink the primary cursor if it's been inactive for too long
    if (primaryIdx > -1 && hands[primaryIdx]->activeFrames <= 0) {
        primaryIdx = -1;
    }
    
    // look for a new designated cursor if necessary
    if (primaryIdx == -1) {
        int candidateIdx = -1;
        int candidateActiveFrames = 0;
        float candidateY = -1;
        for (map<int, k4w::Hand *>::iterator it = hands.begin(); it != hands.end(); ++it) {
            // select this hand if it is active AND either:
            //  1. there is no candidate yet
            //  2. it is older than the current candidate
            //  3. it is as old the current candidate but higher up
            if (it->second->activeFrames >= kPollThreshold && 
                    ((candidateIdx == -1) || 
                     (candidateActiveFrames < it->second->activeFrames) || 
                     (candidateActiveFrames == it->second->activeFrames && 
                         candidateY < it->second->handJoint.clampedPosition.y))) {
                candidateIdx = it->first;
                candidateActiveFrames = it->second->activeFrames;
                candidateY = it->second->handJoint.clampedPosition.y;
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
        if (currentPosition.distance(hands[primaryIdx]->handJoint.screenPosition) <= posSetInstantThreshold) {
            currentPosition.set(hands[primaryIdx]->handJoint.screenPosition);
        }
        else {
            currentPosition.interpolate(hands[primaryIdx]->handJoint.screenPosition, posSetLerpPct);
        }
    }
    
    // update the viewer state and idle time
	if (viewerState < k4w::ViewerState_Interacting && primaryIdx != -1) {
        // upgrayedd!
        viewerState = k4w::ViewerState_Interacting;
    }
    if (viewerState == k4w::ViewerState_PresentIdle) {
        viewerIdleTime += ofGetLastFrameTime() * 1000;
    }
    else {
        viewerIdleTime = 0;
    }
    
    if (viewerState == k4w::ViewerState_None) {
        cout << "VIEWER STATE:: NONE" << endl;
    }
    else if (viewerState == k4w::ViewerState_OutOfRange) {
        cout << "VIEWER STATE:: OUT OF RANGE" << endl;
    }
    else if (viewerState == k4w::ViewerState_PresentIdle) {
        cout << "VIEWER STATE:: PRESENT IDLE " << viewerIdleTime << endl;
    }
    else {
        cout << "VIEWER STATE:: INTERACTING" << endl;
    }
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::mapHandCoords(k4w::HandJoint& joint, ofVec3f& origin, float zRef, float width, float height)
{
    // switch to a local coord system, centered at origin
    joint.localPosition = joint.clampedPosition;
    joint.localPosition -= origin;
    
    // map the local position to the 2D viewport coord system
    float inMaxY = -(height - (2 * (1.0f - activeThresholdY) * height));
    joint.screenPosition.set(ofMap(joint.localPosition.x, -width, width,  0, ofGetWidth()),
                             ofMap(joint.localPosition.y, height, inMaxY, 0, ofGetHeight()),
                             ofMap(joint.localPosition.z, zRef - width, zRef + width,  1, -1));
    
    // calculate the focus
    float focusXY = ofMap(joint.inputPosition.distance(joint.clampedPosition), 0.0f, 0.5f, 1.0f, 0.0f, true);
    //float focusZ = ofMap(joint.screenPosition.z, 0.2f, 1.0f, 1.0f, 0.0f, true);
    float focusZ = ofMap(ABS(joint.screenPosition.z - activeThresholdZ), focusRange, 1.0f, 1.0f, 0.0f, true);
    joint.focus = powf(MIN(focusXY, focusZ), 2.0f);
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::processHandEvent(int handIdx, k4w::Hand * hand, k4w::HandState newState)
{    
    bool primary = (handIdx == primaryIdx);
    if (newState == k4w::HandState_Lasso) {
        if (hand->actionState == k4w::ActionState_Lasso) {
            // matching state: continue
            interactionDragged(hand->handJoint.screenPosition, primary, true, k4w::ActionState_Lasso, handIdx, hand->handJoint.focus);
        }
        else if (hand->actionState == k4w::ActionState_Closed) {
            // state mismatch: end previous
            if (primary) dragging = false;
            interactionEnded(hand->handJoint.screenPosition, primary, true, k4w::ActionState_Closed, handIdx, hand->handJoint.focus);
            hand->actionState = k4w::ActionState_Idle;
        }
        else {
            // idle/inactive state: start
            if (primary) dragging = true;
            interactionStarted(hand->handJoint.screenPosition, primary, true, k4w::ActionState_Lasso, handIdx, hand->handJoint.focus);
            hand->actionState = k4w::ActionState_Lasso;
        }
    }  
    else if (newState == k4w::HandState_Closed) {
        if (hand->actionState == k4w::ActionState_Closed) {
            // matching state: continue
            interactionDragged(hand->handJoint.screenPosition, primary, true, k4w::ActionState_Closed, handIdx, hand->handJoint.focus);
        }
        else if (hand->actionState == k4w::ActionState_Lasso) {
            // state mismatch: end previous
            if (primary) dragging = false;
            interactionEnded(hand->handJoint.screenPosition, primary, true, k4w::ActionState_Lasso, handIdx, hand->handJoint.focus);
            hand->actionState = k4w::ActionState_Idle;
        }
        else {
            // idle/inactive state: start
            if (primary) dragging = true;
            interactionStarted(hand->handJoint.screenPosition, primary, true, k4w::ActionState_Closed, handIdx, hand->handJoint.focus);
            hand->actionState = k4w::ActionState_Closed;
        }
    }
    else if (newState == k4w::HandState_Open) {
        if (hand->actionState == k4w::ActionState_Idle) {
            // matching state: continue
            interactionMoved(hand->handJoint.screenPosition, primary, false, k4w::ActionState_Idle, handIdx, hand->handJoint.focus);
        }
        else {
            // state mismatch: end previous
            if (primary) dragging = false;
            interactionEnded(hand->handJoint.screenPosition, primary, (hand->actionState > k4w::ActionState_Idle), hand->actionState, handIdx, hand->handJoint.focus);
            hand->actionState = k4w::ActionState_Idle;
        }
    }
    else {  // (newState == k4w::HandState_NotTracked || newState == k4w::HandState_Unknown)
        if (hand->actionState != k4w::ActionState_Inactive) {
            // state mismatch: end previous
            if (primary) dragging = false;
            interactionEnded(hand->handJoint.screenPosition, primary, (hand->actionState > k4w::ActionState_Idle), hand->actionState, handIdx, hand->handJoint.focus);
            hand->actionState = k4w::ActionState_Inactive;
        }
    }
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::debug(float x, float y, float width, float height)
{
    ofPushStyle();
    ofPushMatrix();
    {
        // scale up from our -1, 1 viewport
        ofTranslate(x, y);
        ofScale(width / 2.0f, height / 2.0f);
        ofTranslate(1, 1);
        ofScale(1, -1);
        
//        ofSetColor(255, 0, 0);
//        ofRect(-1, -1, 2, 2);
        
        // draw bodies
        for (map<int, k4w::Body *>::iterator it = bodies.begin(); it != bodies.end(); ++it) {
            k4w::Body * body = it->second;
            
            ofSetColor(ofColor::gray);
            ofNoFill();
            
            // draw the spine
            ofBeginShape();
            {
                ofVertex(body->headJoint.inputPosition);
                ofVertex(body->neckJoint.inputPosition);
                ofVertex(body->spineShoulderJoint.inputPosition);
                ofVertex(body->spineMidJoint.inputPosition);
                ofVertex(body->spineBaseJoint.inputPosition);
            }
            ofEndShape();
            
            // draw the arm span
            ofBeginShape();
            {
                ofVertex(body->shoulderLeftJoint.inputPosition);
				ofVertex(body->spineShoulderJoint.inputPosition);
				ofVertex(body->shoulderRightJoint.inputPosition);
            }
            ofEndShape();
        }
        
        // draw hands
        for (map<int, k4w::Hand *>::iterator it = hands.begin(); it != hands.end(); ++it) {
            k4w::Hand * hand = it->second;
            
            // draw the arm
            ofSetColor(ofColor::gray);
            //ofLine(hand->handJoint.clampedPosition, (hand->handJoint.type == k4w::JointType_HandLeft)? bodies[hand->bodyIdx]->elbowLeftJoint.inputPosition : bodies[hand->bodyIdx]->elbowRightJoint.inputPosition);
            
            bool bActive = (hand->activeFrames > 0);
            bool bInBounds = (hand->handJoint.clampedPosition.x >= hand->trackingBounds.getMinX() &&
                              hand->handJoint.clampedPosition.x <= hand->trackingBounds.getMaxX() &&
                              hand->handJoint.clampedPosition.y >= hand->trackingBounds.getMinY() &&
                              hand->handJoint.clampedPosition.y <= hand->trackingBounds.getMaxY());
            
            if (hand->handJoint.trackingState == k4w::TrackingState_Tracked) {
                if (bActive) {
                    ofSetColor(ofColor::darkGreen.getLerped(ofColor::white, sinf(ofGetElapsedTimef() * 15.0f) * 0.5f + 0.5f));
                }
                else {
                    ofSetColor(ofColor::darkGreen);
                }
            }
            else if (hand->handJoint.trackingState == k4w::TrackingState_Inferred) {
                ofSetColor(ofColor::yellow);
            }
            else {
                ofSetColor(ofColor::darkRed);
            }
            
            // fill if we're in the tracking zone
            if (bInBounds) {
                ofFill();
            }
            else {
                ofNoFill();
            }
            ofCircle(hand->handJoint.clampedPosition, 0.02f);
            
            ofNoFill();
            ofSetColor(ofColor::white);
            ofRect(hand->trackingBounds);
            ofLine(hand->trackingBounds.x,
                   hand->trackingBounds.y + hand->trackingBounds.height * (1.0f - activeThresholdY),
                   hand->trackingBounds.x + hand->trackingBounds.width,
                   hand->trackingBounds.y + hand->trackingBounds.height * (1.0f - activeThresholdY));
        }
        
        // draw bounds
        ofNoFill();
        ofSetColor(ofColor::cyan);
        ofRect(boundsMin.x, boundsMin.y, boundsMax.x - boundsMin.x, boundsMax.y - boundsMin.y);
    }
    ofPopMatrix();
    ofPopStyle();
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::draw(float x, float y, float width, float height, float alpha)
{
    // Adjust the dimensions to fit in a 4:3 window cause stretching is gross.
	if (width/height != 4.0f/3.0f) {
		float newWidth = 4.0f * height / 3.0f;
		x += (width - newWidth) / 2.0f;
		width = newWidth;
	}
	
	ofPushStyle();
    ofPushMatrix();
    {
        // scale up from our -1, 1 viewport
        ofTranslate(x, y);
        ofScale(width / 2.0f, height / 2.0f);
        ofTranslate(1, 1);
        ofScale(1, -1);
        
		ofSetColor(ofColor::white, alpha);

        ofNoFill();
		ofRect(-1, -1, 2, 2);

        // draw bodies
        for (map<int, k4w::Body *>::iterator it = bodies.begin(); it != bodies.end(); ++it) {
            k4w::Body * body = it->second;
            
            ofNoFill();
            
            // draw the spine
            ofBeginShape();
            {
                ofVertex(body->headJoint.inputPosition);
                ofVertex(body->neckJoint.inputPosition);
                ofVertex(body->spineShoulderJoint.inputPosition);
                ofVertex(body->spineMidJoint.inputPosition);
                ofVertex(body->spineBaseJoint.inputPosition);
            }
            ofEndShape();
            
            // draw the arm span
            ofBeginShape();
            {
                ofVertex(body->elbowLeftJoint.inputPosition);
                ofVertex(body->shoulderLeftJoint.inputPosition);
				ofVertex(body->spineShoulderJoint.inputPosition);
				ofVertex(body->shoulderRightJoint.inputPosition);
				ofVertex(body->elbowRightJoint.inputPosition);
            }
            ofEndShape();
        }
        
        // draw hands
        for (map<int, k4w::Hand *>::iterator it = hands.begin(); it != hands.end(); ++it) {
            k4w::Hand * hand = it->second;
            
            // draw the arm
            ofLine(hand->handJoint.inputPosition, (hand->handJoint.type == k4w::JointType_HandLeft)? bodies[hand->bodyIdx]->elbowLeftJoint.inputPosition : bodies[hand->bodyIdx]->elbowRightJoint.inputPosition);
            
            bool bActive = (hand->activeFrames > 0);
            bool bInBounds = (hand->handJoint.inputPosition.x >= hand->trackingBounds.getMinX() &&
                              hand->handJoint.inputPosition.x <= hand->trackingBounds.getMaxX() &&
                              hand->handJoint.inputPosition.y >= hand->trackingBounds.getMinY() &&
                              hand->handJoint.inputPosition.y <= hand->trackingBounds.getMaxY());
            
			// draw the hand
            ofCircle(hand->handJoint.inputPosition, 0.02f);
        }
    }
    ofPopMatrix();
    ofPopStyle();
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::drawCursorDefault(CloudsCursorMode mode, ofVec3f& pos, bool bDragged, float focus){
    float cursorSize;
    if (mode == CURSOR_MODE_INACTIVE)
        cursorSize = cursorUpSizeMin;
    else if (bDragged)
        cursorSize = ofMap(pos.z, 2, -2, cursorDownSizeMin, cursorDownSizeMax, true);
    else
        cursorSize = ofMap(pos.z, 2, -2, cursorUpSizeMin, cursorUpSizeMax, true);
    selfDrawCursorDefault(mode, pos, bDragged, focus, cursorSize);
}

//--------------------------------------------------------------
void SetCloudsInputKinect(float activeThresholdY, float activeThresholdZ)
{
    SetCloudsInput(ofPtr<CloudsInput>(new CloudsInputKinectOSC(activeThresholdY, activeThresholdZ)));
}
