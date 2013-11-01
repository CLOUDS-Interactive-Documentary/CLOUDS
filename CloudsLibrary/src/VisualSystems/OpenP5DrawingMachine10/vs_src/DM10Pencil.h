//
//  DM10Pencil.h
//  OpenP5DrawingMachine10
//
//  Created by Elie Zananiri on 2013-11-01.
//
//

#pragma once

#include "ofMain.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class DM10Pencil
{
    public:
        DM10Pencil(ofVec2f pos);
    
        void update(ofVec2f& force, float strength);
        void draw();
    
        ofVec2f& getPos() { return _pos; }
        
        int getHue() { return _hue; }
        void setHue(int hue) { _hue = hue; }
    
    private:
        ofVec2f _pos;
        int _hue;
    
    
    
};
