//
//  CloudsInputKinectOSC.cpp
//  YellowTail
//
//  Created by Elie Zananiri on 12/8/13.
//  
//

#include "CloudsInputKinectOSC.h"
#include "CloudsInputEvents.h"
#include "CloudsGlobal.h"

int kListenPort          = 12345;
int kNumFramesForRemoval = 60;
int kPollThreshold       = 5;

//--------------------------------------------------------------
CloudsInputKinectOSC::CloudsInputKinectOSC(float activeThresholdY, float activeThresholdZ)
: activeThresholdY(activeThresholdY)
, activeThresholdZ(activeThresholdZ)
, primaryIdx(-1)
, mainBodyIdx(-1)
, jointLerpPct(0.3f)
, focusRange(0.2f)
, bClampToBounds(true)
, bDoDebug(false)
, viewerState(k4w::ViewerState_None)
, viewerIdleTime(0)
, currViewerBodyIdx(-1)
, bCurrViewerHasInteracted(false)
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
, feedbackScale(0.2f)
, feedbackMargin(0.02f)
, feedbackAlpha(0)
, feedbackFade(1.0f)
, feedbackPrompt("")
{
    feedbackFont.loadFont(GetCloudsDataPath() + "font/Blender-BOOK.ttf", 15);
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
                mainBodyIdx = idx;
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
        if (ABS(it->second->lastUpdateFrame - ofGetFrameNum()) > kNumFramesForRemoval) {
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
        bool bDead = ABS(it->second->lastUpdateFrame - ofGetFrameNum()) > kNumFramesForRemoval;
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
    if (bodies.empty()) {
        // downgrayedd!
        viewerState = k4w::ViewerState_None;
    }
	else if (viewerState < k4w::ViewerState_Interacting && primaryIdx != -1) {
        // upgrayedd!
        viewerState = k4w::ViewerState_Interacting;
        currViewerBodyIdx = primaryIdx;
        bCurrViewerHasInteracted = true;
    }
    
    if (viewerState == k4w::ViewerState_PresentIdle) {
        viewerIdleTime += ofGetLastFrameTime() * 1000;
    }
    else {
        viewerIdleTime = 0;
    }
    if (viewerState <= k4w::ViewerState_OutOfRange || (primaryIdx != -1 && primaryIdx != currViewerBodyIdx)) {
        // this should work assuming a user can't just magically appear in PresentIdle state
        bCurrViewerHasInteracted = false;
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
    if (focusXY < focusZ) {
        joint.focus = powf(focusXY, 2.0f);
    }
    else {
        joint.focus = powf(focusZ, 2.0f);
        if (joint.screenPosition.z > activeThresholdZ) {
            if (joint.focus == 0) {
                joint.focus = 0.0000001;
            }
            joint.focus *= -1;
        }
    }
//    joint.focus = powf(MIN(focusXY, focusZ), 2.0f);
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
        ofDisableLighting();
        
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
            ofEndShape(false);
            
            // draw the arm span
            ofBeginShape();
            {
                ofVertex(body->shoulderLeftJoint.inputPosition);
				ofVertex(body->spineShoulderJoint.inputPosition);
				ofVertex(body->shoulderRightJoint.inputPosition);
            }
            ofEndShape(false);
            
            ofDrawBitmapString(ofToString(body->spineBaseJoint.inputPosition.z), body->spineBaseJoint.inputPosition);
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
void CloudsInputKinectOSC::draw()
{
    float margin = ofGetHeight() * feedbackMargin;
    float height = ofGetHeight() * feedbackScale;
    float width = height * 4.0f/3.0f;
    draw(margin, ofGetHeight() - height - margin, width, height);
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::draw(float x, float y, float width, float height)
{
    
    
    // Adjust the dimensions to fit in a 4:3 window cause stretching is gross.
	if (width/height != 4.0f/3.0f) {
		float newWidth = 4.0f * height / 3.0f;
		x += (width - newWidth) / 2.0f;
		width = newWidth;
	}
    
    static ofxEasingQuad easingQuad;
    
    ofPushStyle();
    
    ofSetLineWidth(2);

    // Display feedback if either:
    // 1. A viewer is detected but out of range (not in the hot seat)
    // 2. A viewer is in the hot seat AND has not began yet
    // 3. A viewer is interacting and pushing in too far
    bool bOutOfRange = (viewerState == k4w::ViewerState_OutOfRange);
    bool bHasNotBegun = (viewerState > k4w::ViewerState_OutOfRange && !bUserBegan);
    bool bPushTooFar = (primaryIdx != -1 && hands[primaryIdx]->handJoint.focus < 0 && hands[primaryIdx]->handJoint.focus > -0.3);

    // Hide feedback if either:
    // 1. No one is around
    // 2. A viewer just sat in the hot seat
    // 3. A viewer is interacting properly
    bool bNobody = (viewerState == k4w::ViewerState_None);
    bool bHasBegun = (viewerState > k4w::ViewerState_OutOfRange && bUserBegan);
    bool bGoodJob = (primaryIdx != -1 && (hands[primaryIdx]->handJoint.focus <= -0.3 || hands[primaryIdx]->handJoint.focus >= 0));
    
    if (!feedbackTween.isRunning()) {
        if (bOutOfRange || bHasNotBegun || bPushTooFar) {
            if (bOutOfRange) {
                feedbackPrompt = "HAVE A SEAT";
            }
            else if (bHasNotBegun) {
                feedbackPrompt = "SELECT THE CIRCLE";
            }
            else {  // bPushTooFar
                feedbackPrompt = "TOO CLOSE";
            }
            feedbackTween.setParameters(easingQuad, ofxTween::easeOut, feedbackAlpha, 0.4f * 255, 1000, 0);
            feedbackTween.addValue(1.0f, 1.0f);
            feedbackTween.start();
        }
        else if (bNobody || bHasBegun || bGoodJob) {
            feedbackTween.setParameters(easingQuad, ofxTween::easeOut, feedbackAlpha, 0, 250, bNobody? 4000 : (bHasBegun? 3000 : 500));
            feedbackTween.addValue(feedbackFade, 1.25f);
            feedbackTween.start();
        }
    }
//    else {
//        feedbackPrompt = "";
//        feedbackAlpha = ofLerp(feedbackAlpha, 0, 0.5f);
//    }
    
    if (feedbackTween.isRunning()) {
        feedbackAlpha = feedbackTween.update();
        feedbackFade = feedbackTween.getTarget(1);
    }
    
    // Draw the text prompt.
    ofSetColor(ofColor::white, feedbackAlpha);
    feedbackFont.drawString(feedbackPrompt, x + (width - feedbackFont.stringWidth(feedbackPrompt)) / 2, y + height - feedbackFont.stringHeight(feedbackPrompt));
	
    ofPushMatrix();
    {
        // scale up from our -1, 1 viewport
        ofTranslate(x, y);
        ofScale(width / 2.0f, height / 2.0f);
        ofTranslate(1, 1);
        ofScale(1 * feedbackFade, -1 * feedbackFade);
        
        ofNoFill();
		ofRect(-1, -1, 2, 2);
        
        ofScale(1.5, 1.5);
        ofTranslate(0, -0.2);
        
        if (viewerState < k4w::ViewerState_PresentIdle) {
            ofSetColor(ofColor::gray, feedbackAlpha);
        }
        
        // TODO: Make sure the skeletons are drawn inside the box.
//        glEnable(GL_SCISSOR_TEST);
//        glScissor-1, -1, 2, 2);

        // draw bodies
        for (map<int, k4w::Body *>::iterator it = bodies.begin(); it != bodies.end(); ++it) {
            if (viewerState > k4w::ViewerState_OutOfRange && it->first != mainBodyIdx) {
                // Only draw the main body.
                continue;
            }
            
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
            
            if (viewerState > k4w::ViewerState_OutOfRange && hand->bodyIdx != mainBodyIdx) {
                // Only draw the main body hands.
                continue;
            }
            
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
        
//        glDisable(GL_SCISSOR_TEST);
    }
    ofPopMatrix();
    
    ofPopStyle();
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::drawCursorDefault(CloudsCursorMode mode, ofVec3f& pos, bool bDragged, float focus, float fadeOut){
    float cursorSize;
    if (mode == CURSOR_MODE_INACTIVE)
        cursorSize = cursorUpSizeMin;
    else if (bDragged)
        cursorSize = ofMap(pos.z, 2, -2, cursorDownSizeMin, cursorDownSizeMax, true);
    else
        cursorSize = ofMap(pos.z, 2, -2, cursorUpSizeMin, cursorUpSizeMax, true);
    
    //cout << "DRAW CURSOR " << pos << " mode " << mode << " fade " << fadeOut << " size " << cursorSize << endl;
    fadeOut = 1.0;
    
    selfDrawCursorDefault(mode, pos, bDragged, focus, fadeOut, cursorSize);
}

//--------------------------------------------------------------
void SetCloudsInputKinect(float activeThresholdY, float activeThresholdZ)
{
    SetCloudsInput(ofPtr<CloudsInput>(new CloudsInputKinectOSC(activeThresholdY, activeThresholdZ)));
}
