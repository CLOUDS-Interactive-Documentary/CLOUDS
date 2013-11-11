//
//  MazeCamera.cpp
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#include "MazeCamera.h"

MazeCamera::MazeCamera(float x, float y, float z, MazeSettings* set) : ofCamera()
{
    settings = set;
    setPosition(x, y, z);
    lookAt(ofVec3f(x, y, z+10));
    rotate(settings->cameraAngle, ofVec3f(1, 0, 0));
    setFov(60);
    
    xRot = yRot = zRot = 0;
    
    vel = ofVec3f(0, 0, 1);
}

void MazeCamera::setPath(ofxSimpleSpline *p)
{
    path = p;
    pathT = 0;
    xRot = yRot = zRot = 0;
}

void MazeCamera::setFlyOver(float x)
{
    setPosition(x, settings->cameraHeight, 100);
    
    // face mouse centered
    mouseMove(ofVec2f(ofGetMouseX(), ofGetMouseY()));
    update();
    
}

void MazeCamera::update()
{
    if (settings->groundCam) {
        setPosition(path->getPoint(pathT));
        lookAt(path->getPoint(pathT+0.005*settings->groundCamLookAt), ofVec3f(0, 1, 0));
        pathT += 0.01 * settings->groundCamSpeed * ofGetLastFrameTime();
        rotate(yRot, ofVec3f(0, 1, 0));
        if (pathT >= 1) {
            pathT = 0;
        }
    }
    else {
        setPosition(getPosition().x, settings->cameraHeight, getPosition().z);
        vel = ofVec3f(0, 0, settings->cameraSpeed);
        ofVec3f lookVec = ofVec3f(0, 0, 10);
        lookVec.rotate(xRot, ofVec3f(1, 0, 0));
        lookAt(getPosition() + lookVec);
        rotate(yRot, ofVec3f(0, 1, 0));
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
    if (settings->groundCam) {
        float coeff = (float)-90/ofGetWidth();
        p.x -= ofGetWidth()/2;
        yRot = p.x*coeff;
        
        coeff = (float)120/ofGetHeight();
        p.y -= ofGetHeight()/2;
        xRot = coeff*p.y;
    }
    else {
        float coeff = (float)-45/ofGetWidth();
        p.x -= ofGetWidth()/2;
        yRot = coeff*p.x;
        
        coeff = (float)45/ofGetHeight();
        p.y -= ofGetHeight()/2;
        xRot = settings->cameraAngle + coeff*p.y;
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