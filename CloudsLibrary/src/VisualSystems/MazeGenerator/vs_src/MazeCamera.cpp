//
//  MazeCamera.cpp
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#include "MazeCamera.h"

MazeCamera::MazeCamera(float x, float y, float z) : ofCamera()
{
    setPosition(x, y, z);
    lookAt(ofVec3f(x, y, z+10));
    rotate(25, ofVec3f(1, 0, 0));
}

void MazeCamera::update()
{
    move(0, 0, 1.0f);
}