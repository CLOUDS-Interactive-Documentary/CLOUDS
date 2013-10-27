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
    rotate(ParamManager::getInstance().cameraAngle, ofVec3f(1, 0, 0));
    setFov(100);
    
    vel = ofVec3f(0, 0, 1);
}

//#define FLYING_CAM

void MazeCamera::update()
{
#ifdef FLYING_CAM
    vel = (mousePos-getPosition()).normalize() * ParamManager::getInstance().cameraSpeed;
    lookAt(mousePos);
#else
    vel = ofVec3f(0, 0, ParamManager::getInstance().cameraSpeed);
    lookAt(getPosition() + ofVec3f(0, 0, 10));
#endif
    rotate(ParamManager::getInstance().cameraAngle, ofVec3f(1, 0, 0));
    move(vel);
    
    float mazeWidth = 1860;
    float mazeHeight = 600;
    mousePos = ofVec3f((1-(float)ofGetMouseX()/ofGetWidth()) * mazeWidth, 200 + (1-(float)ofGetMouseY()/ofGetHeight()) * mazeHeight, getPosition().z+500);
}

void MazeCamera::draw()
{
    ofPushMatrix();
    ofTranslate(mousePos);
    ofSetColor(0, 255, 0);
    ofFill();
    ofBox(10);
    ofPopMatrix();
}

void MazeCamera::setVelocity(ofVec3f v)
{
    vel = v;
}

void MazeCamera::applyLimits(ofVec3f &p)
{
    if (p.x < 50) {
        p.x = 50;
    } else if (p.x > 30*60) {
        p.x = 30*60;
    }
    
    if (p.y < 60) {
        p.y = 60;
    }
    else if (p.y > 250) {
        p.y = 250;
    }
        

}