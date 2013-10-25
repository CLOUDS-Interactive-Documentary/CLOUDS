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
#include "MazeCamera.h"
#include "MazeCell.h"

#define NUM_CELLS_X 31
#define NUM_CELLS_Y 1000
#define CELL_SIZE 30
#define WALL_HEIGHT 20
#define WALL_THICKNESS 3

class Maze
{
public:
    Maze();
    ~Maze();
    void generate();
    
    void draw(int y);
    
    float getWidth();
    
    
protected:
    void generateStep();
    
    MazeCell* cells[NUM_CELLS_X][NUM_CELLS_Y];
    std::stack<MazeCell*> cellStack;
    int step;
    MazeCell* currentCell;
    MazeCell* exitCell;

    bool finishedGenerating;
    bool finishedSolving;
};

#endif /* defined(__MazeGenerator__Maze__) */
