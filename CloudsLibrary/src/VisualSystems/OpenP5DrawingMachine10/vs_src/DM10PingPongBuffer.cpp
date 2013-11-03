//
//  DM10PingPongBuffer.cpp
//  OpenP5DrawingMachine10
//
//  Created by Elie Zananiri on 2013-11-02.
//
//

#include "DM10PingPongBuffer.h"

//--------------------------------------------------------------
void DM10PingPongBuffer::allocate(int width, int height, int internalformat)
{
    for (int i = 0; i < 2; i++) {
        FBOs[i].allocate(width, height, internalformat);
        FBOs[i].getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    }
    
    clear();
    
    // Set everything to 0.
    flag = 0;
    swap();
    flag = 0;
}

//--------------------------------------------------------------
void DM10PingPongBuffer::swap()
{
    src = &(FBOs[(flag) % 2]);
    dst = &(FBOs[++(flag) % 2]);
}

//--------------------------------------------------------------
void DM10PingPongBuffer::clear()
{
    for (int i = 0; i < 2; i++) {
        FBOs[i].begin();
        {
            ofClear(0, 255);
        }
        FBOs[i].end();
    }
}

//--------------------------------------------------------------
ofFbo& DM10PingPongBuffer::operator[](int n)
{
    return FBOs[n];
}
