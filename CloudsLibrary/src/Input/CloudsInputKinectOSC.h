//
//  CloudsInputKinectOSC.h
//  YellowTail
//
//  Created by Elie Zananiri on 12/8/13.
//  
//

#pragma once

#include "CloudsInput.h"

#include "ofxOsc.h"

namespace k4w 
{
    enum TrackingState 
    {
        TrackingState_NotTracked = 0,
        TrackingState_Inferred   = 1,
        TrackingState_Tracked    = 2
    };
    
    enum HandState 
    {
        HandState_Unknown    = 0,
        HandState_NotTracked = 1,
        HandState_Open       = 2,
        HandState_Closed     = 3,
        HandState_Lasso      = 4
    };
    
    enum JointType 
    {
        JointType_SpineBase     = 0,
        JointType_SpineMid      = 1,
        JointType_Neck          = 2,
        JointType_Head          = 3,
        JointType_ShoulderLeft	= 4,
        JointType_ElbowLeft     = 5,
        JointType_WristLeft     = 6,
        JointType_HandLeft      = 7,
        JointType_ShoulderRight	= 8,
        JointType_ElbowRight	= 9,
        JointType_WristRight	= 10,
        JointType_HandRight	    = 11,
        JointType_HipLeft       = 12,
        JointType_KneeLeft      = 13,
        JointType_AnkleLeft     = 14,
        JointType_FootLeft      = 15,
        JointType_HipRight      = 16,
        JointType_KneeRight     = 17,
        JointType_AnkleRight	= 18,
        JointType_FootRight     = 19,
        JointType_SpineShoulder	= 20,
        JointType_HandTipLeft	= 21,
        JointType_ThumbLeft     = 22,
        JointType_HandTipRight	= 23,
        JointType_ThumbRight	= 24,
        
        JointType_Count
    };
    
    struct Joint 
    {
        JointType type;
        TrackingState trackingState;
        ofVec3f position;
    };
    
    struct HandJoint : public Joint
    {
        HandState handState;
        bool bPressed;
    };
    
    class Body 
    {
    public:
        Body(){
            // set default joint attributes
            headJoint.type = JointType_Head;
            headJoint.trackingState = TrackingState_NotTracked;
            
            leftHandJoint.type = JointType_HandLeft;
            leftHandJoint.trackingState = TrackingState_NotTracked;
            leftHandJoint.handState = HandState_NotTracked;
            leftHandJoint.bPressed = false;
            
            rightHandJoint.type = JointType_HandLeft;
            rightHandJoint.trackingState = TrackingState_NotTracked;
            rightHandJoint.handState = HandState_NotTracked;
            rightHandJoint.bPressed = false;
        }
        
        int idx;
        
        int lastUpdateFrame;
        
        Joint headJoint;
        HandJoint leftHandJoint;
        HandJoint rightHandJoint;
    };
};

class CloudsInputKinectOSC : public CloudsInput
{
public:
	
	virtual void enable();
	virtual void disable();
    
    void update(ofEventArgs& args);
	
	void processHandEvent(int bodyIdx, k4w::HandJoint& handJoint, k4w::HandState newState);
    
    ofxOscReceiver receiver;
    int lastOscFrame;
    
    map<int, k4w::Body *> bodies;
};
