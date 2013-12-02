//
//  CodePanel.h
//  Code
//
//  Created by James George on 12/1/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxFTGLSimpleLayout.h"

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
	vector<string> lines;
	string code;
};