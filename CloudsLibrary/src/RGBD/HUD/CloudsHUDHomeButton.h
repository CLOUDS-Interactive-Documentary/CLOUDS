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

class CloudsHUDHomeButton {
  public:
	CloudsHUDHomeButton();
	
	void setup();
	void update();
	
	void draw();
	
	bool isActive();
    bool hitTest( float xPos, float yPos );
    bool hitTest( ofPoint mousePos );
    
    void playIdle();
    void playHover();
    void playLoop();
    
	ofVec2f homeLocation;

  protected:
	ofRectangle hitBox;
    
    float   playhead;
    float   targetFps;
    
    bool    bIsHovering;
    
	float maxHoverTime;
	float hoverStartTime;
    float targetButtonHeight;
    
    int IDLE_FRAME;
    int LOOP_START;
    int LOOP_END;
    
    
    vector<ofTexture*>  textureList;
};