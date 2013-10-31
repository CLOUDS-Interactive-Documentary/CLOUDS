//
//  Cell.cpp
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#include "MazeCell.h"

MazeCell::MazeCell(int _x, int _y, float s, float t, float h)
{
    x = _x;
    y = _y;
    size = s;
    wallThickness = t;
    wallHeight = h;
    
    wallLength = size + wallThickness - 0.1;
    
    top = left = right = bottom = true;
    mazeExit = searchStart = visited = searched = false;
    
    visible = true;
}

void MazeCell::draw(bool isGenerator)
{
    if (!visible) {
        return;
    }
    
    if (isGenerator)
    {
        ofFill();
        ofSetColor(ParamManager::getInstance().getGeneratorColor());
        ofPushMatrix();
        ofTranslate(x*size+size/2, 0, y*size+size/2);
        ofScale(size, size, size);
        ofBox(1);
        ofPopMatrix();
    }
    
    ofFill();
    ofSetColor(ParamManager::getInstance().getWallColor());

    if (top && bottom && right && left) {
        ofPushMatrix();
        ofTranslate(x*size+size/2, 0, y*size+size/2);
        ofScale(size, size, size);
        ofBox(1);
        ofPopMatrix();
        return;
    }
    
    if (top) {
        ofPushMatrix();
        ofTranslate(x*size+size/2, 0, y*size);
        ofScale(wallLength, wallHeight, wallThickness);
        ofBox(1);
        ofPopMatrix();
    }
    if (right) {
        ofPushMatrix();
        ofTranslate((x+1)*size, 0, y*size+size/2);
        ofRotateY(90);
        ofScale(wallLength, wallHeight, wallThickness);
        ofBox(1);
        ofPopMatrix();
    }
    
    if (x==0) {
        ofPushMatrix();
        ofTranslate(x*size, 0, y*size + size/2);
        ofRotateY(90);
        ofScale(wallLength, wallHeight, wallThickness);
        ofBox(1);
        ofPopMatrix();
    }

//    if (bottom) {
//        ofPushMatrix();
//        ofTranslate(x*size+size/2, 0, (y+1)*size);
//        ofScale(wallLength, wallHeight, wallThickness);
//        ofBox(1);
//        ofPopMatrix();
//    }
}

size_t MazeCell::getVertexCount()
{
    int verts=0;
    
    if (top) {
        verts += 8;
    }
    if (right) {
        verts += 8;
    }
    
    return verts;
}

size_t MazeCell::getFaceCount()
{
    int faces=0;
    
    if (top) {
        faces += 10;
    }
    if (right) {
        faces += 10;
    }
    
    return faces;
}

size_t MazeCell::fillVertexData(ofVec3f *data, int index)
{
    int startIndex = index;

    float northH = 0;
    if (top) {
        northH = wallHeight;
        // north wall bottom
        data[index++] = ofVec3f(x*size, 0, y*size);
        data[index++] = ofVec3f(x*size+size, 0, y*size);
        data[index++] = ofVec3f(x*size, 0, y*size-wallThickness);
        data[index++] = ofVec3f(x*size+size, 0, y*size-wallThickness);
        // north wall top
        data[index++] = ofVec3f(x*size, northH, y*size);
        data[index++] = ofVec3f(x*size+size, northH, y*size);
        data[index++] = ofVec3f(x*size, northH, y*size-wallThickness);
        data[index++] = ofVec3f(x*size+size, northH, y*size-wallThickness);
        
    }
    float eastH = 0;
    if (right) {
        eastH = wallHeight;
        // east wall bottom
        data[index++] = ofVec3f(x*size+size-wallThickness, 0, y*size+size);
        data[index++] = ofVec3f(x*size+size, 0, y*size+size);
        data[index++] = ofVec3f(x*size+size-wallThickness, 0, y*size);
        data[index++] = ofVec3f(x*size+size, 0, y*size);
        // east wall top
        data[index++] = ofVec3f(x*size+size-wallThickness, eastH, y*size+size);
        data[index++] = ofVec3f(x*size+size, eastH, y*size+size);
        data[index++] = ofVec3f(x*size+size-wallThickness, eastH, y*size);
        data[index++] = ofVec3f(x*size+size, eastH, y*size);
    }
    
    
    return index-startIndex;
}

size_t MazeCell::fillIndexData(ofIndexType *data, int vertStartIndex, int index)
{
    int startIndex = index;
    ofIndexType wallIndexes[] =
                        {4, 5, 6, 6, 5, 7,  // top
                        5, 4, 1, 1, 4, 0,   // north
                        7, 5, 3, 3, 5, 1,   // east
                        6, 7, 2, 2, 7, 3,   // south
                        4, 6, 0, 0, 6, 2};  // west
    
    // north wall
    if (top) {
        for (int i=0; i<sizeof(wallIndexes)/sizeof(ofIndexType); i++)
        {
            data[index++] = vertStartIndex + wallIndexes[i];
        }
        vertStartIndex += 8;
    }
    
    // east wall
    if (right)
    {
        for (int i=0; i<sizeof(wallIndexes)/sizeof(ofIndexType); i++)
        {
            data[index++] = vertStartIndex + wallIndexes[i];
        }
    }

    return index-startIndex;
}

size_t MazeCell::fillNormalData(ofVec3f *data, int index)
{
    int startIndex = index;
    ofVec3f normals[] = {ofVec3f(0, 1, 0),
                         ofVec3f(0, 1, 0),
                         ofVec3f(0, 0, 1),
                         ofVec3f(0, 0, 1),
                         ofVec3f(1, 0, 0),
                         ofVec3f(1, 0, 0),
                         ofVec3f(0, 0, -1),
                         ofVec3f(0, 0, -1),
                         ofVec3f(-1, 0, 0),
                         ofVec3f(-1, 0, 0)};

    if (top) {
        for (int i=0; i<10; i++) {
            data[index++] = normals[i];
        }
    }
    
    if (right) {
        for (int i=0; i<10; i++) {
            data[index++] = normals[i];
        }
    }
    
    return index-startIndex;
}


