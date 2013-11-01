//
//  DM10Pencil.cpp
//  OpenP5DrawingMachine10
//
//  Created by Elie Zananiri on 2013-11-01.
//
//

#include "DM10Pencil.h"

//--------------------------------------------------------------
DM10Pencil::DM10Pencil(ofVec2f pos)
{
    _pos = pos;
}

//--------------------------------------------------------------
void DM10Pencil::update(ofVec2f& force, float strength)
{
    ofVec2f to = _pos - force;
    float dF = to.length();
    to.normalize();
    to *= strength / dF;
    _pos += to;
}

//--------------------------------------------------------------
void DM10Pencil::draw()
{
    glVertex2f(_pos.x, _pos.y);
}
