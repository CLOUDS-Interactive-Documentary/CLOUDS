//
//  Maze.cpp
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#include "Maze.h"

#define SIDE_WALLS

Maze::Maze(float cSize, float wThickness, float wHeight, MazeSettings* set, ofVec3f p)
{
    cellSize = cSize;
    wallThickness = wThickness;
    wallHeight = wHeight;
    settings = set;
    pos = p;
    
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

    for (int i=0; i<balls.size(); i++)
    {
        delete balls[i];
    }
    balls.clear();
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
    
    buildModel();
    
    // create moving balls with paths
    for (int i=0; i<settings->numberOfBalls; i++)
    {
        int sx = (int)ofRandom(60, NUM_CELLS_X-60);
        int sy = (int)ofRandom(10, 500);
        ofxSimpleSpline* spline = createSimpleSpline(sx, sy, 100);
        balls.push_back(new MovingBall(spline, settings));
    }
}

void Maze::buildModel()
{
    vector<ofVec3f> vertices;
    vector<ofVec3f> normals;
    vector<ofVec2f> uvs;
    vertexCount = 0;
    
    for (int j=0; j<NUM_CELLS_Y; j++) {
        vertexIndexForLines[j] = vertexCount;
        for (int i=0; i<NUM_CELLS_X; i++) {
            vertexCount += cells[i][j]->addGeometry(vertices, normals, uvs);
        }
    }

    geometry.setVertexData(&vertices[0], vertices.size(), GL_STATIC_DRAW);
    geometry.setNormalData(&normals[0], normals.size(), GL_STATIC_DRAW);
    geometry.setTexCoordData(&uvs[0], uvs.size(), GL_STATIC_DRAW);

	vertices.clear();
	normals.clear();
    uvs.clear();
}

void Maze::update(ofCamera *cam)
{
    for (int i=0; i<balls.size(); i++)
    {
        balls[i]->update();
    }
}

void Maze::draw(ofCamera *cam, ofVec3f &lightPos)
{
    ofPushMatrix();
    ofTranslate(pos);
    
    int camCellY = (int)(cam->getPosition().z / cellSize);
    camCellY = max(0, camCellY-30);
    int lastVisibleLine = (int)MIN((int)camCellY + settings->showAhead, NUM_CELLS_Y);
    
    // draw surface
    float length = (float)lastVisibleLine - camCellY;
    float middle = (float)camCellY + length/2;
    
    // draw the ground
    ofFill();
    ofSetColor(settings->getGroundColor());
    ofPushMatrix();
    ofTranslate(NUM_CELLS_X*cellSize/2, -wallHeight/2, middle*cellSize);
    ofScale(NUM_CELLS_X*cellSize+120, 1, length*cellSize);
    ofBox(2);
    ofPopMatrix();
    
    // draw maze geometry
    ofSetColor(settings->getWallColor());
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	geometry.draw(GL_TRIANGLES, vertexIndexForLines[camCellY], vertexIndexForLines[lastVisibleLine]-vertexIndexForLines[camCellY]);
	glDisable(GL_CULL_FACE);

    // draw moving balls
    for (int i=0; i<balls.size(); i++)
    {
        balls[i]->draw();
    }
    
    ofPopMatrix();
}

float Maze::getWidth()
{
    return NUM_CELLS_X*cellSize;
}

ofxSimpleSpline* Maze::createSimpleSpline(int sx, int sy, int length)
{
    bool cellSearched[NUM_CELLS_X][NUM_CELLS_Y] = {false};
    std::stack<MazeCell*> solveStack;

    MazeCell* cell = cells[sx][sy];
    solveStack.push(cell);
    
    for (int i=0; i<length; i++)
    {
        // this is one step
        
        // add available direction from the current cell
        vector<int> dirs;
        if (!cell->top) dirs.push_back(0);
        if (!cell->right) dirs.push_back(1);
        if (!cell->bottom) dirs.push_back(2);
        if (!cell->left) dirs.push_back(3);

        bool found = false;
        while(!found)
        {
            int x = cell->getX();
            int y = cell->getY();
            int dirIdx = (int)ofRandom(dirs.size());
            int dir = dirs[dirIdx];
            
            switch (dir) {
                case 0:
                    if (y>0 && !cellSearched[x][y-1]) {
                        cell = cells[x][y-1];
                        solveStack.push(cell);
                        cellSearched[x][y-1] = true;
                        found=true;
                    }
                    else {
                        dirs.erase(dirs.begin() + dirIdx);
                    }
                    break;
                case 1:
                    if (x<NUM_CELLS_X-1 && !cellSearched[x+1][y]) {
                        cell = cells[x+1][y];
                        solveStack.push(cell);
                        cellSearched[x+1][y] = true;
                        found=true;
                    }
                    else {
                        dirs.erase(dirs.begin() + dirIdx);
                    }
                    break;
                case 2:
                    if (y<NUM_CELLS_Y-1 && !cellSearched[x][y+1]) {
                        cell = cells[x][y+1];
                        solveStack.push(cell);
                        cellSearched[x][y+1] = true;
                        found=true;
                    }
                    else {
                        dirs.erase(dirs.begin() + dirIdx);
                    }
                    break;
                case 3:
                    if (x>0 && !cellSearched[x-1][y]) {
                        cell = cells[x-1][y];
                        solveStack.push(cell);
                        cellSearched[x-1][y] = true;
                        found=true;
                    }
                    else {
                        dirs.erase(dirs.begin() + dirIdx);
                    }
                    break;
            }
            
            if (!found && dirs.size()==0) {
                // we got to an end point, pop from the stack (backtracing)
                i--;
                solveStack.pop();
                cell = solveStack.top();
                found = true;
            }
        }
    }

    // create the path curve based on solveStack
    vector<ofVec3f> points;

    while (!solveStack.empty())
    {
        cell = solveStack.top();
        cout<<"cell["<<cell->getX()<<"]["<<cell->getY()<<"]\n";
        points.push_back(ofVec3f(cell->getX()*cellSize+cellSize/2,
                                 settings->ballRadius,
                                 cell->getY()*cellSize+cellSize/2));
        solveStack.pop();
    }

    ofxSimpleSpline* curve = new ofxSimpleSpline();
    curve->setSubdivisions(10);
    curve->addControlVertices(points);
    curve->update();
    
    return curve;
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
        int dir_idx = (int)ofRandom(available_dirs.size());
        int dir = available_dirs[dir_idx];
        
        switch(dir) {
            case 0:
                if (cury > 0 && cells[curx][cury-1]->notVisited()) {
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


