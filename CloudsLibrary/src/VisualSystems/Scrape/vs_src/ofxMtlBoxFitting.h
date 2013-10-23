//
//  ofxMtlBoxFitting.h
//  openFrameworks
//
//  Created by Hugues Bruyere on 2009-09-21.
//  Based on David Bollinger's BoxFitter for Processing, July 2006
//

#pragma once

#include "ofMain.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
struct ofxMtlBox
{
	int x;
	int y;
	int w;
	int h;
	float r, g, b, a;
};

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlBoxFitting {
	
public:
	
	ofxMtlBoxFitting();
	~ofxMtlBoxFitting();
	
	void setup(int _wid, int _hei, int _divx, int _divy, int _maxsizer);
	
	// reset the pattern grid
	void wipe();
	
	// determines if cells already contain defined area(s)
	bool isOccupied(int col, int row, int w, int h);
	
	// marks cells as containing an area
	void doOccupy(int col, int row, int w, int h, int value);
	
	// define the pattern grid
	void make(int s);
	
	// define the pattern grid and generate the specs of each boxes.
	void generate(int s);
	
	// is cursor at 0,0
	bool at00();
	
	// step the cursor forward
	void advance(int advcol);
	
	// this would typically be overridden
	void render(int x1, int y1, int w, int h);
	
	// advance through the pattern and draw the next box
	void drawone();
	
	// draw all the boxes at once
	void drawall();
	
	int wid, hei;
	int cols, rows;
	int col, row;
	int divx, divy;
	int maxsizer;
	int seed;
	
	int nbOfBox;
	vector<ofxMtlBox> boxes;
	
	int x1;
	int y1;
	
	int w;
	int h;
	
	// cells store the box fit pattern, contents are byte encoded as:
	// (cell >> 16) & 0xff == work to do
	// (cell) & 0xffff == size of allocated area
	int** cells;
	//int cells[][];
	// the types of work that may occur in a cell
	static const int WORK_NONE = 0;
	static const int WORK_DONE = 1;
	
	vector<ofColor> colorsWhitisch;
	float nbOfWhitischColors;
	
	vector<ofColor> colors;
	float nbOfColors;
};
