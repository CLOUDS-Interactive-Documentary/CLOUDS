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
    
//    if (x==0) {
//        ofPushMatrix();
//        ofTranslate(x*size, 0, y*size + size/2);
//        ofRotateY(90);
//        ofScale(wallLength, wallHeight, wallThickness);
//        ofBox(1);
//        ofPopMatrix();
//    }

    if (bottom) {
        ofPushMatrix();
        ofTranslate(x*size+size/2, 0, (y+1)*size);
        ofScale(wallLength, wallHeight, wallThickness);
        ofBox(1);
        ofPopMatrix();
    }
}

size_t MazeCell::addGeometry(vector<ofVec3f> &verts, vector<ofVec3f> &normals)
{
    ofVec3f wallPoints[] = {
        // north wall bottom
        ofVec3f(-size/2, 0, size/2),
        ofVec3f(size/2, 0, size/2),
        ofVec3f(-size/2, 0, size/2-wallThickness/2),
        ofVec3f(size/2, 0, size/2-wallThickness/2),
        
        // north wall top
        ofVec3f(-size/2, wallHeight, size/2),
        ofVec3f(size/2, wallHeight, size/2),
        ofVec3f(-size/2, wallHeight, size/2-wallThickness/2),
        ofVec3f(size/2, wallHeight, size/2-wallThickness/2),
    };
    
    ofVec3f wallNormals[] = {
        ofVec3f(0, 1, 0),
        ofVec3f(0, 0, 1),
        ofVec3f(1, 0, 0),
        ofVec3f(0, 0, -1),
        ofVec3f(-1, 0, 0)
    };
    
    short wallIndexes[5][6] = {
        {4, 5, 6, 6, 5, 7},  // top
        {5, 4, 1, 1, 4, 0},   // north
        {7, 5, 3, 3, 5, 1},   // east
        {6, 7, 2, 2, 7, 3},   // south
        {4, 6, 0, 0, 6, 2}};  // west

    int vertexCount = 0;
    
    ofMatrix4x4 translate = ofMatrix4x4::newTranslationMatrix(ofVec3f(x*size+size/2, 0, y*size+size/2));
    
    if (bottom) {
        for (int w=0; w<5; w++) {
            for (int i=0; i<6; i++) {
                normals.push_back(wallNormals[w]);
                verts.push_back(wallPoints[wallIndexes[w][i]] * translate);
                vertexCount++;
            }
        }
    }
    
    if (left) {
        ofMatrix4x4 rotate = ofMatrix4x4::newRotationMatrix(90, ofVec3f(0, 1, 0));
        for (int w=0; w<5; w++) {
            for (int i=0; i<6; i++) {
                normals.push_back(rotate * wallNormals[w]);
                verts.push_back(rotate * wallPoints[wallIndexes[w][i]] * translate);
                vertexCount++;
            }
        }
    }

//    if (top) {
//        ofMatrix4x4 rotate = ofMatrix4x4::newRotationMatrix(180, ofVec3f(0, 1, 0));
//        for (int w=0; w<5; w++) {
//            for (int i=0; i<6; i++) {
//                normals.push_back(rotate * wallNormals[w]);
//                verts.push_back(rotate * wallPoints[wallIndexes[w][i]] * translate);
//                vertexCount++;
//            }
//        }
//    }
//
//    if (right) {
//        ofMatrix4x4 rotate = ofMatrix4x4::newRotationMatrix(270, ofVec3f(0, 1, 0));
//        for (int w=0; w<5; w++) {
//            for (int i=0; i<6; i++) {
//                normals.push_back(rotate * wallNormals[w]);
//                verts.push_back(rotate * wallPoints[wallIndexes[w][i]] * translate);
//                vertexCount++;
//            }
//        }
//    }

    return vertexCount;
}

