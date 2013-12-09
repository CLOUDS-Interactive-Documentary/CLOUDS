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
            
            cout << "RECEIVED idx " << idx << " on frame " << lastOscFrame << endl;
            
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
            processHandEvent(bodies[idx]->leftHandJoint.handState, newHandState, idx, bodies[idx]->leftHandJoint.position);
            bodies[idx]->leftHandJoint.handState = newHandState;
            
            // update the right hand joint
            bodies[idx]->rightHandJoint.type = (k4w::JointType)m.getArgAsInt32(i++);
            bodies[idx]->rightHandJoint.trackingState = (k4w::TrackingState)m.getArgAsInt32(i++);
            bodies[idx]->rightHandJoint.position.set(m.getArgAsFloat(i++), 
                                                     m.getArgAsFloat(i++), 
                                                     m.getArgAsFloat(i++));
            newHandState = (k4w::HandState)m.getArgAsInt32(i++);
            processHandEvent(bodies[idx]->rightHandJoint.handState, newHandState, idx, bodies[idx]->rightHandJoint.position);
            bodies[idx]->rightHandJoint.handState = newHandState;
            
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
            cout << "REMOVING idx " << it->first << " frame " << it->second->lastUpdateFrame << " vs " << lastOscFrame << endl;
            toRemove.push_back(it->first);
        }
    }
    for (int i = 0; i < toRemove.size(); i++) {
        delete bodies[toRemove[i]];
        bodies.erase(toRemove[i]);
    }
}

void CloudsInputKinectOSC::processHandEvent(k4w::HandState prevState, k4w::HandState currState, int bodyIdx, ofVec3f& position){
    cout << "PREV STATE: " << prevState << " // CURR STATE: " << currState << endl; 
    
    // temp mapping
    ofVec3f position2D(ofMap(position.x, -1.0, 1.0, 0, ofGetWidth()),
                       ofMap(position.y, -1.0, 1.0, 0, ofGetHeight()),
                       ofMap(position.z, -1.0, 1.0, 0, ofGetWidth()));

	if (prevState == k4w::HandState_Open && currState == k4w::HandState_Lasso) {
        interactionStarted(position2D, 0, bodyIdx);
    }    
    else if (prevState == k4w::HandState_Open && currState == k4w::HandState_Closed) {
        interactionStarted(position2D, 2, bodyIdx);
    }
	
    else if (prevState == k4w::HandState_Lasso && currState == k4w::HandState_Open) {
        interactionEnded(position2D, 0, bodyIdx);
    }
    else if (prevState == k4w::HandState_Closed && currState == k4w::HandState_Open) {
        interactionEnded(position2D, 2, bodyIdx);
    }
    
    else if (currState == k4w::HandState_Lasso) {
        interactionDragged(position2D, 0, bodyIdx);
    }    
    else if (currState == k4w::HandState_Closed) {
        interactionDragged(position2D, 2, bodyIdx);
    }
    else if (currState == k4w::HandState_Open) {
        interactionMoved(position2D, 0, bodyIdx);
    }
}

