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
	
    bool hitTest( float xPos, float yPos );
    bool hitTest( ofPoint mousePos );
    
    void activate();

    bool wasActivated();
    
    ofRectangle bounds;
    ofRectangle interactiveBounds;

  protected:
    void rollover();
    void rollout();
    
    void loadFramesDir( string dirPath, vector<ofPixels>& pixels );
    
    float   playhead;
    float   targetFps;
    
    CloudsHUDHomeState  currentState;
    
    bool    bShowIdle;
    bool    bIsHovering;
    bool    bWasActivated;
    
	float maxHoverTime;
	float hoverStartTime;
    ofImage currentImage;
    //map<CloudsHUDHomeState, vector<ofTexture*> >    rolloverTextures;
    map<CloudsHUDHomeState, vector<ofPixels> >    rolloverPix;
    
};