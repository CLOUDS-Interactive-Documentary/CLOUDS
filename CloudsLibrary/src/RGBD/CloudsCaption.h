#pragma once

#include "ofMain.h"
//#include "CloudsClip.h"
//#include "ofxUI.h"
#include "ofxFTGL.h"

class CloudsCaption
{
public:
	CloudsCaption();
    
	string caption;
	
	ofxFTGLFont* font;
	
	float charsPerSecond;
	
	void setup();
    void begin();
	
	float birthTime;
    float deathTime;
    float lifeDuration;
    float deathDuration;
    
	float expandPercent;
	float selectPercent;
	
	
	void draw();
	void drawOverlay();
	void update();

    bool isPlaying;
    bool isEnabled;
        
protected:
    
};
