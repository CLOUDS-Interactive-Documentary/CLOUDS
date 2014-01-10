//
//  ColorManager.cpp
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#include "ParamManager2.h"

ParamManager2::ParamManager2()
{
    cameraSpeed = 1;
    cameraAngle = 45;
    cameraHeight = 180;
    showAhead = 60;

}

ParamManager2::~ParamManager2()
{
    
}

ofColor ParamManager2::getGroundColor()
{
    return ofColor::fromHsb(groundColor.r, groundColor.g, groundColor.b);
}

ofColor ParamManager2::getWallColor()
{
    return ofColor::fromHsb(wallColor.r, wallColor.g, wallColor.b);
}

ofColor ParamManager2::getSideWallsColor()
{
    return ofColor::fromHsb(sideWallsColor.r, sideWallsColor.g, sideWallsColor.b);
}

ofColor ParamManager2::getGeneratorColor()
{
    return ofColor::fromHsb(generatorColor.r, generatorColor.g, generatorColor.b);
}