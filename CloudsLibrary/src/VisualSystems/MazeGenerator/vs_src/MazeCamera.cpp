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
    rotate(50, ofVec3f(1, 0, 0));
    setFov(100);
    
    vel = ofVec3f(0, 0, 1);
    acc = ofVec3f();
}

void MazeCamera::update()
{
    vel = ofVec3f(0, 0, ParamManager::getInstance().cameraSpeed);
    move(vel);
}

void MazeCamera::setVelocity(ofVec3f v)
{
    vel = v;
}