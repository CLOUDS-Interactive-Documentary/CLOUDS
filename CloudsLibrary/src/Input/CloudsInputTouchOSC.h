//
//  CloudsInputTouchOSC.h
//  Ripples
//
//  Created by Elie Zananiri on 2014-03-24.
//
//

#pragma once

#include "ofxOsc.h"
#include "CloudsInput.h"

//--------------------------------------------------------------
enum TouchState
{
    TouchState_Up   = 0,
    TouchState_Down = 1,
    
    TouchState_Count
};

//--------------------------------------------------------------
class Touch
{
public:
    Touch(){
        activeFrames = 0;
        state = TouchState_Up;
        for (int i = 0; i < TouchState_Count; i++){
            poll[i] = 0;
        }
    }
    
    int idx;
    int lastUpdateFrame;
    int activeFrames;
    int poll[TouchState_Count];
    TouchState state;
    
    ofVec3f inputPosition;
    ofVec3f screenPosition;
};

//--------------------------------------------------------------
class CloudsInputTouchOSC : public CloudsInput
{
public:
    CloudsInputTouchOSC();
    
	virtual void enable();
	virtual void disable();
    
    void update(ofEventArgs& args);
    
    void mapCoords(Touch *touch);
    void processTouchEvent(int idx, Touch *touch, TouchState newState);
    
    ofxOscReceiver receiver;
    int lastOscFrame;
    int primaryIdx;
    
    // current position attributes
    float posResetLerpPct;
    float posSetLerpPct;
    float posSetInstantThreshold;
    unsigned long long posOutOfBoundsStart;
    int posOutOfBoundsDelay;
    
    map<int, Touch *> touches;
    
    static int kListenPort;
    static int kNumFramesForRemoval;
    static int kPollThreshold;
};

//--------------------------------------------------------------
void SetCloudsInputTouchOSC();
