//
//  MazeSettings.cpp
//  MazeGenerator
//
//  Created by Gal Sasson on 11/10/13.
//
//

#include "MazeSettings.h"

MazeSettings::MazeSettings()
{
    cameraSpeed = 1;
    cameraAngle = 45;
    cameraHeight = 180;
    showAhead = 60;
    
    numberOfBalls = 30;
    ballRadius = 15;
    ballMaxSpeed = 0.001;
    
    heightRandom = 0;
    
    groundCam = false;
    groundCamSpeed = 0;
    groundCamLookAt = 0.5;
    
    fogDensity = 0.1;
    fogStart = 1;
    fogEnd = 5;
}

ofFloatColor MazeSettings::getFogColor()
{
    return ofColor::fromHsb(fogColor.r, fogColor.g, fogColor.b);
}

ofColor MazeSettings::getGroundColor()
{
    return ofColor::fromHsb(groundColor.r, groundColor.g, groundColor.b);
}

ofColor MazeSettings::getWallColor()
{
    return ofColor::fromHsb(wallColor.r, wallColor.g, wallColor.b);
}

ofColor MazeSettings::getBallColor()
{
    return ofColor::fromHsb(ballColor.r, ballColor.g, ballColor.b);
}


