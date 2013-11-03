//
//  Maze.cpp
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#include "Maze.h"

#define SIDE_WALLS

Maze::Maze(float cSize, float wThickness, float wHeight, ofVec3f p)
{
    cellSize = cSize;
    wallThickness = wThickness;
    wallHeight = wHeight;
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
}

void Maze::buildModel()
{
    vector<ofVec3f> vertices;
    vector<ofVec3f> normals;
    vertexCount = 0;
    
    for (int j=0; j<NUM_CELLS_Y; j++) {
        vertexIndexForLines[j] = vertexCount;
        for (int i=0; i<NUM_CELLS_X; i++) {
            vertexCount += cells[i][j]->addGeometry(vertices, normals);
        }
    }

    geometry.setVertexData(&vertices[0], vertices.size(), GL_STATIC_DRAW);
    geometry.setNormalData(&normals[0], normals.size(), GL_STATIC_DRAW);

	vertices.clear();
	normals.clear();
    
    // setup boxes locations
    for (int i=0; i<1000; i++)
    {
        blocks.push_back(ofVec3f(ofRandom(0, NUM_CELLS_X*cellSize),0, ofRandom(0, NUM_CELLS_Y*cellSize)));
    }
}

void Maze::update(ofCamera *cam)
{
}

void Maze::draw(ofCamera *cam)
{
    ofPushMatrix();
    ofTranslate(pos);
    
    int camCellY = (int)(cam->getPosition().z / cellSize);
    camCellY = max(0, camCellY-10);
    int lastVisibleLine = (int)MIN((int)camCellY + ParamManager::getInstance().showAhead, NUM_CELLS_Y);
    
    // draw surface
    float length = (float)lastVisibleLine - camCellY;
    float middle = (float)camCellY + length/2;
    
    // draw the ground
    ofFill();
    ofSetColor(ParamManager::getInstance().getGroundColor());
    ofPushMatrix();
    ofTranslate(NUM_CELLS_X*cellSize/2, -wallHeight/2, middle*cellSize);
    ofScale(NUM_CELLS_X*cellSize+120, 1, length*cellSize);
    ofBox(1);
    ofPopMatrix();
    
    // draw maze geometry
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	geometry.draw(GL_TRIANGLES, vertexIndexForLines[camCellY], vertexIndexForLines[lastVisibleLine]);
	glDisable(GL_CULL_FACE);
    
    // draw random blocks inside the maze
    for (int i=0; i<blocks.size(); i++)
    {
        ofPushMatrix();
        ofTranslate(blocks[i]+ofVec3f(0, 20, 0));
        ofSetColor(255);
        ofBox(40);
        ofPopMatrix();
    }
    
//    for (int j=0; j<NUM_CELLS_Y; j++)
//    {
//        for (int i=0; i<NUM_CELLS_X; i++)
//        {
//            cells[i][j]->draw(false);
//        }
//    }
	
    ofPopMatrix();
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


