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

class CodePanel {
  public:
	CodePanel();
	void setup(string codeFile);
	
	void update();
	void draw(float screenHeight);
	//ofxFTGLSimpleLayout layout;
	ofxFTGLFont layout;
	ofBuffer initialBuffer;
	
	float offset;
	float scanSpeed;
	
	ofRectangle drawRect;
//	vector<string> lines;
	vector<SyntaxLine> syntaxLines;
	string code;
	
	float animationStartTime;
	float animationEndTime;

	float* outlineAlpha;
	int maxCharsOnLine;
	bool drawAsHist;
protected:
	
	ofColor randomBoxColor;
	void myReplace(string& str, const string& oldStr, const string& newStr);
	int indexOf(string s, string f, int startIndex);
	
};