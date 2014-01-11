//
//  Maze.h
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#ifndef __MazeGenerator__Maze__
#define __MazeGenerator__Maze__

#include <ofMain.h>
#include "MazeCamera2.h"
#include "MazeCell2.h"
#include "ParamManager2.h"

#define NUM_CELLS_X2 61
#define NUM_CELLS_Y2 10000

class Maze2
{
public:
    float cellSize;
    float wallThickness;
    float wallHeight;
    
    Maze2(float cSize, float wThick, float wHeight, ofVec3f p = ofVec3f());
    ~Maze2();
    void generate();
    
    void update(ofCamera* cam);
    void draw(ofCamera* cam);
    
    float getWidth();
    
    
protected:
    void generateStep();
    
    ofVec3f pos;
    ofVbo geometry;
    
    MazeCell2* cells[NUM_CELLS_X2][NUM_CELLS_Y2];
    std::stack<MazeCell2*> cellStack;
    int step;
    int currentYLimit;
    MazeCell2* currentCell;
    MazeCell2* exitCell;

    bool finishedGenerating;
    bool finishedSolving;
};

#endif /* defined(__MazeGenerator__Maze__) */
