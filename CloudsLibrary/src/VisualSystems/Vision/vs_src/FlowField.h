//
//  FlowField.h
//  yearLong
//
//  Created by Surya on 01/04/13.
//
//

#ifndef __yearLong__FlowField__
#define __yearLong__FlowField__
#define ROWS 3
#define COLS 3

#include <iostream>
#include "ofMain.h"
class FlowField{
public:
    FlowField();
    FlowField(int _resolution, int _w, int _h);
    void init();
    ofVec2f lookUp(ofVec2f lookup);
    int resolution;
    int width;
    int height;
    int cols;
    int rows;
    
   // ofVec2f[ROWS][COLS] field;
    vector<vector<ofVec2f> > field;
    
};
#endif /* defined(__yearLong__FlowField__) */
