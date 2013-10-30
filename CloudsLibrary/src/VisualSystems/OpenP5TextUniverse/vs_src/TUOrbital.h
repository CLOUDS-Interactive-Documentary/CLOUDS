//
//  TUOrbital.h
//  OpenP5TextUniverse
//
//  Created by Elie Zananiri on 2013-10-29.
//
//

#pragma once

#include "ofMain.h"
#include "ofxFTGL.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class TUOrbital
{
    public:
        TUOrbital(float size, float radius);
        TUOrbital(TUOrbital& parent, string text);
    
        void update(float x, float y, float z, bool bSelected);
        void draw(ofCamera& cam, bool bMouseDragged);
        void billboard();
        bool isMouseover(ofCamera& cam);
    
        static float focusX, focusY, focusZ;
        static ofxFTGLSimpleLayout font;
        static ofColor textColor;
    
        static ofColor lineColor;
        static float lineWidth;
    
        static ofColor nodeColor;
        static float nodeScalar;
    
        ofVec3f pos;
    
        float radius, size;
        
        string text;
        
        vector<TUOrbital> children;
        
        bool bSelected;
        bool bClicked;
        bool bRenderText;
    
        int color;
};


