//
//  ColorManager.cpp
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#include "ParamManager.h"

ParamManager::ParamManager()
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

ParamManager::~ParamManager()
{
    
}

ofFloatColor ParamManager::getFogColor()
{
    return ofColor::fromHsb(fogColor.r, fogColor.g, fogColor.b);
}

ofColor ParamManager::getGroundColor()
{
    return ofColor::fromHsb(groundColor.r, groundColor.g, groundColor.b);
}

ofColor ParamManager::getWallColor()
{
    return ofColor::fromHsb(wallColor.r, wallColor.g, wallColor.b);
}

ofColor ParamManager::getBallColor()
{
    return ofColor::fromHsb(ballColor.r, ballColor.g, ballColor.b);
}

