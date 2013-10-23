//
//  ofxMtlBoxFitting.cpp
//  openFrameworks
//
//  Created by Hugues Bruyere on 2009-09-21.
//  Based on David Bollinger's ofxMtlBoxFitting for Processing, July 2006
//

#include "ofxMtlBoxFitting.h"

//--------------------------------------------------------------
ofxMtlBoxFitting::ofxMtlBoxFitting()
{
	nbOfBox = 0;
	
	nbOfColors = 18.0f;
	
	ofColor nColor;
	nColor.r = 86.0f; nColor.g = 113.0f; nColor.b = 141.0f;
	colors.push_back(nColor);
	nColor.r = 126.0f; nColor.g = 172.0f; nColor.b = 205.0f;
	colors.push_back(nColor);
	nColor.r = 188.0f; nColor.g = 26.0f; nColor.b = 85.0f;
	colors.push_back(nColor);
	nColor.r = 251.0f; nColor.g = 204.0f; nColor.b = 219.0f;
	colors.push_back(nColor);
	nColor.r = 213.0f; nColor.g = 239.0f; nColor.b = 253.0f;
	colors.push_back(nColor);
	nColor.r = 197.0f; nColor.g = 194.0f; nColor.b = 194.0f;
	colors.push_back(nColor);
	
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	nColor.r = 255.0f; nColor.g = 255.0f; nColor.b = 255.0f;
	colors.push_back(nColor);
	
	//
	nbOfWhitischColors = 10.0f;
	nColor.r = 86.0f; nColor.g = 113.0f; nColor.b = 141.0f;
	colorsWhitisch.push_back(nColor);
}

//--------------------------------------------------------------
ofxMtlBoxFitting::~ofxMtlBoxFitting(){
	
}

//--------------------------------------------------------------
void ofxMtlBoxFitting::setup(int _wid, int _hei, int _divx, int _divy, int _maxsizer) {
	wid = _wid;
	hei = _hei;
	// width AND height should be evenly divisible by divx/y
	divx = _divx;
	divy = _divy;
	maxsizer = _maxsizer; // largest box is (maxsizer * max(divx,divy)) pixels
	cols = wid / divx;
	rows = hei / divy;
	// sanity checks
	if (maxsizer*divx > wid) maxsizer = wid/divx;
	if (maxsizer*divy > hei) maxsizer = hei/divy;
	// further sanity check? hopefully not necessary:
	// if ((rows>0xffff)||(cols>0xffff)) then too big for current encoding
	// allocate
	cells = new int*[rows];
	for( int i = 0 ; i < rows ; i++ ) {
		cells[i] = new int [cols];
	}
}

//--------------------------------------------------------------
void ofxMtlBoxFitting::wipe() {
	for (int r=0; r<rows; r++)
		for (int c=0; c<cols; c++)
			cells[r][c] = 0;
	col = row = 0;
}

//--------------------------------------------------------------
bool ofxMtlBoxFitting::isOccupied(int col, int row, int w, int h) {
	for (int r=row; r<row+h; r++) {
		for (int c=col; c<col+w; c++) {
			if (cells[r][c] != 0) {
				return true;
			}
		}
	}
	return false;
}

//--------------------------------------------------------------
void ofxMtlBoxFitting::doOccupy(int col, int row, int w, int h, int value) {
	for (int r=row; r<row+h; r++) {
		for (int c=col; c<col+w; c++) {
			cells[r][c] = value;
		}
	}
}

//--------------------------------------------------------------
void ofxMtlBoxFitting::make(int s) {
	seed = s;
	//ofSeedRandom(seed);
	wipe();
	for (int r=0; r<rows; r++) {
		for (int c=0; c<cols; c++) {
			int cell = cells[r][c];
			if (cell != 0) continue;
			// figure out the size of area to occupy
			int sizer, limit;
			do {
				limit = MIN(cols-c, rows-r);
				limit = MIN(limit, maxsizer);
				sizer = int(ofRandom(0, limit))+1;
			} while(isOccupied(c,r,sizer,sizer));
			// flag all cells as occupied by width x height area
			doOccupy(c,r,sizer,sizer,sizer);
			// indicate work to perform in upper-left cell
			int work = WORK_DONE;
			cells[r][c] |= (work<<16);
		} // for c
	} // for r
}

