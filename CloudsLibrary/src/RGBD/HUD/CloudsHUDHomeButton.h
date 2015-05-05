//
//  CloudsHUDHomeButton.h
//  CloudsHUDDesigner
//
//  Created by James George on 12/14/13.
//
//

#pragma once

#include "ofMain.h"
#include "CloudsInput.h"

typedef enum {
    CLOUDS_HOME_IDLE = 0,
    CLOUDS_HOME_HOVER_ON,
    CLOUDS_HOME_HOVER_LOOP_BACK,
    CLOUDS_HOME_ACTIVATING,
    CLOUDS_HOME_ACTIVE,
    CLOUDS_HOME_DEACTIVATING
} CloudsHUDHomeState;

class CloudsHUDHomeButton {
  public:
	CloudsHUDHomeButton();
	
	void setup();
	void update();
	
	void draw();
	
    bool hitTest( float xPos, float yPos );
    bool hitTest( ofPoint mousePos );
    
    void activate();
    void deactivate();
    bool wasActivated();
    
    ofRectangle bounds;
    ofRectangle interactiveBounds;

    bool bAnimateHoverRadar;
    float animatedHoverStartTime;
    float bleepAlpha;
    float bleepExpand;
    
  protected:
    void rollover();
    void rollout();
    void changeState(CloudsHUDHomeState newState);
    void queueState(CloudsHUDHomeState newState);
    
    map<CloudsHUDHomeState, vector<ofPixels> > rolloverPix;
    void loadFramesDir( string dirPath, vector<ofPixels>& pixels );
    
    int     playhead;
    float   targetFps;
    
    bool    bHasNextState;
    float   stateChangedTime;
    CloudsHUDHomeState  currentState;
    CloudsHUDHomeState  nextState;
    CloudsHUDHomeState  lastState;
    
    bool    bShowIdle;
    bool    bIsHovering;
    bool    bWasActivated;
    
	//float   maxHoverTime;
	//float   hoverStartTime;
    ofImage currentImage;
    
    
};