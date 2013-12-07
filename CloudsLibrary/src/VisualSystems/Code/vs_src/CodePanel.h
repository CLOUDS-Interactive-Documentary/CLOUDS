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
} SyntaxLine;

class CodePanel {
  public:
	CodePanel();
	void setup(string codeFile);
	
	void update();
	void draw();
	
	//ofxFTGLSimpleLayout layout;
	ofxFTGLFont layout;
	ofBuffer initialBuffer;
	ofBuffer leftToType;
	
	float offset;
	float scanSpeed;
	
	ofRectangle drawRect;
//	vector<string> lines;
	vector<SyntaxLine> syntaxLines;
	string code;
	
protected:
	
	void myReplace(string& str, const string& oldStr, const string& newStr);
	int indexOf(string s, string f, int startIndex);
	
};