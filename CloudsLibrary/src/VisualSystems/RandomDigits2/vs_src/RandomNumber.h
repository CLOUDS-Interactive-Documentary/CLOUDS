//
//  RandomNumber.h
//  RandomDigits2
//
//  Created by Gal Sasson on 1/6/14.
//
//

#ifndef __RandomDigits2__RandomNumber__
#define __RandomDigits2__RandomNumber__

#include <iostream>
#include <vector>
#include "ofMain.h"

class RandomNumber
{
public:
    RandomNumber(int _id, float x, float y, float z, int r, int i);
    unsigned int fillVertices(vector<ofVec3f> &verts);
    unsigned int fillIndices(vector<unsigned int> &indices);
    unsigned int fillUvs(vector<ofVec2f> &uvs);
    void fillColors(vector<ofFloatColor> &colors, ofFloatColor &dark, ofFloatColor &light);
    
    void update();
    void triggerCount();
    
    unsigned int id;
    ofVec3f pos;
    int row;
    int index;
    int indexInc;
    
    
    
};

#endif /* defined(__RandomDigits2__RandomNumber__) */
