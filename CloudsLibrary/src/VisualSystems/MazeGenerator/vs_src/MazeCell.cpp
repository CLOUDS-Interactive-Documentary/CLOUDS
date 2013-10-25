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
    
    // bottom
//    wall.addVertex(ofVec3f(-1, 0, 1));
//    wall.addVertex(ofVec3f(size/2, 0, wallThickness/2));
//    wall.addVertex(ofVec3f(size/2, 0, -wallThickness/2));
//    wall.addVertex(ofVec3f(-size/2, 0, -wallThickness/2));
//    
//    // top
//    wall.addVertex(ofVec3f(-size/2, wallHeight, wallThickness/2));
//    wall.addVertex(ofVec3f(size/2, wallHeight, wallThickness/2));
//    wall.addVertex(ofVec3f(size/2, wallHeight, -wallThickness/2));
//    wall.addVertex(ofVec3f(-size/2, wallHeight, -wallThickness/2));
//    
//    wall.addTriangle(0, 1, 2); // bottom
//    wall.addTriangle(2, 3, 0);
    
    
    
}

void MazeCell::draw()
{
    float lx = x*size;
    float rx = x*size+size;
    float ty = y*size;
    float by = y*size+size;
    
//    ofRect(lx,ty,size,size);
    
    ofFill();
    ofSetColor(200);
    
    if (top) {
        ofPushMatrix();
        ofTranslate(x*size+size/2, 0, y*size);
        ofScale(size, wallHeight, wallThickness);
        ofBox(1);
        ofPopMatrix();
    }
    if (right) {
        ofPushMatrix();
        ofTranslate((x+1)*size, 0, y*size+size/2);
        ofRotateY(90);
        ofScale(size, wallHeight, wallThickness);
        ofBox(1);
        ofPopMatrix();
    }
    if (left) {
        ofPushMatrix();
        ofTranslate(x*size, 0, y*size + size/2);
        ofRotateY(90);
        ofScale(size, wallHeight, wallThickness);
        ofBox(1);
        ofPopMatrix();
    }
    if (bottom) {
        ofPushMatrix();
        ofTranslate(x*size+size/2, 0, (y+1)*size);
        ofScale(size, wallHeight, wallThickness);
        ofBox(1);
        ofPopMatrix();
    }
    
//    if (searched && (stack.search(this) > -1 )) {
//        fill(20);
//        ellipse((lx+rx)/2,(ty+by)/2,s/3,s/3);
//    }
}