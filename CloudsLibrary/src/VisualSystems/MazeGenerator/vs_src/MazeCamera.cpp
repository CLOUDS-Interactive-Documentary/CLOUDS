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
    rotate(37, ofVec3f(1, 0, 0));
    setFov(30);
    
    vel = ofVec3f(0, 0, 1);
    acc = ofVec3f();
}

void MazeCamera::update()
{
//    acc = ;
    vel = ofVec3f(0, 0, 2 + abs((float)ofGetMouseY()/2)/100);
    cout<<vel.z<<endl;
    setFov(90 - vel.z*5);
    move(vel);
    acc = ofVec3f();
}

void MazeCamera::setVelocity(ofVec3f v)
{
    vel = v;
}