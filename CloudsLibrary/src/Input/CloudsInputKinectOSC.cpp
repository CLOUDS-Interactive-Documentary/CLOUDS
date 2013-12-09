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
void CloudsInputKinectOSC::enable(){
	if(!enabled){
        receiver.setup(kListenPort);
        ofAddListener(ofEvents().update, this, &CloudsInputKinectOSC::update);
		enabled = true;
	}
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::disable(){
	if(enabled){
//        receiver.shutdown();
        ofRemoveListener(ofEvents().update, this, &CloudsInputKinectOSC::update);
		enabled = false;
	}
}

//--------------------------------------------------------------
void CloudsInputKinectOSC::update(ofEventArgs& args){
	// check for waiting messages
    while (receiver.hasWaitingMessages()) {
        lastOscFrame = ofGetFrameNum();
        
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
        bool bRecognized = true;
        
        k4w::HandState newHandState;
        
		if (m.getAddress() == "/playerData") {
            int i = 0;
			int idx = m.getArgAsInt32(i++);
            
//            cout << "RECEIVED idx " << idx << " on frame " << lastOscFrame << endl;
            
            // create a body if it doesn't exist yet
            if (bodies.find(idx) == bodies.end()) {
                bodies[idx] = new k4w::Body();
                bodies[idx]->idx = idx;
            }
            
            // update the head joint
            bodies[idx]->headJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->headJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->headJoint.position.set(m.getArgAsFloat(i++), 
                                                m.getArgAsFloat(i++), 
                                                m.getArgAsFloat(i++));
            
            // update the left hand joint
            bodies[idx]->leftHandJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->leftHandJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->leftHandJoint.position.set(m.getArgAsFloat(i++), 
                                                    m.getArgAsFloat(i++), 
                                                    m.getArgAsFloat(i++));
            newHandState = (k4w::HandState)m.getArgAsInt32(i++);
            bodies[idx]->leftHandJoint.poll[newHandState]++;
            if (bodies[idx]->leftHandJoint.poll[newHandState] >= kPollThreshold) {
                // boom! new state achieved
                processHandEvent(idx, 0, bodies[idx]->leftHandJoint, newHandState);
                bodies[idx]->leftHandJoint.handState = newHandState;
                
                for (int i = 0; i < k4w::HandState_Count; i++) {
                    if (i != newHandState) {
                        bodies[idx]->leftHandJoint.poll[i] = 0;
                    }
                }
            }
            else {
                // carry on with the same state
                processHandEvent(idx, 0, bodies[idx]->leftHandJoint, bodies[idx]->leftHandJoint.handState);
            }
            
            // update the right hand joint
            bodies[idx]->rightHandJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->rightHandJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->rightHandJoint.position.set(m.getArgAsFloat(i++), 
                                                     m.getArgAsFloat(i++), 
                                                     m.getArgAsFloat(i++));
            newHandState = (k4w::HandState)m.getArgAsInt32(i++);
            bodies[idx]->rightHandJoint.poll[newHandState]++;
            if (bodies[idx]->rightHandJoint.poll[newHandState] >= kPollThreshold) {
                // boom! new state achieved
                processHandEvent(idx, 3, bodies[idx]->rightHandJoint, newHandState);
                bodies[idx]->rightHandJoint.handState = newHandState;
                
                for (int i = 0; i < k4w::HandState_Count; i++) {
                    if (i != newHandState) {
                        bodies[idx]->rightHandJoint.poll[i] = 0;
                    }
                }
            }
            else {
                // carry on with the same state
                processHandEvent(idx, 3, bodies[idx]->rightHandJoint, bodies[idx]->rightHandJoint.handState);
            }
            
            // refresh the update frame
            bodies[idx]->lastUpdateFrame = lastOscFrame;
		}
		else {
            bRecognized = false;
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
            ofLog() << msg_string;
		}
	}
    
    // remove any dead bodies
    vector<int> toRemove;
    for (map<int, k4w::Body *>::iterator it = bodies.begin(); it != bodies.end(); ++it) {
        if (ABS(it->second->lastUpdateFrame - lastOscFrame) > kNumFramesForRemoval) {
//            cout << "REMOVING idx " << it->first << " frame " << it->second->lastUpdateFrame << " vs " << lastOscFrame << endl;
            toRemove.push_back(it->first);
        }
    }
    for (int i = 0; i < toRemove.size(); i++) {
        delete bodies[toRemove[i]];
        bodies.erase(toRemove[i]);
    }
}

void CloudsInputKinectOSC::processHandEvent(int bodyIdx, int jointIdx, k4w::HandJoint& handJoint, k4w::HandState newState){    
    // temp mapping
    ofVec3f position2D(ofMap(handJoint.position.x, -1.0,  1.0, 0, ofGetWidth()),
                       ofMap(handJoint.position.y,  1.0, -1.0, 0, ofGetHeight()),
                       ofMap(handJoint.position.z, -1.0,  1.0, 0, ofGetWidth()));
    
	if (newState == k4w::HandState_Lasso) {
        if (handJoint.actionState == k4w::ActionState_Lasso) {
            // matching state: continue
            CloudsInteractionEventArgs args(position2D, jointIdx + k4w::ActionState_Lasso, bodyIdx);
//            cout << "DRAG " << args.playerId << " " << args.actionType << endl;
            ofNotifyEvent(getEvents().interactionDragged, args, this);
        }
        else if (handJoint.actionState == k4w::ActionState_Closed) {
            // state mismatch: end previous
            CloudsInteractionEventArgs args(position2D, jointIdx + k4w::ActionState_Closed, bodyIdx);
//            cout << "RELEASE " << args.playerId << " " << args.actionType << endl;
            ofNotifyEvent(getEvents().interactionEnded, args, this);
            handJoint.actionState = k4w::ActionState_Idle;
        }
        else {
            // idle state: start
            CloudsInteractionEventArgs args(position2D, jointIdx + k4w::ActionState_Lasso, bodyIdx);
//            cout << "PRESS " << args.playerId << " " << args.actionType << endl;
            ofNotifyEvent(getEvents().interactionStarted, args, this);
            handJoint.actionState = k4w::ActionState_Lasso;
        }
    }  
    else if (newState == k4w::HandState_Closed) {
        if (handJoint.actionState == k4w::ActionState_Closed) {
            // matching state: continue
            CloudsInteractionEventArgs args(position2D, jointIdx + k4w::ActionState_Closed, bodyIdx);
//            cout << "DRAG " << args.playerId << " " << args.actionType << endl;
            ofNotifyEvent(getEvents().interactionDragged, args, this);
        }
        else if (handJoint.actionState == k4w::ActionState_Lasso) {
            // state mismatch: end previous
            CloudsInteractionEventArgs args(position2D, jointIdx + k4w::ActionState_Lasso, bodyIdx);
//            cout << "RELEASE " << args.playerId << " " << args.actionType << endl;
            ofNotifyEvent(getEvents().interactionEnded, args, this);
            handJoint.actionState = k4w::ActionState_Idle;
        }
        else {
            // idle state: start
            CloudsInteractionEventArgs args(position2D, jointIdx + k4w::ActionState_Closed, bodyIdx);
//            cout << "PRESS " << args.playerId << " " << args.actionType << endl;
            ofNotifyEvent(getEvents().interactionStarted, args, this);
            handJoint.actionState = k4w::ActionState_Closed;
        }
    }
    else if (newState <= k4w::HandState_Open) {
        if (handJoint.actionState == k4w::ActionState_Idle) {
            // matching state: continue
            CloudsInteractionEventArgs args(position2D, jointIdx + k4w::ActionState_Idle, bodyIdx);
//            cout << "MOVE " << args.playerId << " " << args.actionType << endl;
            ofNotifyEvent(getEvents().interactionMoved, args, this);
        }
        else {
            // state mismatch: end previous
            CloudsInteractionEventArgs args(position2D, jointIdx + handJoint.actionState, bodyIdx);
//            cout << "RELEASE " << args.playerId << " " << args.actionType << endl;
            ofNotifyEvent(getEvents().interactionEnded, args, this);
            handJoint.actionState = k4w::ActionState_Idle;
        }
    }
}

