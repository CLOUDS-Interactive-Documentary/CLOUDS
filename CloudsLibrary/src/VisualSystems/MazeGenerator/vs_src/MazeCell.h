//
//  Cell.h
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#ifndef __MazeGenerator__MazeCell__
#define __MazeGenerator__MazeCell__

#include <ofMain.h>

class MazeCell
{
public:
    float size;
    float wallThickness;
    float wallHeight;
    bool top,left,right,bottom;
    bool mazeExit;
    bool searchStart;
    bool visible;

    MazeCell(int x, int y, float s, float thickness, float height);
    
    unsigned long long addGeometry(vector<ofVec3f> &verts, vector<ofVec3f> &normals, vector<ofVec2f> &uvs);
    
    int getX() { return x; }
    int getY() { return y; }
    void visit() { visited = true; }
    void search() { searched = true; }
    void unsearch() { searched = false; searchStart = false; }
    bool notVisited() { return !visited; }
    bool notSearched() { return !searched; }

private:
    int x, y;
    bool visited;
    bool searched;
    float wallLength;
    
};


#endif /* defined(__MazeGenerator__MazeCell__) */
