//
//  Maze.cpp
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#include "Maze.h"

Maze::Maze(float cSize, float wThickness, float wHeight)
{
    cellSize = cSize;
    wallThickness = wThickness;
    wallHeight = wHeight;
    
    for (int i=0; i<NUM_CELLS_X; i++) {
        for (int j=0; j<NUM_CELLS_Y; j++)
        {
            cells[i][j] = new MazeCell(i, j, cellSize,
                wallThickness, wallHeight);
        }
    }
    
    currentCell = NULL;
    exitCell = NULL;
    
    finishedGenerating = false;
    finishedSolving = false;
    step = 0;
    currentYLimit = 40;
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
    int randX = 0;//(int)ofRandom(NUM_CELLS_X);
    int randY = 0;//(int)ofRandom(NUM_CELLS_Y);
    currentCell = cells[randX][randY];
    currentCell->visit();
    currentCell->mazeExit = true;
    cellStack.push(currentCell);
    exitCell = currentCell;
    finishedGenerating = false;
    
    // generate the maze
//    while (!finishedGenerating) {
//        generateStep();
//    }
}

void Maze::update(ofCamera *cam)
{
    if (!finishedGenerating) {
        for (int i=0; i<8; i++) {
            generateStep();
        }
    }
}

void Maze::draw(ofCamera *cam)
{
    // draw the ground
    ofFill();
    ofSetColor(ofColor::fromHsb(
            ParamManager::getInstance().groundColor.r,
            ParamManager::getInstance().groundColor.g,
            ParamManager::getInstance().groundColor.b));
    
    ofPushMatrix();
    ofTranslate(NUM_CELLS_X*cellSize/2, -wallHeight/2, NUM_CELLS_Y*cellSize/2);
    ofScale(NUM_CELLS_X*cellSize, 1, NUM_CELLS_Y*cellSize);
    ofBox(1);
    ofPopMatrix();
    
    
    // draw the cells
    int yStart = cam->getPosition().z/cellSize;
    int yLimit = min(yStart+60, NUM_CELLS_Y);
    
    for (int i=0; i<NUM_CELLS_X; i++)
    {
        for (int j=yStart; j<yLimit; j++)
        {
            // make the visual effect of the maze beeing generated.
            if (cells[i][j]->visible == false) {
                if (j > yLimit-2) {
                    if (ofRandom(80) < 1) {
                        cells[i][j]->visible = true;
                    }
                }
                else {
                    cells[i][j]->visible = true;
                }
            }
            cells[i][j]->draw(currentCell == cells[i][j]);
        }
    }
}

float Maze::getWidth()
{
    return NUM_CELLS_X*cellSize;
}

void Maze::generateStep()
{
    if (finishedGenerating) { return; }
    bool valid = false;
    std::vector<int> available_dirs;
    for (int i=0;i<4;i++)
    {
        available_dirs.push_back(i);
        
        
    }
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
                if (cury < min(NUM_CELLS_Y-1,currentYLimit-1) && cells[curx][cury+1]->notVisited()) {
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
                int prevLimit = currentYLimit;
                currentYLimit += 40;
                currentCell = cells[0][prevLimit+1];
//                finishedGenerating = true; 
//                currentCell = NULL;
            }
            valid = true;
        }
    } // end while
}


