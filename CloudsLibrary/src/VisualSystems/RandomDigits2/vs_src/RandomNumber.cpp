//
//  RandomNumber.cpp
//  RandomDigits2
//
//  Created by Gal Sasson on 1/6/14.
//
//

#include "RandomNumber.h"

RandomNumber::RandomNumber(int _id, float x, float y, float z, int r, int i)
{
    id = _id;
    pos = ofVec3f(x, y, z);
    row = r;
    index = i;
    
    indexInc = 0;
}

unsigned int RandomNumber::fillVertices(vector<ofVec3f> &verts)
{
    verts.push_back(pos + ofVec3f(-18, 5, 0));
    verts.push_back(pos + ofVec3f(18, 5, 0));
    verts.push_back(pos + ofVec3f(-18, -5, 0));
    verts.push_back(pos + ofVec3f(18, -5, 0));
    return 4;
}

unsigned int RandomNumber::fillIndices(vector<unsigned int> &indices)
{
    unsigned int start = id*4;
    
    indices.push_back(start+0);
    indices.push_back(start+1);
    indices.push_back(start+2);
    indices.push_back(start+2);
    indices.push_back(start+1);
    indices.push_back(start+3);
    
    return 6;
}

unsigned int RandomNumber::fillUvs(vector<ofVec2f> &uvs)
{
    uvs.push_back(ofVec2f(index*36, row*10+1));
    uvs.push_back(ofVec2f((index+1)*36-1, row*10+1));
    uvs.push_back(ofVec2f(index*36, (row+1)*10+1));
    uvs.push_back(ofVec2f((index+1)*36-1, (row+1)*10+1));
    
    return 4;
}

void RandomNumber::fillColors(vector<ofFloatColor> &colors, ofFloatColor &dark, ofFloatColor &light)
{
    if (indexInc != 0) {
        colors.push_back(light);
        colors.push_back(light);
        colors.push_back(light);
        colors.push_back(light);
    }
    else {
        colors.push_back(dark);
        colors.push_back(dark);
        colors.push_back(dark);
        colors.push_back(dark);
    }
}

void RandomNumber::update()
{
    index += indexInc;
    if (index > 27) {
        index = 27;
        indexInc = 0;
    }
    else if (index < 0) {
        index = 0;
        indexInc = 0;
    }
}

void RandomNumber::triggerCount()
{
    if (index == 0) {
        indexInc = 1;
    }
    else {
        indexInc = -1;
    }
}