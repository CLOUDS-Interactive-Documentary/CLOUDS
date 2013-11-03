//
//  DM10PingPongBuffer.h
//  OpenP5DrawingMachine10
//
//  Created by Elie Zananiri on 2013-11-02.
//
//

#pragma once

#include "ofMain.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
struct DM10PingPongBuffer
{
    public:
        void allocate(int width, int height, int internalformat = GL_RGBA);
        void swap();
        void clear();
        ofFbo& operator[](int n);
        
        ofFbo * src;
        ofFbo * dst;
        
    private:
        ofFbo   FBOs[2];
        int     flag;
};
