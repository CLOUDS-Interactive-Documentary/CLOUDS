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
#include "ParamManager.h"

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
    
    void draw(bool isGenerator);
    
    size_t getVertexCount();
    size_t getFaceCount();
    size_t fillVertexData(ofVec3f *data, int index);
    size_t fillIndexData(ofIndexType *data, int vertStartIndex, int index);
    size_t fillNormalData(ofVec3f *data, int index);
    
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
