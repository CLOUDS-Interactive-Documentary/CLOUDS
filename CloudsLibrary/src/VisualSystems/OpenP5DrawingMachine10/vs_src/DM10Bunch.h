//
//  DM10Bunch.h
//  OpenP5DrawingMachine10
//
//  Created by Elie Zananiri on 2013-11-01.
//
//

#pragma once

#include "ofMain.h"
#include "DM10Pencil.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class DM10Bunch
{
    public:
        DM10Bunch(int xg, int yg, float factor);
        ~DM10Bunch();
    
        void update(vector<ofVec2f>& attractors);
        void update(vector<ofVec2f>& attractors, float maxDist);
    
        void drawPoints();
        void drawLines();
    
    private:
        vector<DM10Pencil *> _pencils;
    
        float _factor;
        int _cols, _rows;
};
