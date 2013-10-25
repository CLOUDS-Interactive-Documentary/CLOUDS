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

class MazeCamera : public ofCamera
{
public:
    
    MazeCamera(float x, float y, float z);
    
    void update();
};

#endif /* defined(__MazeGenerator__MazeCamera__) */
