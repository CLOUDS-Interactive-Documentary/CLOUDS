//
//  MazeCamera.h
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#ifndef __MazeGenerator__MazeCamera__
#define __MazeGenerator__MazeCamera__

#include <ofMain.h>
#include "ParamManager2.h"
#include "Maze2.h"

class MazeCamera2 : public ofCamera
{
public:
    
    MazeCamera2(float x, float y, float z);
    
    void update();
    void draw();
    
    void setVelocity(ofVec3f vel);
    
private:
    ofVec3f vel;
    ofVec3f acc;
    float fov;
    ofVec3f mousePos;
    
    void applyLimits(ofVec3f &p);
};

#endif /* defined(__MazeGenerator__MazeCamera__) */
