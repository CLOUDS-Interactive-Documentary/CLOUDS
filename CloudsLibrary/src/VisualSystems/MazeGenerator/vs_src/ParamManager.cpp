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

}

ParamManager::~ParamManager()
{
    
}

ofColor ParamManager::getGroundColor()
{
    return ofColor::fromHsb(groundColor.r, groundColor.g, groundColor.b);
}

ofColor ParamManager::getWallColor()
{
    return ofColor::fromHsb(wallColor.r, wallColor.g, wallColor.b);
}

ofColor ParamManager::getSideWallsColor()
{
    return ofColor::fromHsb(sideWallsColor.r, sideWallsColor.g, sideWallsColor.b);
}

ofColor ParamManager::getGeneratorColor()
{
    return ofColor::fromHsb(generatorColor.r, generatorColor.g, generatorColor.b);
}