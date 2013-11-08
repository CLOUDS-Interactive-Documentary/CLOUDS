//
//  MovingBall.h
//  MazeGenerator
//
//  Created by Gal Sasson on 11/4/13.
//
//

#ifndef __MazeGenerator__MovingBall__
#define __MazeGenerator__MovingBall__

#include <iostream>
#include "ofMain.h"
#include "ofxSimpleSpline.h"
#include "ParamManager.h"

class MovingBall
{
public:
    MovingBall(ofxSimpleSpline* s);
    ~MovingBall();
    
    void update();
    void draw();
    
private:
    ofxSimpleSpline* spline;
    float time;
    ofVec3f pos;
    float speed;
    
    float noiseT;
};


#endif /* defined(__MazeGenerator__MovingBall__) */
