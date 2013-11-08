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
#include "ParamManager.h"
#include "Maze.h"
#include "ofxSimpleSpline.h"

class MazeCamera : public ofCamera
{
public:
    
    MazeCamera(float x, float y, float z);
    
    void setPath(ofxSimpleSpline *p);
    
    void update();
    void draw();
    
    void setVelocity(ofVec3f vel);
    
    void mouseMove(ofVec2f p);
    
private:
    ofVec3f vel;
    ofVec3f acc;
    float fov;
    ofVec3f mousePos;
    
    ofxSimpleSpline* path;
    float pathT;
    float xRot, yRot, zRot;
    
    void applyLimits(ofVec3f &p);
};

#endif /* defined(__MazeGenerator__MazeCamera__) */
