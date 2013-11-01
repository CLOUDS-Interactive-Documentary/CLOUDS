//
//  DM10Bunch.cpp
//  OpenP5DrawingMachine10
//
//  Created by Elie Zananiri on 2013-11-01.
//
//

#include "DM10Bunch.h"

//--------------------------------------------------------------
DM10Bunch::DM10Bunch(int xg, int yg, float factor)
{
    _factor = factor;
    
    _cols = ofGetWidth() / xg;
    _rows = ofGetHeight() / yg;
    
    for (int i = 0; i < _cols * _rows; i++) {
        _pencils.push_back(new DM10Pencil(ofVec3f(i % _cols * xg, floorf(i / _cols) * yg)));
    }
}

//--------------------------------------------------------------
DM10Bunch::~DM10Bunch()
{
    for (int i = 0; i < _pencils.size(); i++) {
        delete _pencils[i];
    }
    _pencils.clear();
}

//--------------------------------------------------------------
void DM10Bunch::update(vector<ofVec2f>& attractors)
{
    for (int i = 0; i < _pencils.size(); i++) {
        for (int j = 0; j < attractors.size(); j++) {
            _pencils[i]->update(attractors[j], _factor);
        }
    }
}

//--------------------------------------------------------------
void DM10Bunch::update(vector<ofVec2f>& attractors,float maxDist)
{
    for (int i = 0; i < _pencils.size(); i++) {
        for (int j = 0; j < attractors.size(); j++) {
//            float dist = attractors[j].distance(_pencils[i]->getPos());
//            if (dist < maxDist) {
                _pencils[i]->update(attractors[j], _factor);
//            }
        }
    }
}

//--------------------------------------------------------------
void DM10Bunch::drawPoints()
{
    glBegin(GL_POINTS);
    for (int i = 0; i < _pencils.size(); i++) {
        _pencils[i]->draw();
    }
    glEnd();
}

//--------------------------------------------------------------
void DM10Bunch::drawLines()
{
    for (int i = 0; i < _rows; i++) {
        glBegin(GL_LINE_STRIP);
        for (int j = 0; j < _cols; j++) {
            int idx = (i * _cols) + j;
            glVertex2f(_pencils[idx]->getPos().x, _pencils[idx]->getPos().y);
        }
        glEnd();
    }
}
