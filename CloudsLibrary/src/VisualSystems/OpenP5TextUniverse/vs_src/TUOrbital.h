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
        TUOrbital(TUOrbital * parent, string& text);
        ~TUOrbital();
    
        void update(float x, float y, float z);
        void draw(ofCamera& cam);
    
        static int billboardType;
    
        static ofxFTGLSimpleLayout font;
        static string fontName;
        static float lineLength;
        static float fontSize;
        static float fontDepth;
        static bool bAllCaps;
        static ofColor textColor;
    
        static ofColor lineColor;
        static float lineWidth;
    
        static ofColor nodeColor;
        static float nodeScalar;
    
        ofVec3f pos;
        float radius, size;
        
        string text;
        bool bRenderText;
    
        TUOrbital * parent;
        vector<TUOrbital *> children;
};


