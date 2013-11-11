//
//  MazeSettings.h
//  MazeGenerator
//
//  Created by Gal Sasson on 11/10/13.
//
//

#ifndef __MazeGenerator__MazeSettings__
#define __MazeGenerator__MazeSettings__

#include <iostream>
#include "ofMain.h"

class MazeSettings
{
public:
    MazeSettings();
    
    ofFloatColor fogColor;
    ofFloatColor groundColor;
    ofFloatColor wallColor;
    ofFloatColor ballColor;
    
    ofFloatColor getFogColor();
    ofColor getGroundColor();
    ofColor getWallColor();
    ofColor getBallColor();
    
    float cameraSpeed;
    float cameraAngle;
    float cameraHeight;
    float showAhead;
    
    float numberOfBalls;
    float ballRadius;
    float ballMaxSpeed;
    
    float heightRandom;
    
    bool groundCam;
    float groundCamSpeed;
    float groundCamLookAt;
    
    float fogDensity;
    float fogStart;
    float fogEnd;
};

#endif /* defined(__MazeGenerator__MazeSettings__) */
