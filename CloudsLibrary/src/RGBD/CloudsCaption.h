#pragma once

#include "ofMain.h"
#include "CloudsClip.h"
#include "ofxUI.h"
#include "ofxFTGL.h"

class CloudsCaption
{
public:
	CloudsCaption();
    
	string caption;
	
	ofxFTGLFont* font;
	
	float charsPerSecond;
	
	void setup();
	
	float birthTime;
	float expandPercent;
	float selectPercent;
	
	//hack for now to make oculus q bigger
	float enlarge;
	
	void draw();
	void drawOverlay();
	void update();
    
protected:
    bool isSetup;
    
};
