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
    CLOUDS_HUD_HOVER_ACTIVE_LOOP
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
    
    bool wasHomeOpened(){
        if( bWasOpened ){
            bWasOpened = false;
            return true;
        }
        return false;
    }
    
	ofVec2f homeLocation;
    ofRectangle bounds;

  protected:
    void rollover();
    void rollout();
    
    vector<ofTexture*>  loadFramesDir( string dirPath );
    
    ofVec2f     buttonSize;
    
    float   playhead;
    float   targetFps;
    float   lastFrameTime;
    
    CloudsHUDHomeState  currentState;
    
    bool    bIsHovering;
    bool    bWasOpened;
    
	float maxHoverTime;
	float hoverStartTime;
    
    map<CloudsHUDHomeState, vector<ofTexture*> >    rolloverTextures;
};