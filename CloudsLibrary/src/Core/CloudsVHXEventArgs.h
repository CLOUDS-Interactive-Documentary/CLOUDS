//
//  CloudsVHXEventArgs.h
//  CLOUDS
//
//  Created by Elias Zananiri on 2015-05-11.
//
//

#pragma once

#include "ofMain.h"

//--------------------------------------------------------------
class CloudsVHXEventArgs
: ofEventArgs
{
public:
    CloudsVHXEventArgs()
    : ofEventArgs(), success(false), result("")
    {}
    
    bool success;
    string result;
};
