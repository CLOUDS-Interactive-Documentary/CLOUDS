//
//  CodePanel.h
//  Code
//
//  Created by James George on 12/1/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxFTGL.h"

#define MATCH_TYPES 5

typedef struct {
	string baseLine;
	string colored[MATCH_TYPES];
	map<unsigned char,int> charCounts;
	
} SyntaxLine;


class Panel {
  public:
	Panel();
	void setup();
	void startAnimation();
	
	void update();
	void draw(float screenHeight);
	
	virtual void selfSetup();
	virtual void selfUpdate();
	virtual void selfDraw();
	
	ofBuffer initialBuffer;
	
	float offset;
	float scanSpeed;
	
	string dataPath;
	
	ofRectangle drawRect;
	string code;
	vector<string> lines;
	vector<SyntaxLine> syntaxLines;

	float animationStartTime;
	float animationEndTime;

	float* outlineAlpha;
	int maxCharsOnLine;
	
	ofFloatColor tint;
	vector<ofFloatColor> matchColorTypes;
	
	ofxFTGLFont* sharedFont;
	ofxFTGLSimpleLayout* sharedLayout;
  protected:
	void myReplace(string& str, const string& oldStr, const string& newStr);
	int indexOf(string s, string f, int startIndex);

};