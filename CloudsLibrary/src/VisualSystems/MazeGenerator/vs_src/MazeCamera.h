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

class MazeCamera : public ofCamera
{
public:
    
    MazeCamera(float x, float y, float z);
    
    void update();
    
    void setVelocity(ofVec3f vel);
    
private:
    ofVec3f vel;
    ofVec3f acc;
    float fov;
};

#endif /* defined(__MazeGenerator__MazeCamera__) */
