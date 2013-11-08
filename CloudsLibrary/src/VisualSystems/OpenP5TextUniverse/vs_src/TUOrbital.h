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
#include "ofxTween.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
enum TURevealMode
{
    REVEAL_INSTANT,
    REVEAL_LEVELS,
    REVEAL_ORDERED,
    REVEAL_RANDOM
};

//--------------------------------------------------------------
//--------------------------------------------------------------
class TUOrbital
{
    public:
        TUOrbital(float size, float radius);
        TUOrbital(TUOrbital * parent, string& text);
        ~TUOrbital();
    
        unsigned int restart(unsigned int delay);
        void update(float x, float y, float z);
        void draw(ofCamera& cam);
    
        static int numOrbitals;
    
        static int billboardType;
    
        static TURevealMode revealMode;
        static float minFadeTime;
        static float maxFadeTime;
        static float minLineTime;
        static float maxLineTime;
    
        static ofxFTGLSimpleLayout font;
        static ofxFTGLTextAlignment textAlign;
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
    
        ofxTween fadeTween;
        ofxTween lineTween;
    
        float textAlpha;
        float nodeAlpha;
    
        ofVec3f pos;
        float radius, size;
        
        string text;
        bool bRenderText;
    
        TUOrbital * parent;
        vector<TUOrbital *> children;
};


