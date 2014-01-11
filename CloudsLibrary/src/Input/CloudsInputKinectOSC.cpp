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
, boundsMin(-0.5f, -0.7f, 1.0f)
, boundsMax( 0.5f, -0.2f, 2.0f)
, posResetLerpPct(0.1f)
, posSetLerpPct(0.3f)
, posSetInstantThreshold(20)
, posOutOfBoundsStart(0)
, posOutOfBoundsDelay(2000)
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
            bodies[idx]->headJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i++), 
                                                                     m.getArgAsFloat(i++), 
                                                                     m.getArgAsFloat(i++)), jointLerpPct);
            
            // update the neck joint
            bodies[idx]->neckJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->neckJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->neckJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i++), 
                                                                     m.getArgAsFloat(i++),
                                                                     m.getArgAsFloat(i++)), jointLerpPct);
            
            // update the spine shoulder joint
            bodies[idx]->spineShoulderJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->spineShoulderJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->spineShoulderJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i++), 
                                                                              m.getArgAsFloat(i++),
                                                                              m.getArgAsFloat(i++)), jointLerpPct);
            
            // update the spine mid joint
            bodies[idx]->spineMidJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->spineMidJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->spineMidJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i++), 
                                                                         m.getArgAsFloat(i++),
                                                                         m.getArgAsFloat(i++)), jointLerpPct);
            
            // update the spine base joint
            bodies[idx]->spineBaseJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->spineBaseJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->spineBaseJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i++), 
                                                                          m.getArgAsFloat(i++),
                                                                          m.getArgAsFloat(i++)), jointLerpPct);
            
            // update the left shoulder joint
            bodies[idx]->shoulderLeftJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->shoulderLeftJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->shoulderLeftJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i++), 
                                                                             m.getArgAsFloat(i++),
                                                                             m.getArgAsFloat(i++)), jointLerpPct);
            
            // update the right shoulder joint
            bodies[idx]->shoulderRightJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->shoulderRightJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->shoulderRightJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i++), 
                                                                              m.getArgAsFloat(i++),
                                                                              m.getArgAsFloat(i++)), jointLerpPct);
            
            // calculate the head to spine base length for mapping height
            float mappingWidth = MAX(bodies[idx]->spineShoulderJoint.inputPosition.distance(bodies[idx]->shoulderLeftJoint.inputPosition), 
                                     bodies[idx]->spineShoulderJoint.inputPosition.distance(bodies[idx]->shoulderRightJoint.inputPosition));
            float mappingHeight = (bodies[idx]->neckJoint.inputPosition.distance(bodies[idx]->spineMidJoint.inputPosition)
                                   + bodies[idx]->spineMidJoint.inputPosition.distance(bodies[idx]->spineBaseJoint.inputPosition) * 0.5f) * 0.5f;
            float neckOverlapWidth = mappingWidth * 0.2f;
            
            // map the spine points
            // these are commented out because we now have two origins (left and right hand), so the x-map would not make sense
            //mapCoords(bodies[idx]->spineShoulderJoint.inputPosition, mappingLength, bodies[idx]->headJoint);
            //mapCoords(bodies[idx]->spineShoulderJoint.inputPosition, mappingLength, bodies[idx]->neckJoint);
            //mapCoords(bodies[idx]->spineShoulderJoint.inputPosition, mappingLength, bodies[idx]->spineShoulderJoint);
            //mapCoords(bodies[idx]->spineShoulderJoint.inputPosition, mappingLength, bodies[idx]->spineMidJoint);
            mapCoords(bodies[idx]->spineShoulderJoint.inputPosition, zRef, mappingWidth, mappingHeight, bodies[idx]->spineBaseJoint);
            
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
                hands[handIdx]->handJoint.inputPosition.interpolate(ofVec3f(m.getArgAsFloat(i++), 
                                                                            m.getArgAsFloat(i++), 
                                                                            m.getArgAsFloat(i++)), jointLerpPct);
                
                // set the custom origin and map the hand coords
                ofVec3f origin = bodies[idx]->neckJoint.inputPosition;
                if (hands[handIdx]->handJoint.type == k4w::JointType_HandLeft) {
                    origin.x -= mappingWidth - neckOverlapWidth;
                }
                else {
                    origin.x += mappingWidth - neckOverlapWidth;
                }
                origin.y -= mappingHeight;
                mapCoords(origin, zRef, mappingWidth, mappingHeight, hands[handIdx]->handJoint);
                
                hands[handIdx]->trackingBounds.setFromCenter(origin, mappingWidth * 2, mappingHeight * 2);
                
                // set the new hand state, and calculate active frames while we're at it
                if (bBodyOutOfBounds ||
                    hands[handIdx]->handJoint.trackingState != k4w::TrackingState_Tracked ||
                    !hands[handIdx]->trackingBounds.inside(hands[handIdx]->handJoint.inputPosition)) {
                    
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
                         candidateY < it->second->handJoint.inputPosition.y))) {
                candidateIdx = it->first;
                candidateActiveFrames = it->second->activeFrames;
                candidateY = it->second->handJoint.inputPosition.y;
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
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::mapCoords(ofVec3f& origin, float zRef, float width, float height, k4w::Joint& joint)
{
    // switch to a local coord system, centered at origin
    joint.localPosition = joint.inputPosition;
    joint.localPosition -= origin;
    
    // map the local position to the 2D viewport coord system
    float inMaxY = -(height - (2 * (1.0f - activeThresholdY) * height));
    joint.screenPosition.set(ofMap(joint.localPosition.x, -width, width,  0, ofGetWidth()),
                             ofMap(joint.localPosition.y, height, inMaxY, 0, ofGetHeight()),
                             ofMap(joint.localPosition.z, zRef - width, zRef + width,  1, -1));
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::processHandEvent(int handIdx, k4w::Hand * hand, k4w::HandState newState)
{    
    bool primary = (handIdx == primaryIdx);
    if (newState == k4w::HandState_Lasso) {
        if (hand->actionState == k4w::ActionState_Lasso) {
            // matching state: continue
            interactionDragged(hand->handJoint.screenPosition, primary, k4w::ActionState_Lasso, handIdx);
        }
        else if (hand->actionState == k4w::ActionState_Closed) {
            // state mismatch: end previous
            if (primary) dragging = false;
            interactionEnded(hand->handJoint.screenPosition, primary, k4w::ActionState_Closed, handIdx);
            hand->actionState = k4w::ActionState_Idle;
        }
        else {
            // idle/inactive state: start
            if (primary) dragging = true;
            interactionStarted(hand->handJoint.screenPosition, primary, k4w::ActionState_Lasso, handIdx);
            hand->actionState = k4w::ActionState_Lasso;
        }
    }  
    else if (newState == k4w::HandState_Closed) {
        if (hand->actionState == k4w::ActionState_Closed) {
            // matching state: continue
            interactionDragged(hand->handJoint.screenPosition, primary, k4w::ActionState_Closed, handIdx);
        }
        else if (hand->actionState == k4w::ActionState_Lasso) {
            // state mismatch: end previous
            if (primary) dragging = false;
            interactionEnded(hand->handJoint.screenPosition, primary, k4w::ActionState_Lasso, handIdx);
            hand->actionState = k4w::ActionState_Idle;
        }
        else {
            // idle/inactive state: start
            if (primary) dragging = true;
            interactionStarted(hand->handJoint.screenPosition, primary, k4w::ActionState_Closed, handIdx);
            hand->actionState = k4w::ActionState_Closed;
        }
    }
    else if (newState == k4w::HandState_Open) {
        if (hand->actionState == k4w::ActionState_Idle) {
            // matching state: continue
            interactionMoved(hand->handJoint.screenPosition, primary, k4w::ActionState_Idle, handIdx);
        }
        else {
            // state mismatch: end previous
            if (primary) dragging = false;
            interactionEnded(hand->handJoint.screenPosition, primary, hand->actionState, handIdx);
            hand->actionState = k4w::ActionState_Idle;
        }
    }
    else {  // (newState == k4w::HandState_NotTracked || newState == k4w::HandState_Unknown)
        if (hand->actionState != k4w::ActionState_Inactive) {
            // state mismatch: end previous
            if (primary) dragging = false;
            interactionEnded(hand->handJoint.screenPosition, primary, hand->actionState, handIdx);
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
        
        for (map<int, k4w::Hand *>::iterator it = hands.begin(); it != hands.end(); ++it) {
            k4w::Hand * hand = it->second;
            
            // draw the arm
            ofSetColor(ofColor::gray);
            ofLine(hand->handJoint.inputPosition, (hand->handJoint.type == k4w::JointType_HandLeft)? bodies[hand->bodyIdx]->shoulderLeftJoint.inputPosition : bodies[hand->bodyIdx]->shoulderRightJoint.inputPosition);
            
            bool bActive = (hand->activeFrames > 0);
            bool bInBounds = hand->trackingBounds.inside(hand->handJoint.inputPosition);
            
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
            ofCircle(hand->handJoint.inputPosition, 0.02f);
            
            ofNoFill();
            ofSetColor(ofColor::white);
            ofRect(hand->trackingBounds);
        }
    }
    ofPopMatrix();
    ofPopStyle();
}

//--------------------------------------------------------------
void SetCloudsInputKinect(float activeThresholdY, float activeThresholdZ)
{
    SetCloudsInput(ofPtr<CloudsInput>(new CloudsInputKinectOSC(activeThresholdY, activeThresholdZ)));
}
