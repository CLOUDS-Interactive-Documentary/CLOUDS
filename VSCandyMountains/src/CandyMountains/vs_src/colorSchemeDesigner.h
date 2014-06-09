//
//  colorSchemeDesigner.h
//  colorSchemeDesigner
//
//  Created by Jason Levine on 2013-12-05.
//
//

#pragma once
#include "ofMain.h"
#include "ofxUI.h"


class colorSchemeDesigner {
public:
    void setup();
    void draw();
    void drawHueScheme(int scheme, int position);
    void guiEvent(ofxUIEventArgs &e);
    
    void addColorRef(ofFloatColor *col) { assignColors.push_back(col); }
	void assignRandom(bool unique);
    ofFloatColor getRandomColor();
	
    ofxUICanvas * gui;
    ofxUIDropDownList *ddl;
    
    ofFloatColor primary;
    
    float hue, saturation, brightness;
    float angle, distance;
    
    float width, xOffset;
    
    int mode;
    
    vector<vector<ofFloatColor> > colorScheme;
    
    vector<ofFloatColor*> assignColors;
	
	
	// ------------------------------------------------
	// !!!:motoi added
	void updateColorScheme();
	void applyColors();
	vector<ofVec2f> colorSchemeIndices;
	vector<int> applyColorIndices;
	
	
	// interfaces
	void setHue(const float value)
	{
		hue = value;
		updateColorScheme();
	}
	
	void setSaturation(const float value)
	{
		saturation = value;
		updateColorScheme();
	}
	
	void setBrightness(const float value)
	{
		brightness = value;
		updateColorScheme();
	}
	
	void setAngle(const float value)
	{
		angle = value;
		updateColorScheme();
	}
	
	void setDistance(const float value)
	{
		distance = value;
		updateColorScheme();
	}
};
