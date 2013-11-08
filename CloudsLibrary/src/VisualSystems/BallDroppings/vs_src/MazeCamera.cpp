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
    setFov(60);
    
    yRot = xRot = 0;
    
    vel = ofVec3f(0, 0, 1);
}

void MazeCamera::setPath(ofxSimpleSpline *p)
{
    path = p;
    pathT = 0;
}

void MazeCamera::update()
{
    if (ParamManager::getInstance().groundCam) {
        setPosition(path->getPoint(pathT));
        lookAt(path->getPoint(pathT+0.005*ParamManager::getInstance().groundCamLookAt), ofVec3f(0, 1, 0));
        pathT += 0.0001 * ParamManager::getInstance().groundCamSpeed;
        rotate(yRot, ofVec3f(0, 1, 0));
    }
    else {
        setPosition(getPosition().x, ParamManager::getInstance().cameraHeight, getPosition().z);
        vel = ofVec3f(0, 0, ParamManager::getInstance().cameraSpeed);
        ofVec3f lookVec = ofVec3f(0, 0, 10);
        lookVec.rotate(xRot, ofVec3f(1, 0, 0));
        lookAt(getPosition() + lookVec);
        rotate(zRot, ofVec3f(0, 1, 0));
        move(vel*ofGetLastFrameTime());
    }
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

void MazeCamera::mouseMove(ofVec2f p)
{
    if (ParamManager::getInstance().groundCam) {
        float coeff = (float)-180/ofGetWidth();
        p.x -= ofGetWidth()/2;
        yRot = p.x*coeff;
        
        coeff = (float)120/ofGetHeight();
        p.y -= ofGetHeight()/2;
        xRot = coeff*p.y;
    }
    else {
        float coeff = (float)-45/ofGetWidth();
        p.x -= ofGetWidth()/2;
        zRot = coeff*p.x;
        
        coeff = (float)45/ofGetHeight();
        p.y -= ofGetHeight()/2;
        xRot = ParamManager::getInstance().cameraAngle + coeff*p.y;
    }
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