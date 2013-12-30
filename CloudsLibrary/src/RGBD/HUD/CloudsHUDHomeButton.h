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
    CLOUDS_HUD_HOVER_IDLE = 0,
    CLOUDS_HUD_HOVER_ROLLOVER,
    CLOUDS_HUD_HOVER_OUTRO,
    CLOUDS_HUD_HOVER_ACTIVE_INTRO,
    CLOUDS_HUD_HOVER_ACTIVE
} CloudsHUDHomeState;

class CloudsHUDHomeButton {
  public:
	CloudsHUDHomeButton();
	
	void setup();
	void update();
	
	void draw();
	
	bool isActive();
    bool hitTest( float xPos, float yPos );
    bool hitTest( ofPoint mousePos );
    
	ofVec2f homeLocation;

  protected:
    void rollover();
    void rollout();
    
    vector<ofTexture*>  loadFramesDir( string dirPath );
    
	ofRectangle hitBox;
    ofVec2f     buttonSize;
    
    float   playhead;
    float   targetFps;
    
    CloudsHUDHomeState  currentState;
    
    bool    bIsHovering;
    
	float maxHoverTime;
	float hoverStartTime;
    
    vector<ofTexture*>  rolloverFrames;
    vector<ofTexture*>  rolloverOutFrames;
    vector<ofTexture*>  activeIntroFrames;
    vector<ofTexture*>  activeLoopFrames;
};