//--------------------------------------------------------------
void ofxMtlBoxFitting::generate(int s) {
	make(s);
	
	boxes.clear();
	
	nbOfBox = 0;
	int counter = 0;
	do {
		int cell = cells[row][col];
		int work = (cell>>16) & 0xff;
		int sizer = (cell) & 0xffff;
		if (work != WORK_NONE) {
			y1 = row*divy;
			x1 = col*divx;
			w = sizer * divx;
			h = sizer * divy;
			
			
			//Store box specs.
			ofxMtlBox box;
			box.x = x1;
			box.y = y1;
			box.w = w;
			box.h = h;
			
			float area = w * h;
			float maxarea = (maxsizer*divx) * (maxsizer*divy);
			//box.r = 64.0 + (area / maxarea) * ofRandom(0.0f, 128.0f);
			//box.g = 64.0 + (area / maxarea) * ofRandom(0.0f, 128.0f);
			//box.b = 64.0 + (area / maxarea) * ofRandom(0.0f, 128.0f);
			int randomColor = int(round(ofRandom(0.0f, nbOfColors)));
			box.r = colors[randomColor].r;
			box.g = colors[randomColor].g;
			box.b = colors[randomColor].b;
			
			
			
			nbOfBox++;
			boxes.push_back(box);
			
		}
		advance(sizer);
		counter += sizer;
		if (at00()) return;
	} while (counter < (rows*cols));
	
}

//--------------------------------------------------------------
bool ofxMtlBoxFitting::at00() {
	return ((col==0) && (row==0));
}

//--------------------------------------------------------------
void ofxMtlBoxFitting::advance(int advcol) {
	col += advcol;
	if (col >= cols) {
		col = 0;
		row++;
		if (row >= rows) {
			row = 0;
		}
	}
}

//--------------------------------------------------------------
void ofxMtlBoxFitting::render(int x1, int y1, int w, int h) {
	/*
     float area = w * h;
     float maxarea = (maxsizer*divx) * (maxsizer*divy);
     float r = 64.0 + (area / maxarea) * ofRandom(0.0f, 128.0f);
     float g = 64.0 + (area / maxarea) * ofRandom(0.0f, 128.0f);
     float b = 64.0 + (area / maxarea) * ofRandom(0.0f, 128.0f);
     */
	
	float r = 255.0f;
	float g = 255.0f;
	float b = 255.0f;
	
	ofPushStyle();
    //Inside
    ofFill();
    ofSetColor(r,g,b);
    ofRect(x1,y1,w,h);
    
    //Edge
    ofNoFill();
    ofSetColor(0,0,0);
    ofRect(x1,y1,w,h);
	ofPopStyle();
}

//--------------------------------------------------------------
void ofxMtlBoxFitting::drawall() {
	nbOfBox = 0;
	int counter = 0;
	do {
		int cell = cells[row][col];
		int work = (cell>>16) & 0xff;
		int sizer = (cell) & 0xffff;
		if (work != WORK_NONE) {
			y1 = row*divy;
			x1 = col*divx;
			w = sizer * divx;
			h = sizer * divy;
			
			nbOfBox++;
			
			render(x1,y1,w,h);
		}
		advance(sizer);
		counter += sizer;
		if (at00()) return;
	} while (counter < (rows*cols));
}


//--------------------------------------------------------------
void ofxMtlBoxFitting::drawone() {
	nbOfBox = -1;
	bool drawn = false;
	do {
		int cell = cells[row][col];
		int work = (cell>>16) & 0xff;
		int sizer = (cell) & 0xffff;
		if (work != WORK_NONE) {
			y1 = row*divy;
			x1 = col*divx;
			w = sizer * divx;
			h = sizer * divy;
			render(x1,y1,w,h);
			drawn = true;
		}
		advance(sizer);
		if (at00()) return;
	} while (!drawn);
}
