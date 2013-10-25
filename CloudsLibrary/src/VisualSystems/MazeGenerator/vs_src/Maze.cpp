//
//  Maze.cpp
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#include "Maze.h"

Maze::Maze()
{
    for (int i=0; i<NUM_CELLS_X; i++) {
        for (int j=0; j<NUM_CELLS_Y; j++)
        {
            cells[i][j] = new MazeCell(i, j, CELL_SIZE, 2, 10);
        }
    }
    
    currentCell = NULL;
    exitCell = NULL;
    
    finishedGenerating = false;
    finishedSolving = false;
    step = 0;
}

Maze::~Maze()
{
    for (int i=0; i<NUM_CELLS_X; i++)
    {
        for (int j=0; j<NUM_CELLS_Y; j++)
        {
            delete cells[i][j];
        }
    }
}

void Maze::generate()
{
    // set starting (exit) point
    int randX = (int)ofRandom(NUM_CELLS_X);
    int randY = (int)ofRandom(NUM_CELLS_Y);
    currentCell = cells[randX][randY];
    currentCell->visit();
    currentCell->mazeExit = true;
    cellStack.push(currentCell);
    exitCell = currentCell;
    finishedGenerating = false;
    
    // generate the maze
    while (!finishedGenerating) {
        generateStep();
    }
}

void Maze::draw()
{
    ofPushMatrix();
    ofTranslate(-NUM_CELLS_X*CELL_SIZE/2, 50, 0);
    
    for (int i=0; i<NUM_CELLS_X; i++)
    {
        for (int j=0; j<NUM_CELLS_Y; j++)
        {
            cells[i][j]->draw();
        }
    }
    
    ofPopMatrix();
}

void Maze::generateStep()
{
    if (finishedGenerating) { return; }
    bool valid = false;
    std::vector<int> available_dirs;
    for (int i=0;i<4;i++) { available_dirs.push_back(i); }
    while (!valid) {
        int curx = currentCell->getX();
        int cury = currentCell->getY();
        int dir_idx = int(ofRandom(available_dirs.size()));
        int dir = available_dirs[dir_idx];
        
        switch(dir) {
            case 0:
                if (cury > 0 && cells[curx][cury-1]->notVisited()) {
                    cout<<"Going north"<<endl;
                    currentCell->top = false;
                    currentCell = cells[curx][cury-1];
                    currentCell->visit();
                    currentCell->bottom = false;
                    cellStack.push(currentCell);
                    valid = true;
                } else {
                    available_dirs.erase(available_dirs.begin() + dir_idx);
                }
                break;
            case 1:
                if (curx < NUM_CELLS_X-1 && cells[curx+1][cury]->notVisited()) {
                    cout<<"Going east"<<endl;
                    currentCell->right = false;
                    currentCell = cells[curx+1][cury];
                    currentCell->visit();
                    currentCell->left = false;
                    cellStack.push(currentCell);
                    valid = true;
                } else {
                    available_dirs.erase(available_dirs.begin() + dir_idx);
                }
                break;
            case 2:
                if (cury < NUM_CELLS_Y-1 && cells[curx][cury+1]->notVisited()) {
                    cout<<"Going south"<<endl;
                    currentCell->bottom = false;
                    currentCell = cells[curx][cury+1];
                    currentCell->visit();
                    currentCell->top = false;
                    cellStack.push(currentCell);
                    valid = true;
                } else {
                    available_dirs.erase(available_dirs.begin() + dir_idx);
                }
                break;
            case 3:
                if (curx > 0 && cells[curx-1][cury]->notVisited()) {
                    cout<<"Going west"<<endl;
                    currentCell->left = false;
                    currentCell = cells[curx-1][cury];
                    currentCell->visit();
                    currentCell->right = false;
                    cellStack.push(currentCell);
                    valid = true;
                } else {
                    available_dirs.erase(available_dirs.begin() + dir_idx);
                }
                break;
        }  // end switch
        if (!valid && available_dirs.size() == 0) {
            if (!cellStack.empty()) {
                currentCell = cellStack.top();
                cellStack.pop();
            }
            else {
                finishedGenerating = true;
                currentCell = NULL;
            }
            valid = true;
        }
    } // end while
}


