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
    // comes from K4W SDK
    enum TrackingState 
    {
        TrackingState_NotTracked = 0,
        TrackingState_Inferred   = 1,
        TrackingState_Tracked    = 2
    };
    
    // comes from K4W SDK
    // re-appropriated in special cases e.g. proximity mode or out of range hands
    enum HandState 
    {
        HandState_Unknown    = 0,
        HandState_NotTracked = 1,
        HandState_Open       = 2,
        HandState_Closed     = 3,
        HandState_Lasso      = 4,
        
        HandState_Count
    };
    
    // comes from K4W SDK
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
    
    // defined by us
    enum ActionState
    {
        ActionState_Inactive    = -1,
        ActionState_Idle        =  0,
        ActionState_Lasso       =  1,
        ActionState_Closed      =  2
    };
    
    struct Joint 
    {
        JointType type;
        TrackingState trackingState;
        ofVec3f inputPosition;   // input position from K4W SDK
        ofVec3f localPosition;   // position translated by origin
        ofVec3f screenPosition;  // position mapped to 2D viewport
    };
    
    struct HandJoint : public Joint
    {
        HandState handState;
    };
    
    class Body 
    {
    public:
        Body(){
            // set default joint attributes
            headJoint.type = JointType_Head;
            headJoint.trackingState = TrackingState_NotTracked;
            
            neckJoint.type = JointType_Neck;
            neckJoint.trackingState = TrackingState_NotTracked;
            
            spineShoulderJoint.type = JointType_SpineShoulder;
            spineShoulderJoint.trackingState = TrackingState_NotTracked;
            
            spineMidJoint.type = JointType_SpineMid;
            spineMidJoint.trackingState = TrackingState_NotTracked;
            
            spineBaseJoint.type = JointType_SpineBase;
            spineBaseJoint.trackingState = TrackingState_NotTracked;
            
            shoulderLeftJoint.type = JointType_ShoulderLeft;
            shoulderLeftJoint.trackingState = TrackingState_NotTracked;
            
            shoulderRightJoint.type = JointType_ShoulderRight;
            shoulderRightJoint.trackingState = TrackingState_NotTracked;
            
            age = 0;
        }
        
        int idx;
        int age;
        int lastUpdateFrame;
        Joint headJoint;
        Joint neckJoint;
        Joint spineShoulderJoint;
        Joint spineMidJoint;
        Joint spineBaseJoint;
        Joint shoulderLeftJoint;
        Joint shoulderRightJoint;
    };
    
    class Hand
    {
    public:
        Hand(){
            // set default joint attributes
            handJoint.type = JointType_HandLeft;
            handJoint.trackingState = TrackingState_NotTracked;
            handJoint.handState = HandState_NotTracked;
            
            activeFrames = 0;
            actionState = ActionState_Inactive;
            for (int i = 0; i < HandState_Count; i++){
                poll[i] = 0;
            }
        }
        
        int idx;
        int bodyIdx;
        int lastUpdateFrame;
        int activeFrames;
        int poll[HandState_Count];
        ActionState actionState;
        HandJoint handJoint;
        ofRectangle trackingBounds;
    };
};

class CloudsInputKinectOSC : public CloudsInput
{
public:
    CloudsInputKinectOSC(float activeThresholdY, float activeThresholdZ);
    
	virtual void enable();
	virtual void disable();
    
    void update(ofEventArgs& args);
    
    void mapCoords(k4w::Joint& joint, ofVec3f& origin, float zRef, float width, float height);
	void processHandEvent(int handIdx, k4w::Hand * hand, k4w::HandState newState);
    
    void debug(float x, float y, float width, float height);
    
    ofxOscReceiver receiver;
    int lastOscFrame;
    int primaryIdx;
    float activeThresholdY;
    float activeThresholdZ;
    ofVec3f boundsMin;
    ofVec3f boundsMax;
    float jointLerpPct;
    
    bool bDoDebug;
    
    // current position attributes
    float posResetLerpPct;
    float posSetLerpPct;
    float posSetInstantThreshold;
    unsigned long long posOutOfBoundsStart;
    int posOutOfBoundsDelay;
    
    map<int, k4w::Body *> bodies;
    map<int, k4w::Hand *> hands;
};

void SetCloudsInputKinect(float activeThresholdY = 0.8f, float activeThresholdZ = 0.4f);
