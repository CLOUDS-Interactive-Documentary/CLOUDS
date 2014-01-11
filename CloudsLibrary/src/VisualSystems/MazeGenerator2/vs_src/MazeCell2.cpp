//
//  Cell.cpp
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#include "MazeCell2.h"

MazeCell2::MazeCell2(int _x, int _y, float s, float t, float h)
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

void MazeCell2::draw(bool isGenerator)
{
    if (!visible) {
        return;
    }
    
    if (isGenerator)
    {
        ofFill();
        ofSetColor(ParamManager2::getInstance().getGeneratorColor());
        ofPushMatrix();
        ofTranslate(x*size+size/2, 0, y*size+size/2);
        ofScale(size, size, size);
        ofBox(1);
        ofPopMatrix();
    }
    
    ofFill();
    ofSetColor(ParamManager2::getInstance().getWallColor());

    if (top && bottom && right && left) {
//        ofPushMatrix();
//        ofTranslate(x*size+size/2, 0, y*size+size/2);
//        ofScale(size, size, size);
//        ofBox(1);
//        ofPopMatrix();
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