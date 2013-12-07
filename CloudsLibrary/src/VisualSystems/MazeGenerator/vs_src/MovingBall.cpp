//
//  MovingBall.cpp
//  MazeGenerator
//
//  Created by Gal Sasson on 11/4/13.
//
//

#include "MovingBall.h"

MovingBall::MovingBall(ofxSimpleSpline* s, MazeSettings *set)
{
    settings = set;
    
    spline = s;
    time = 0;
    noiseT = ofRandom(10000);
    speed = 1;
    pos = spline->getPoint(time);
}

MovingBall::~MovingBall()
{
    delete spline;
}

void MovingBall::update()
{
    if (time > 1) {
        speed = -1;
        time = 1;
    }
    else if (time <= 0) {
        speed = 1;
        time = 0;
    }
    
    time += speed*ofNoise(noiseT)*settings->ballMaxSpeed;
    pos = spline->getPoint(time);
    
    noiseT += 0.1;
}

void MovingBall::draw()
{
    ofSetColor(settings->getBallColor());
    ofFill();
    ofSphere(pos, settings->ballRadius);
    
    // draw the path
//    ofSetColor(0, 255, 255);
//    spline->draw();
}