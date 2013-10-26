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
        ofSetColor(255, 0, 0);
        ofPushMatrix();
        ofTranslate(x*size+size/2, 0, y*size+size/2);
        ofScale(size, size, size);
        ofBox(1);
        ofPopMatrix();
    }
    
    ofFill();
    ofSetColor(ofColor::fromHsb(
        ParamManager::getInstance().wallColor.r,
        ParamManager::getInstance().wallColor.g,
        ParamManager::getInstance().wallColor.b));

    if (top && bottom && right && left) {
        ofPushMatrix();
        ofTranslate(x*size+size/2, 0, y*size+size/2);
        ofScale(size, size, size);
        ofBox(1);
        ofPopMatrix();
        return;
    }
    
    float length = size + wallThickness - 0.1;
    
    if (top) {
        ofPushMatrix();
        ofTranslate(x*size+size/2, 0, y*size);
        ofScale(length, wallHeight, wallThickness);
        ofBox(1);
        ofPopMatrix();
    }
    if (right) {
        ofPushMatrix();
        ofTranslate((x+1)*size, 0, y*size+size/2);
        ofRotateY(90);
        ofScale(length, wallHeight, wallThickness);
        ofBox(1);
        ofPopMatrix();
    }
    if (left) {
        ofPushMatrix();
        ofTranslate(x*size, 0, y*size + size/2);
        ofRotateY(90);
        ofScale(length, wallHeight, wallThickness);
        ofBox(1);
        ofPopMatrix();
    }
    if (bottom) {
        ofPushMatrix();
        ofTranslate(x*size+size/2, 0, (y+1)*size);
        ofScale(length, wallHeight, wallThickness);
        ofBox(1);
        ofPopMatrix();
    }
    
//    if (searched && (stack.search(this) > -1 )) {
//        fill(20);
//        ellipse((lx+rx)/2,(ty+by)/2,s/3,s/3);
//    }
}