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

size_t MazeCell::addGeometry(vector<ofVec3f> &verts, vector<ofVec3f> &normals, vector<ofVec2f> &uvs)
{
    float r = ofRandomf()*0.1f;
    ofVec3f wallPoints[] = {
        // north wall bottom
        ofVec3f(-size/2, 0, size/2+wallThickness/2+r),
        ofVec3f(size/2, 0, size/2+wallThickness/2+r),
        ofVec3f(-size/2, 0, size/2-wallThickness/2+r),
        ofVec3f(size/2, 0, size/2-wallThickness/2+r),
        
        // north wall top
        ofVec3f(-size/2, wallHeight+r, size/2+wallThickness/2+r),
        ofVec3f(size/2, wallHeight+r, size/2+wallThickness/2+r),
        ofVec3f(-size/2, wallHeight+r, size/2-wallThickness/2+r),
        ofVec3f(size/2, wallHeight+r, size/2-wallThickness/2+r),
    };
    
    r = ofRandomf()*0.1f;
    ofVec3f wallPoints2[] = {
        // north wall bottom
        ofVec3f(-size/2, 0, size/2+wallThickness/2+r),
        ofVec3f(size/2, 0, size/2+wallThickness/2+r),
        ofVec3f(-size/2, 0, size/2-wallThickness/2+r),
        ofVec3f(size/2, 0, size/2-wallThickness/2+r),
        
        // north wall top
        ofVec3f(-size/2, wallHeight+r, size/2+wallThickness/2+r),
        ofVec3f(size/2, wallHeight+r, size/2+wallThickness/2+r),
        ofVec3f(-size/2, wallHeight+r, size/2-wallThickness/2+r),
        ofVec3f(size/2, wallHeight+r, size/2-wallThickness/2+r),
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
        {5, 4, 1, 1, 4, 0},  // north
        {7, 5, 3, 3, 5, 1},  // east
        {6, 7, 2, 2, 7, 3},  // south
        {4, 6, 0, 0, 6, 2}}; // west
    
    ofVec2f sideUvs[] = {ofVec2f(0, 0), ofVec2f(0.8, 0), ofVec2f(0, 0.8),
        ofVec2f(0, 0.8), ofVec2f(0.8, 0), ofVec2f(0.8, 0.8)};
    ofVec2f topUvs[] = {ofVec2f(0.81, 0.81), ofVec2f(1, 0.81), ofVec2f(0.81, 1),
        ofVec2f(0.81, 1), ofVec2f(1, 0.81), ofVec2f(1, 1)};

    int vertexCount = 0;
    
    ofMatrix4x4 translate = ofMatrix4x4::newTranslationMatrix(ofVec3f(x*size+size/2, 0, y*size+size/2));
    
    if (bottom) {
        for (int w=0; w<5; w++) {
            for (int i=0; i<6; i++) {
                normals.push_back(wallNormals[w]);
                verts.push_back(wallPoints[wallIndexes[w][i]] * translate);
                if (w==0) {
                    uvs.push_back(topUvs[i]);
                }
                else {
                    uvs.push_back(sideUvs[i]);
                }
                vertexCount++;
            }
        }
    }
    
    if (left) {
        ofMatrix4x4 rotate = ofMatrix4x4::newRotationMatrix(90, ofVec3f(0, 1, 0));
        for (int w=0; w<5; w++) {
            for (int i=0; i<6; i++) {
                normals.push_back(rotate * wallNormals[w]);
                verts.push_back(rotate * wallPoints2[wallIndexes[w][i]] * translate);
                if (w==0) {
                    uvs.push_back(topUvs[i]);
                }
                else {
                    uvs.push_back(sideUvs[i]);
                }
                vertexCount++;
            }
        }
    }

    return vertexCount;
}

